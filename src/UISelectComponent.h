#pragma once

#include <flecs/flecs.h>
#include <glm/vec2.hpp>
#include <optional>

namespace xg
{
    struct UISelectComponent
    {
        flecs::entity m_SelectEntity;
        std::optional<glm::vec2> m_BoxStart;
        bool m_SelectBox = false;
    };
}
