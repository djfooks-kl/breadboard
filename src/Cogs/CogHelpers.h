#pragma once

#include <glm/fwd.hpp>

#include "Core/Rotation90.h"

namespace xg::cog
{
    template<typename TFn>
    void ForEachCellUntil(const glm::ivec2& position, const glm::ivec2& size, xc::Rotation90 rotation, TFn fn)
    {
        for (int x = 0; x <= size.x - 1; ++x)
        {
            for (int y = 0; y <= size.y - 1; ++y)
            {
                glm::ivec2 offset(x, y);
                offset = rotation.GetIMatrix() * offset;
                const glm::ivec2 p = position + offset;
                if (fn(p))
                {
                    return;
                }
            }
        }
    }
}