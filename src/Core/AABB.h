#pragma once

#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>

#include "Core/ITransform.h"

namespace xc
{
    struct AABBCenterExtents
    {
        glm::vec2 m_Center{ 0.f, 0.f };
        glm::vec2 m_HalfExtents{ 0.f, 0.f };
    };

    struct AABB
    {
        static AABB FromPoints(const glm::vec2& p1, const glm::vec2& p2);

        bool Contains(const glm::vec2& p) const;

        bool Overlaps(const xc::AABB& other) const;

        [[nodiscard]] glm::vec2 ClampPointInside(const glm::vec2& p) const;

        [[nodiscard]] xc::AABBCenterExtents ToCenterExtents() const;

        bool operator==(const xc::AABB& other) const = default;

        // min values are inclusive
        glm::vec2 m_Min{ 1.f, 1.f };
        // max values are inclusive
        glm::vec2 m_Max{ -1.f, -1.f };
    };

    std::ostream& operator<<(std::ostream& os, const xc::AABB& obj);
}