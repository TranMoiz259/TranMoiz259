#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "voxel/Block.hpp"

namespace voxel::gameplay {

struct ItemStack {
    voxel::voxel_world::BlockId id{voxel::voxel_world::BlockId::Air};
    int count{};
};

class Inventory {
public:
    explicit Inventory(std::size_t hotbarSize = 9, std::size_t capacity = 36);

    bool add(ItemStack stack);
    bool remove(voxel::voxel_world::BlockId id, int count);
    [[nodiscard]] int count(voxel::voxel_world::BlockId id) const;

    [[nodiscard]] std::optional<ItemStack> hotbarSlot(std::size_t index) const;
    [[nodiscard]] std::size_t hotbarSize() const;

    void selectHotbar(std::size_t index);
    [[nodiscard]] std::size_t selectedHotbar() const;
    [[nodiscard]] std::optional<ItemStack> selectedStack() const;

    [[nodiscard]] bool craft(voxel::voxel_world::BlockId output);

private:
    std::vector<ItemStack> slots_;
    std::size_t hotbarSize_;
    std::size_t selectedHotbar_{0};
    std::unordered_map<voxel::voxel_world::BlockId, std::vector<ItemStack>> recipes_;
};

}  // namespace voxel::gameplay
