#pragma once

#include <unordered_map>
#include <vector>
#include <flecs/flecs.h>

#include "VectorHashFunctions.h"
#include "WireFlags.h"

namespace xg
{
    struct GridAttachments
    {
        std::vector<flecs::entity> m_Entities;
        TWireDirectionFlags m_WireDirectionFlags;
        bool m_HasNode : 1 = false;
        bool m_HasWireCheckpoint : 1 = false;
    };

    using GridAttachmentsMap = std::unordered_map<glm::ivec2, GridAttachments>;
}
