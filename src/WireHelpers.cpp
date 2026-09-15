#include "WireHelpers.h"

#include <glm/common.hpp>

#include "Core/OBB.h"
#include "GridAttachmentsMap.h"
#include "WireFlags.h"
#include <glm/geometric.hpp>

namespace
{
    const xg::TWireDirectionFlags s_SameDirectionFlags0   = xg::TWireDirectionFlags{} | xg::EWireDirection::N | xg::EWireDirection::S;
    const xg::TWireDirectionFlags s_SameDirectionFlags45  = xg::TWireDirectionFlags{} | xg::EWireDirection::NE | xg::EWireDirection::SW;
    const xg::TWireDirectionFlags s_SameDirectionFlags90  = xg::TWireDirectionFlags{} | xg::EWireDirection::E | xg::EWireDirection::W;
    const xg::TWireDirectionFlags s_SameDirectionFlags135 = xg::TWireDirectionFlags{} | xg::EWireDirection::SE | xg::EWireDirection::NW;
}

xg::EWireDirection xg::WireDirectionToFlag(const glm::ivec2& v)
{
    if (v.y > 0)
    {
        return
            v.x > 0 ? xg::EWireDirection::NE :
            v.x < 0 ? xg::EWireDirection::NW :
            xg::EWireDirection::N;
    }
    else if (v.y < 0)
    {
        return
            v.x > 0 ? xg::EWireDirection::SE :
            v.x < 0 ? xg::EWireDirection::SW :
            xg::EWireDirection::S;
    }
    else
    {
        return
            v.x > 0 ? xg::EWireDirection::E :
            v.x < 0 ? xg::EWireDirection::W :
            xg::EWireDirection::None;
    }
}

bool xg::HasWireDot(const xg::GridAttachments& attachments)
{
    return attachments.m_HasWireCheckpoint ||
        (attachments.m_WireDirectionFlags.HasAny() && attachments.m_HasNode);
}

xc::OBB xg::GetWireSegmentOBB(const glm::ivec2& start, const glm::ivec2& end, const float halfWidth)
{
    xc::OBB obb;
    const glm::vec2 v(end - start);
    const float length = glm::length(v);
    obb.m_Center = (glm::vec2(start) + glm::vec2(end)) * 0.5f;
    if (length == 0.f)
    {
        obb.m_HalfExtents.x = 0.f;
        obb.m_HalfExtents.y = 0.f;
        obb.m_AxisX = glm::vec2(1.f, 0.f);
        obb.m_AxisY = glm::vec2(0.f, 1.f);
    }
    else
    {
        obb.m_HalfExtents.x = halfWidth;
        obb.m_HalfExtents.y = length * 0.5f;
        obb.m_AxisY = v / length;
        obb.m_AxisX = glm::vec2(obb.m_AxisY.y, -obb.m_AxisY.x);
    }
    return obb;
}