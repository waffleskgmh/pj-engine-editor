#include "gameplay/crafting.h"

namespace PJEngine {

void CraftingSystem::addRecipe(const Recipe& recipe) {
    recipes[recipe.id] = recipe;
}

bool CraftingSystem::canCraft(const std::string& recipeId, const Inventory& inv) const {
    auto it = recipes.find(recipeId);
    if (it == recipes.end()) return false;

    for (const auto& ing : it->second.ingredients) {
        if (!inv.hasItem(ing.itemId, ing.count)) {
            return false;
        }
    }
    return true;
}

bool CraftingSystem::craft(const std::string& recipeId, Inventory& inv) {
    if (!canCraft(recipeId, inv)) return false;

    const auto& rec = recipes[recipeId];
    for (const auto& ing : rec.ingredients) {
        inv.removeItem(ing.itemId, ing.count);
    }
    return inv.addItem(rec.resultItem, rec.resultCount);
}

ShopSystem::ShopSystem(uint32_t cap)
    : shopInventory(cap, 10000.0f) {}

bool ShopSystem::buyItem(const std::string& itemId, Inventory& playerInv, uint32_t& playerGold) {
    if (!shopInventory.hasItem(itemId, 1)) return false;

    uint32_t cost = 10;
    for (const auto& slot : shopInventory.slots) {
        if (!slot.isEmpty() && slot.item.id == itemId) {
            cost = static_cast<uint32_t>(slot.item.value * buyMultiplier);
            break;
        }
    }

    if (playerGold < cost) return false;

    ItemDefinition def;
    for (const auto& slot : shopInventory.slots) {
        if (!slot.isEmpty() && slot.item.id == itemId) {
            def = slot.item;
            break;
        }
    }

    if (!playerInv.addItem(def, 1)) return false;

    playerGold -= cost;
    shopInventory.removeItem(itemId, 1);
    return true;
}

bool ShopSystem::sellItem(const std::string& itemId, Inventory& playerInv, uint32_t& playerGold) {
    if (!playerInv.hasItem(itemId, 1)) return false;

    uint32_t gain = 5;
    ItemDefinition def;
    for (const auto& slot : playerInv.slots) {
        if (!slot.isEmpty() && slot.item.id == itemId) {
            def = slot.item;
            gain = static_cast<uint32_t>(slot.item.value * sellMultiplier);
            break;
        }
    }

    playerInv.removeItem(itemId, 1);
    playerGold += gain;
    shopInventory.addItem(def, 1);
    return true;
}

} // namespace PJEngine