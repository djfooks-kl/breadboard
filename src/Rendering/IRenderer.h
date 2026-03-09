#pragma once

#include <glm/fwd.hpp>

namespace xc
{
    class Rotation90;
}

namespace xg
{
    struct IRenderer
    {
        virtual ~IRenderer() = default;

        virtual void AddRenderable(
            const glm::ivec2& position,
            const xc::Rotation90 rotation) = 0;

        virtual void RemoveAll() = 0;

        virtual void Draw(const glm::mat4& viewProjection, const float feather) = 0;
    };
}
