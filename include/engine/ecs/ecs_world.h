#pragma once

#include "engine/ecs/entity.h"
#include "engine/ecs/component_pool.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include <functional>

namespace PJEngine {

class ECSWorld {
public:
    ECSWorld();
    ~ECSWorld() = default;

    // Správa entít
    Entity createEntity();
    void destroyEntity(Entity entity);
    bool isAlive(Entity entity) const;
    size_t getEntityCount() const;

    // Správa komponentov
    template <typename T, typename... Args>
    T& addComponent(Entity entity, Args&&... args) {
        assert(isAlive(entity) && "Entita uz nezije!");
        ComponentPool<T>* pool = getOrCreatePool<T>();
        T component(std::forward<Args>(args)...);
        T& ref = pool->insert(entity, component);

        uint32_t index = getEntityIndex(entity);
        signatures[index].set(getComponentTypeID<T>());
        return ref;
    }

    template <typename T>
    void removeComponent(Entity entity) {
        assert(isAlive(entity) && "Entita uz nezije!");
        ComponentPool<T>* pool = getOrCreatePool<T>();
        pool->remove(entity);

        uint32_t index = getEntityIndex(entity);
        signatures[index].reset(getComponentTypeID<T>());
    }

    template <typename T>
    T& getComponent(Entity entity) {
        assert(isAlive(entity) && "Entita uz nezije!");
        ComponentPool<T>* pool = getOrCreatePool<T>();
        return pool->get(entity);
    }

    template <typename T>
    const T& getComponent(Entity entity) const {
        assert(isAlive(entity) && "Entita uz nezije!");
        const ComponentPool<T>* pool = getPool<T>();
        assert(pool && "Pool neexistuje!");
        return pool->get(entity);
    }

    template <typename T>
    bool hasComponent(Entity entity) const {
        if (!isAlive(entity)) return false;
        uint32_t index = getEntityIndex(entity);
        return signatures[index].test(getComponentTypeID<T>());
    }

    // Filtrovanie entít podľa komponentov (Queries)
    template <typename... Components>
    std::vector<Entity> query() {
        ComponentSignature mask;
        (mask.set(getComponentTypeID<Components>()), ...);

        std::vector<Entity> matchingEntities;
        for (uint32_t i = 0; i < entityCount; ++i) {
            if (aliveEntities[i] && (signatures[i] & mask) == mask) {
                matchingEntities.push_back(makeEntity(i, generations[i]));
            }
        }
        return matchingEntities;
    }

    // Priama rýchla iterácia v systémoch
    template <typename... Components, typename Func>
    void forEach(Func&& func) {
        auto entities = query<Components...>();
        for (Entity e : entities) {
            func(e, getComponent<Components>(e)...);
        }
    }

    void clear();

private:
    uint32_t entityCount = 0;
    std::vector<uint8_t> generations;
    std::vector<bool> aliveEntities;
    std::vector<ComponentSignature> signatures;
    std::queue<uint32_t> freeEntityIndices;

    std::unordered_map<ComponentTypeID, std::shared_ptr<IComponentPool>> componentPools;

    template <typename T>
    ComponentPool<T>* getOrCreatePool() {
        ComponentTypeID typeID = getComponentTypeID<T>();
        auto it = componentPools.find(typeID);
        if (it == componentPools.end()) {
            auto pool = std::make_shared<ComponentPool<T>>();
            componentPools[typeID] = pool;
            return pool.get();
        }
        return static_cast<ComponentPool<T>*>(it->second.get());
    }

    template <typename T>
    const ComponentPool<T>* getPool() const {
        ComponentTypeID typeID = getComponentTypeID<T>();
        auto it = componentPools.find(typeID);
        if (it != componentPools.end()) {
            return static_cast<const ComponentPool<T>*>(it->second.get());
        }
        return nullptr;
    }
};

} // namespace PJEngine