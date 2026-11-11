#pragma once

#include "gameplay/inventory.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace PJEngine {

struct Ingredient {
    std::string itemId;
    uint32_t count = 1;
};

struct Recipe {
    std::string id;
    ItemDefinition resultItem;
    uint32_t resultCount = 1;
    std::string stationRequired = "none";
    std::vector<Ingredient> ingredients;
};

class CraftingSystem {
public:
    std::unordered_map<std::string, Recipe> recipes;

    void addRecipe(const Recipe& recipe);
    bool canCraft(const std::string& recipeId, const Inventory& inv) const;
    bool craft(const std::string& recipeId, Inventory& inv);
};

class ShopSystem {
public:
    Inventory shopInventory;
    float buyMultiplier = 1.0f;
    float sellMultiplier = 0.5f;

    explicit ShopSystem(uint32_t capacity = 50);

    bool buyItem(const std::string& itemId, Inventory& playerInv, uint32_t& playerGold);
    bool sellItem(const std::string& itemId, Inventory& playerInv, uint32_t& playerGold);
};

} // namespace PJEngine