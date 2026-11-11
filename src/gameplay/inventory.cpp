#include "gameplay/inventory.h"
#include <cstdlib>
#include <algorithm>

namespace PJEngine {

void LootTable::addEntry(const ItemDefinition& item, float weight, uint32_t minCount, uint32_t maxCount) {
    entries.push_back({item, weight, minCount, maxCount});
}

std::vector<InventorySlot> LootTable::roll() const {
    std::vector<InventorySlot> loot;
    if (entries.empty()) return loot;

    float totalWeight = 0.0f;
    for (const auto& e : entries) totalWeight += e.weight;

    float rnd = (static_cast<float>(rand()) / RAND_MAX) * totalWeight;
    float current = 0.0f;

    for (const auto& e : entries) {
        current += e.weight;
        if (rnd <= current) {
            uint32_t count = e.minCount + (rand() % (e.maxCount - e.minCount + 1));
            loot.push_back({e.item, count});
            break;
        }
    }
    return loot;
}

Inventory::Inventory(uint32_t cap, float maxW)
    : capacity(cap), maxWeight(maxW) {
    slots.resize(capacity);
}

bool Inventory::addItem(const ItemDefinition& item, uint32_t count) {
    if (getTotalWeight() + (item.weight * count) > maxWeight) {
        return false;
    }

    uint32_t remaining = count;

    // 1. Stackovanie do existujúcich slotov
    for (auto& slot : slots) {
        if (!slot.isEmpty() && slot.item.id == item.id && slot.count < slot.item.maxStack) {
            uint32_t canAdd = std::min(remaining, slot.item.maxStack - slot.count);
            slot.count += canAdd;
            remaining -= canAdd;
            if (remaining == 0) return true;
        }
    }

    // 2. Vloženie do nového prázdneho slotu
    for (auto& slot : slots) {
        if (slot.isEmpty()) {
            uint32_t canAdd = std::min(remaining, item.maxStack);
            slot.item = item;
            slot.count = canAdd;
            remaining -= canAdd;
            if (remaining == 0) return true;
        }
    }

    return remaining == 0;
}

bool Inventory::removeItem(const std::string& itemId, uint32_t count) {
    if (!hasItem(itemId, count)) return false;

    uint32_t toRemove = count;
    for (auto& slot : slots) {
        if (!slot.isEmpty() && slot.item.id == itemId) {
            if (slot.count >= toRemove) {
                slot.count -= toRemove;
                if (slot.count == 0) slot = InventorySlot{};
                return true;
            } else {
                toRemove -= slot.count;
                slot = InventorySlot{};
            }
        }
    }
    return false;
}

bool Inventory::hasItem(const std::string& itemId, uint32_t count) const {
    return getItemCount(itemId) >= count;
}

uint32_t Inventory::getItemCount(const std::string& itemId) const {
    uint32_t total = 0;
    for (const auto& slot : slots) {
        if (!slot.isEmpty() && slot.item.id == itemId) {
            total += slot.count;
        }
    }
    return total;
}

float Inventory::getTotalWeight() const {
    float total = 0.0f;
    for (const auto& slot : slots) {
        if (!slot.isEmpty()) {
            total += slot.item.weight * slot.count;
        }
    }
    return total;
}

bool EquipmentSystem::equip(Slot slot, const ItemDefinition& item) {
    equipped[slot] = item;
    return true;
}

void EquipmentSystem::unequip(Slot slot) {
    equipped.erase(slot);
}

bool EquipmentSystem::hasEquipped(Slot slot) const {
    return equipped.find(slot) != equipped.end();
}

const ItemDefinition* EquipmentSystem::getEquipped(Slot slot) const {
    auto it = equipped.find(slot);
    return it != equipped.end() ? &it->second : nullptr;
}

float EquipmentSystem::getStatTotal(const std::string& statName) const {
    float sum = 0.0f;
    for (const auto& kv : equipped) {
        auto it = kv.second.stats.find(statName);
        if (it != kv.second.stats.end()) {
            sum += it->second;
        }
    }
    return sum;
}

} // namespace PJEngine