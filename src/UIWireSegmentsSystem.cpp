#include "UIWireSegmentsSystem.h"

#include <flecs/flecs.h>

#include "GridHelpers.h"
#include "UIWirePathComponent.h"
#include "UIWireSegmentsChangedComponent.h"
#include "UIWireSegmentsComponent.h"

namespace
{
    std::vector<glm::ivec2> GetCheckpoints(const std::vector<glm::ivec2>& path)
    {
        std::vector<glm::ivec2> checkpoints;
        checkpoints.clear();
        if (path.empty())
        {
            return checkpoints;
        }

        checkpoints.push_back(path[0]);
        if (path.size() == 1 || (path.size() == 2 && path[0] == path[1]))
        {
            return checkpoints;
        }

        glm::ivec2 prev = path[0];
        for (int i = 1; i < std::ssize(path) - 1; ++i)
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
        checkpoints.push_back(path[std::ssize(path) - 1]);
        return checkpoints;
    }
}

void xg::UIWireSegmentsSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIWireSegmentsChangedComponent&)
        {
            entity.remove<xg::UIWireSegmentsChangedComponent>();
        });
    world.defer_end();

    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIWirePathComponent& pathComponent)
        {
            const auto& path = pathComponent.m_Path;
            std::vector<glm::ivec2> checkpoints = GetCheckpoints(path);
            auto& checkpointsRef = entity.ensure<xg::UIWireSegmentsComponent>().m_Checkpoints;
            if (checkpointsRef != checkpoints)
            {
                checkpointsRef = checkpoints;
                entity.add<xg::UIWireSegmentsChangedComponent>();
            }
        });
    world.defer_end();
}