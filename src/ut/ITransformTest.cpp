#include "BreadTest.h"

#include "Core/ITransform.h"

TEST_CASE("Simple ITransform tests", "[xc::ITransform]")
{
    xc::ITransform transform { .m_Translation = glm::ivec2(1, 1), .m_Rotation = xc::Rotation90(1) };

    CHECK(transform.Apply(glm::ivec2(2, 3)) == glm::ivec2(4, -1));
    CHECK(transform.ApplyInverse(glm::ivec2(4, -1)) == glm::ivec2(2, 3));
}