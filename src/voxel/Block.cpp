#include "voxel/Block.hpp"

namespace voxel::voxel_world {

BlockRegistry::BlockRegistry() {
    defs_[0] = BlockDef{BlockId::Air, "air", false, true};
    defs_[1] = BlockDef{BlockId::Grass, "grass", true, false};
    defs_[2] = BlockDef{BlockId::Dirt, "dirt", true, false};
    defs_[3] = BlockDef{BlockId::Stone, "stone", true, false};
    defs_[4] = BlockDef{BlockId::Water, "water", false, true};
    defs_[5] = BlockDef{BlockId::Wood, "wood", true, false};
    defs_[6] = BlockDef{BlockId::Leaves, "leaves", true, true};
}

const BlockDef& BlockRegistry::get(BlockId id) const {
    return defs_[static_cast<std::size_t>(id)];
}

}  // namespace voxel::voxel_world
