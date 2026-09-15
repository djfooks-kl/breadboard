#include "CogHelpers.h"

#include "Core/IAABB.h"
#include "RenderSettings.h"

xc::AABB xg::GetCogAABB(const glm::ivec2& size, const xc::ITransform& transform, const xg::RenderSettings& renderSettings)
{
    const xc::IAABB iaabb = xc::IAABB::FromTransformAndSize(transform, size);
    return xc::AABB{
        .m_Min = glm::vec2(iaabb.m_Min) - glm::vec2(renderSettings.m_CogBoxSize),
        .m_Max = glm::vec2(iaabb.m_Max) + glm::vec2(renderSettings.m_CogBoxSize),
    };
}