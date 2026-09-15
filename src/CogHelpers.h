#pragma once

#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>

#include "Core/AABB.h"

namespace xc
{
    struct ITransform;
}

namespace xg
{
    struct RenderSettings;
}

namespace xg
{
    xc::AABB GetCogAABB(const glm::ivec2& size, const xc::ITransform& transform, const xg::RenderSettings& renderSettings);
}