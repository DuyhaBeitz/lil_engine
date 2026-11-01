#include "BackendAccess.hpp"
namespace Lil::Backend {
    static uint32_t s_generation = 1;
    uint32_t AssetGeneration()     { return s_generation; }
    void     BumpAssetGeneration() { ++s_generation; }
}
