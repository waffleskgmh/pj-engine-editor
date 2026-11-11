#pragma once

#include <cstdint>
#include <bitset>

namespace PJEngine {

using Entity = uint32_t;
constexpr Entity NULL_ENTITY = 0xFFFFFFFF;
constexpr size_t MAX_COMPONENTS = 64;
constexpr size_t MAX_ENTITIES = 100000;

using ComponentTypeID = uint32_t;
using ComponentSignature = std::bitset<MAX_COMPONENTS>;

// Pomocné funkcie pre generácie entít (24 bitov index, 8 bitov generácia)
inline uint32_t getEntityIndex(Entity e) {
    return e & 0x00FFFFFF;
}

inline uint32_t getEntityGeneration(Entity e) {
    return (e >> 24) & 0xFF;
}

inline Entity makeEntity(uint32_t index, uint32_t generation) {
    return (generation << 24) | (index & 0x00FFFFFF);
}

// Generátor unikátnych ID pre jednotlivé typy komponentov
inline ComponentTypeID getUniqueComponentID() {
    static ComponentTypeID nextID = 0;
    return nextID++;
}

template <typename T>
inline ComponentTypeID getComponentTypeID() {
    static ComponentTypeID typeID = getUniqueComponentID();
    return typeID;
}

} // namespace PJEngine