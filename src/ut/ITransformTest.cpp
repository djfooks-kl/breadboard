#include "BreadTest.h"

#include "Core/ITransform.h"

TEST_CASE("Simple ITransform tests", "[xc::ITransform]")
{
    xc::ITransform transformA { .m_Translation = glm::ivec2(1, 1), .m_Rotation = xc::Rotation90(1) };
    xc::ITransform transformB { .m_Translation = glm::ivec2(10, 10), .m_Rotation = xc::Rotation90(2) };

    CHECK(transformA.Apply(glm::ivec2(2, 3)) == glm::ivec2(4, -1));
    CHECK(transformA.ApplyInverse(glm::ivec2(4, -1)) == glm::ivec2(2, 3));

    CHECK((transformA * transformB).m_Rotation == xc::Rotation90(3));
    CHECK((transformA * transformB).m_Translation == glm::ivec2(11, -9));

    CHECK((transformB * transformA).m_Rotation == xc::Rotation90(3));
    CHECK((transformB * transformA).m_Translation == glm::ivec2(9, 9));

    CHECK((transformB * transformB).m_Rotation == xc::Rotation90(0));
    CHECK((transformB * transformB).m_Translation == glm::ivec2(0, 0));
}