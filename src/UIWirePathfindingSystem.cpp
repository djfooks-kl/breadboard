#include "UIWirePathfindingSystem.h"

#include <flecs/flecs.h>
#include <glm/geometric.hpp>

#include "Core/MathConstants.h"
#include "GridAttachmentsComponent.h"
#include "GridHelpers.h"
#include "UIPreviewWireComponent.h"
#include "UISettings.h"
#include "UIWirePathComponent.h"
#include "WorldMouseComponent.h"

namespace
{
    bool IsSimilarDirection(const int dir1, const int dir2)
    {
        return dir1 == 0 || dir2 == 0 || (dir1 == dir2);
    }

    bool IsSimilarDirection(const glm::ivec2& dir1, const glm::ivec2& dir2)
    {
        return IsSimilarDirection(dir1.x, dir2.x) &&
            IsSimilarDirection(dir1.y, dir2.y);
    }

    bool IsStraightLine(const glm::ivec2& p1, const glm::ivec2& p2)
    {
        glm::ivec2 dir = p2 - p1;
        if (dir.x == 0 || dir.y == 0 || std::abs(dir.x) == std::abs(dir.y))
            return true;
        return false;
    }
}

void xg::UIWirePathfindingSystem::Update(flecs::world& world)
{
    const auto& worldMouse = world.get<xg::WorldMouseComponent>();

    world.defer_begin();

    world.each([&](flecs::entity entity, const xg::UIPreviewWireComponent& preview)
        {
            auto& wirePathComponent = entity.ensure<xg::UIWirePathComponent>();
            const glm::vec2 offset = worldMouse.m_Position - glm::vec2(preview.m_Start);
            const float distance = glm::length(offset);
            if (distance <= world.get<xg::UISettings>().m_WireDirectionHintRadius || wirePathComponent.m_StartDirection == glm::ivec2(0))
            {
                const glm::vec2 dir = glm::normalize(offset);
                if (dir.y == 0.f)
                {
                    wirePathComponent.m_StartDirection = xg::SnapToGrid(dir);
                }
                else
                {
                    const float gradient = dir.x / dir.y;
                    const float absGradient = std::abs(gradient);
                    if (absGradient > ONE_OVER_SQRT_3 && absGradient < SQRT_3)
                    {
                        wirePathComponent.m_StartDirection = glm::ivec2(dir.x > 0.f ? 1 : -1, dir.y > 0.f ? 1 : -1);
                    }
                    else
                    {
                        wirePathComponent.m_StartDirection = xg::SnapToGrid(dir);
                    }
                }
            }

            std::vector<glm::ivec2>& path = wirePathComponent.m_Path;
            path.clear();
            const int cellDistance = xg::ChebyshevDistance(preview.m_Start, worldMouse.m_Cell);

            path.reserve(cellDistance + 1);
            glm::ivec2 prev = preview.m_Start;
            path.push_back(prev);

            bool useStartDirection = IsSimilarDirection(wirePathComponent.m_StartDirection, xg::GetIDirection(prev, worldMouse.m_Cell));
            while (prev != worldMouse.m_Cell)
            {
                glm::ivec2 dir;
                if (useStartDirection && !IsStraightLine(prev, worldMouse.m_Cell))
                {
                    dir = wirePathComponent.m_StartDirection;
                }
                else
                {
                    dir = xg::GetIDirection(prev, worldMouse.m_Cell);
                    useStartDirection = false;
                }
                const glm::ivec2 next = prev + dir;
                path.push_back(next);
                prev = next;
            }
        });
    world.defer_end();
}