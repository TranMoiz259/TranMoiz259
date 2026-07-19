#include "gameplay/Inventory.hpp"

#include <algorithm>

namespace voxel::gameplay {

Inventory::Inventory(std::size_t hotbarSize, std::size_t capacity)
    : slots_(capacity), hotbarSize_(std::min(hotbarSize, capacity)) {
    recipes_[voxel::voxel_world::BlockId::Wood] = {
        ItemStack{voxel::voxel_world::BlockId::Dirt, 1},
        ItemStack{voxel::voxel_world::BlockId::Grass, 1},
    };
    recipes_[voxel::voxel_world::BlockId::Leaves] = {
        ItemStack{voxel::voxel_world::BlockId::Wood, 1},
        ItemStack{voxel::voxel_world::BlockId::Grass, 2},
    };
}

bool Inventory::add(ItemStack stack) {
    if (stack.count <= 0 || stack.id == voxel::voxel_world::BlockId::Air) {
        return false;
    }

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

bool Inventory::remove(voxel::voxel_world::BlockId id, int countToRemove) {
    if (countToRemove <= 0) {
        return false;
    }

    int remaining = countToRemove;
    for (auto& slot : slots_) {
        if (slot.id != id || slot.count == 0) {
            continue;
        }

        const auto consumed = std::min(slot.count, remaining);
        slot.count -= consumed;
        remaining -= consumed;

        if (slot.count == 0) {
            slot.id = voxel::voxel_world::BlockId::Air;
        }

        if (remaining == 0) {
            return true;
        }
    }

    return false;
}

int Inventory::count(voxel::voxel_world::BlockId id) const {
    int total = 0;
    for (const auto& slot : slots_) {
        if (slot.id == id) {
            total += slot.count;
        }
    }
    return total;
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

std::size_t Inventory::hotbarSize() const {
    return hotbarSize_;
}

void Inventory::selectHotbar(std::size_t index) {
    if (index < hotbarSize_) {
        selectedHotbar_ = index;
    }
}

std::size_t Inventory::selectedHotbar() const {
    return selectedHotbar_;
}

std::optional<ItemStack> Inventory::selectedStack() const {
    return hotbarSlot(selectedHotbar_);
}

bool Inventory::craft(voxel::voxel_world::BlockId output) {
    auto it = recipes_.find(output);
    if (it == recipes_.end()) {
        return false;
    }

    for (const auto& input : it->second) {
        if (count(input.id) < input.count) {
            return false;
        }
    }

    for (const auto& input : it->second) {
        if (!remove(input.id, input.count)) {
            return false;
        }
    }
    return add(ItemStack{output, 1});
}

}  // namespace voxel::gameplay
