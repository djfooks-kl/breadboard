#include "UIWireSegmentsSystem.h"

#include <flecs/flecs.h>

#include "GridHelpers.h"
#include "UIWirePathComponent.h"
#include "UIWireSegmentsComponent.h"

void xg::UIWireSegmentsSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIWirePathComponent& pathComponent)
        {
            const auto& path = pathComponent.m_Path;
            auto& checkpoints = entity.ensure<xg::UIWireSegmentsComponent>().m_Checkpoints;
            checkpoints.clear();
            if (path.empty())
                return;

            checkpoints.push_back(path[0]);
            if (path.size() == 1 || (path.size() == 2 && path[0] == path[1]))
            {
                return;
            }

            glm::ivec2 prev = path[0];
            for (int i = 1; i < path.size() - 1; ++i)
            {
                const glm::ivec2& current = path[i];
                const glm::ivec2& next = path[i + 1];
                const glm::ivec2 prevDir = xg::GetIDirection(prev, current);
                const glm::ivec2 currentDir = xg::GetIDirection(current, next);
                if (prevDir != currentDir)
                {
                    checkpoints.push_back(path[i]);
                }
                prev = current;
            }
            checkpoints.push_back(path[path.size() - 1]);
        });
    world.defer_end();
}