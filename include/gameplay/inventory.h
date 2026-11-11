#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace PJEngine {

enum class ItemType {
    WEAPON,
    ARMOR,
    CONSUMABLE,
    MATERIAL,
    QUEST
};

enum class ItemRarity {
    COMMON,
    UNCOMMON,
    RARE,
    EPIC,
    LEGENDARY
};

struct ItemDefinition {
    std::string id;
    std::string name;
    std::string description;
    ItemType type = ItemType::MATERIAL;
    ItemRarity rarity = ItemRarity::COMMON;
    uint32_t maxStack = 64;
    float weight = 1.0f;
    uint32_t value = 10;
    std::unordered_map<std::string, float> stats;
};

struct InventorySlot {
    ItemDefinition item;
    uint32_t count = 0;

    bool isEmpty() const { return count == 0; }
};

struct LootEntry {
    ItemDefinition item;
    float weight = 1.0f;
    uint32_t minCount = 1;
    uint32_t maxCount = 1;
};

class LootTable {
public:
    std::vector<LootEntry> entries;

    void addEntry(const ItemDefinition& item, float weight, uint32_t minCount = 1, uint32_t maxCount = 1);
    std::vector<InventorySlot> roll() const;
};

class Inventory {
public:
    uint32_t capacity = 20;
    float maxWeight = 100.0f;
    std::vector<InventorySlot> slots;

    explicit Inventory(uint32_t capacity = 20, float maxWeight = 100.0f);

    bool addItem(const ItemDefinition& item, uint32_t count = 1);
    bool removeItem(const std::string& itemId, uint32_t count = 1);
    bool hasItem(const std::string& itemId, uint32_t count = 1) const;
    uint32_t getItemCount(const std::string& itemId) const;
    float getTotalWeight() const;
};

class EquipmentSystem {
public:
    enum class Slot {
        WEAPON,
        ARMOR,
        ACCESSORY
    };

    std::unordered_map<Slot, ItemDefinition> equipped;

    bool equip(Slot slot, const ItemDefinition& item);
    void unequip(Slot slot);
    bool hasEquipped(Slot slot) const;
    const ItemDefinition* getEquipped(Slot slot) const;
    float getStatTotal(const std::string& statName) const;
};

} // namespace PJEngine