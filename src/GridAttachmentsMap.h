#pragma once

#include <unordered_map>
#include <vector>
#include <flecs/flecs.h>

#include "VectorHashFunctions.h"

namespace xg
{
    struct GridAttachments
    {
        std::vector<flecs::entity> m_Entities;
        bool m_HasNode : 1 = false;
    };

    using GridAttachmentsMap = std::unordered_map<glm::ivec2, GridAttachments>;
}
