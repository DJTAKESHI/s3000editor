#pragma once

#include <array>
#include "S3000Types.h"

struct ModMatrix
{
    std::array<ModSlot, 6> slots;

    //// ===== core =====
    //float evaluate(const ModContext& ctx,
    //    ModDestination dest) const;

    //// optional helper
    //void clear();
};