#pragma once

#include <glm/ext/vector_int2.hpp>
#include <vector>

#include "WireFlags.h"

namespace xc
{
    struct OBB;
}

namespace xg
{
    struct GridAttachments;

    /// \brief Convert a vector to a wire direction flag
    xg::EWireDirection WireDirectionToFlag(const glm::ivec2& v);

    bool HasWireDot(const xg::GridAttachments& attachments);

    xc::OBB GetWireSegmentOBB(const glm::ivec2& start, const glm::ivec2& end, const float halfWidth);
}