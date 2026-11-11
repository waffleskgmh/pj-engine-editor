#include "engine/ecs/ecs_world.h"

namespace PJEngine {

ECSWorld::ECSWorld() {
    generations.resize(1024, 0);
    aliveEntities.resize(1024, false);
    signatures.resize(1024);
}

Entity ECSWorld::createEntity() {
    uint32_t index;

    if (!freeEntityIndices.empty()) {
        index = freeEntityIndices.front();
        freeEntityIndices.pop();
    } else {
        index = entityCount++;
        if (index >= generations.size()) {
            size_t newSize = generations.size() * 2;
            generations.resize(newSize, 0);
            aliveEntities.resize(newSize, false);
            signatures.resize(newSize);
        }
    }

    aliveEntities[index] = true;
    signatures[index].reset();

    return makeEntity(index, generations[index]);
}

void ECSWorld::destroyEntity(Entity entity) {
    if (!isAlive(entity)) return;

    uint32_t index = getEntityIndex(entity);
    aliveEntities[index] = false;
    signatures[index].reset();
    generations[index]++; // Zvýšenie generácie zneplatní všetky staré handly

    // Vyčistenie komponentov zo všetkých poolov
    for (auto& pair : componentPools) {
        pair.second->onEntityDestroyed(entity);
    }

    freeEntityIndices.push(index);
}

bool ECSWorld::isAlive(Entity entity) const {
    if (entity == NULL_ENTITY) return false;
    uint32_t index = getEntityIndex(entity);
    if (index >= entityCount) return false;
    return aliveEntities[index] && (generations[index] == getEntityGeneration(entity));
}

size_t ECSWorld::getEntityCount() const {
    return entityCount - freeEntityIndices.size();
}

void ECSWorld::clear() {
    for (uint32_t i = 0; i < entityCount; ++i) {
        if (aliveEntities[i]) {
            destroyEntity(makeEntity(i, generations[i]));
        }
    }
    entityCount = 0;
    while (!freeEntityIndices.empty()) {
        freeEntityIndices.pop();
    }
}

} // namespace PJEngine