#include "gameplay/Inventory.hpp"

#include <algorithm>

namespace voxel::gameplay {

Inventory::Inventory(std::size_t hotbarSize, std::size_t capacity)
    : slots_(capacity), hotbarSize_(std::min(hotbarSize, capacity)) {
    recipes_[voxel::voxel_world::BlockId::Wood] = {
        ItemStack{voxel::voxel_world::BlockId::Dirt, 1},
        ItemStack{voxel::voxel_world::BlockId::Grass, 1},
    };
}

bool Inventory::add(ItemStack stack) {
    for (auto& slot : slots_) {
        if (slot.id == stack.id && slot.count > 0) {
            slot.count += stack.count;
            return true;
        }
    }
    for (auto& slot : slots_) {
        if (slot.count == 0) {
            slot = stack;
            return true;
        }
    }
    return false;
}

bool Inventory::remove(voxel::voxel_world::BlockId id, int count) {
    for (auto& slot : slots_) {
        if (slot.id == id && slot.count >= count) {
            slot.count -= count;
            if (slot.count == 0) {
                slot.id = voxel::voxel_world::BlockId::Air;
            }
            return true;
        }
    }
    return false;
}

std::optional<ItemStack> Inventory::hotbarSlot(std::size_t index) const {
    if (index >= hotbarSize_) {
        return std::nullopt;
    }
    const auto& slot = slots_[index];
    if (slot.count == 0) {
        return std::nullopt;
    }
    return slot;
}

bool Inventory::craft(voxel::voxel_world::BlockId output) {
    auto it = recipes_.find(output);
    if (it == recipes_.end()) {
        return false;
    }

    for (const auto& input : it->second) {
        bool found = false;
        for (const auto& slot : slots_) {
            if (slot.id == input.id && slot.count >= input.count) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    for (const auto& input : it->second) {
        remove(input.id, input.count);
    }
    return add(ItemStack{output, 1});
}

}  // namespace voxel::gameplay
