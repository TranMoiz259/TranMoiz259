#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace voxel::voxel_world {

enum class BlockId : std::uint16_t {
    Air = 0,
    Grass,
    Dirt,
    Stone,
    Water,
    Wood,
    Leaves
};

struct BlockDef {
    BlockId id;
    std::string name;
    bool solid;
    bool transparent;
};

class BlockRegistry {
public:
    BlockRegistry();
    [[nodiscard]] const BlockDef& get(BlockId id) const;

private:
    std::array<BlockDef, 7> defs_{};
};

}  // namespace voxel::voxel_world
