#pragma once

#include "engine/ecs/entity.h"
#include <vector>
#include <cassert>

namespace PJEngine {

class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void onEntityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentPool : public IComponentPool {
public:
    ComponentPool() {
        sparse.resize(1024, NULL_ENTITY);
    }

    T& insert(Entity entity, const T& component) {
        uint32_t index = getEntityIndex(entity);
        if (index >= sparse.size()) {
            sparse.resize(index * 2 + 1, NULL_ENTITY);
        }

        assert(sparse[index] == NULL_ENTITY && "Entita uz ma tento komponent!");

        size_t denseIndex = denseComponents.size();
        sparse[index] = static_cast<Entity>(denseIndex);
        denseEntities.push_back(entity);
        denseComponents.push_back(component);

        return denseComponents.back();
    }

    void remove(Entity entity) {
        uint32_t index = getEntityIndex(entity);
        assert(index < sparse.size() && sparse[index] != NULL_ENTITY && "Komponent neexistuje na entite!");

        size_t indexOfRemoved = sparse[index];
        size_t lastIndex = denseComponents.size() - 1;

        // Swap with last element pre zachovanie suvisleho pola (O(1) zmazanie)
        if (indexOfRemoved != lastIndex) {
            denseComponents[indexOfRemoved] = denseComponents[lastIndex];
            denseEntities[indexOfRemoved] = denseEntities[lastIndex];
            sparse[getEntityIndex(denseEntities[lastIndex])] = static_cast<Entity>(indexOfRemoved);
        }

        denseComponents.pop_back();
        denseEntities.pop_back();
        sparse[index] = NULL_ENTITY;
    }

    T& get(Entity entity) {
        uint32_t index = getEntityIndex(entity);
        assert(index < sparse.size() && sparse[index] != NULL_ENTITY && "Komponent neexistuje na entite!");
        return denseComponents[sparse[index]];
    }

    const T& get(Entity entity) const {
        uint32_t index = getEntityIndex(entity);
        assert(index < sparse.size() && sparse[index] != NULL_ENTITY && "Komponent neexistuje na entite!");
        return denseComponents[sparse[index]];
    }

    bool has(Entity entity) const {
        uint32_t index = getEntityIndex(entity);
        return index < sparse.size() && sparse[index] != NULL_ENTITY;
    }

    void onEntityDestroyed(Entity entity) override {
        if (has(entity)) {
            remove(entity);
        }
    }

    // Priamy pristup k suvislemu polu pre bleskurychlu iteraciu v systemoch
    std::vector<T>& getDenseComponents() { return denseComponents; }
    std::vector<Entity>& getDenseEntities() { return denseEntities; }
    size_t size() const { return denseComponents.size(); }

private:
    std::vector<T> denseComponents;
    std::vector<Entity> denseEntities;
    std::vector<Entity> sparse;
};

} // namespace PJEngine