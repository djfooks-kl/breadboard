#include "BreadTest.h"

#include "Core/Rotation90.h"

TEST_CASE("Simple Rotation90 tests", "[xc::Rotation90]")
{
    {
        xc::Rotation90 direction;
        CHECK(direction.GetRotationIndex() == 0);
        CHECK(direction.GetIMatrix() == glm::imat2x2(1, 0, 0, 1));
        CHECK(direction.GetFMatrix() == glm::mat2x2(1.f, 0.f, 0.f, 1.f));
        CHECK(direction.Apply(glm::ivec2(1, 2)) == glm::ivec2(1, 2));
        CHECK(direction.ApplyInverse(glm::ivec2(1, 2)) == glm::ivec2(1, 2));
    }

    {
        xc::Rotation90 direction;
        direction.RotateClockwise(1);
        CHECK(direction.GetRotationIndex() == 1);
        CHECK(direction.GetIMatrix() == glm::imat2x2(0, -1, 1, 0));
        CHECK(direction.GetFMatrix() == glm::mat2x2(0.f, -1.f, 1.f, 0.f));
        CHECK(direction.Apply(glm::ivec2(1, 2)) == glm::ivec2(2, -1));
        CHECK(direction.ApplyInverse(glm::ivec2(2, -1)) == glm::ivec2(1, 2));
    }

    {
        xc::Rotation90 direction;
        direction.RotateClockwise(2);
        CHECK(direction.GetRotationIndex() == 2);
        CHECK(direction.GetIMatrix() == glm::imat2x2(-1, 0, 0, -1));
        CHECK(direction.GetFMatrix() == glm::mat2x2(-1.f, 0.f, 0.f, -1.f));
        CHECK(direction.Apply(glm::ivec2(1, 2)) == glm::ivec2(-1, -2));
        CHECK(direction.ApplyInverse(glm::ivec2(-1, -2)) == glm::ivec2(1, 2));
    }

    {
        xc::Rotation90 direction;
        direction.RotateClockwise(3);
        CHECK(direction.GetRotationIndex() == 3);
        CHECK(direction.GetIMatrix() == glm::imat2x2(0, 1, -1, 0));
        CHECK(direction.GetFMatrix() == glm::mat2x2(0.f, 1.f, -1.f, 0.f));
        CHECK(direction.Apply(glm::ivec2(1, 2)) == glm::ivec2(-2, 1));
        CHECK(direction.ApplyInverse(glm::ivec2(-2, 1)) == glm::ivec2(1, 2));
    }

    {
        xc::Rotation90 direction(2);
        CHECK(direction.GetRotationIndex() == 2);
        CHECK(direction != xc::Rotation90(1));
        CHECK(direction == xc::Rotation90(2));
    }

    {
        xc::Rotation90 direction(-1);
        CHECK(direction.GetRotationIndex() == 3);
    }

    {
        xc::Rotation90 direction(5);
        CHECK(direction.GetRotationIndex() == 1);
    }

    {
        xc::Rotation90 direction;
        direction.RotateClockwise(1);
        direction.RotateClockwise(1);
        CHECK(direction.GetRotationIndex() == 2);
    }

    {
        xc::Rotation90 direction;
        direction.RotateClockwise(3);
        direction.RotateClockwise(1);
        CHECK(direction.GetRotationIndex() == 0);
    }

    {
        xc::Rotation90 direction;
        direction.RotateClockwise(9);
        CHECK(direction.GetRotationIndex() == 1);
    }

    {
        xc::Rotation90 direction;
        direction.RotateClockwise(-1);
        CHECK(direction.GetRotationIndex() == 3);
    }

    {
        xc::Rotation90 direction;
        direction.RotateClockwise(-5);
        CHECK(direction.GetRotationIndex() == 3);
    }

    {
        xc::Rotation90 direction1(1);
        xc::Rotation90 direction2(2);
        CHECK((direction1 + direction2).GetRotationIndex() == 3);
    }

    {
        xc::Rotation90 direction1(2);
        xc::Rotation90 direction2(3);
        CHECK((direction1 + direction2).GetRotationIndex() == 1);
    }

    {
        xc::Rotation90 direction1(1);
        xc::Rotation90 direction2(2);
        direction2 += direction1;
        CHECK(direction2.GetRotationIndex() == 3);
    }

    {
        xc::Rotation90 direction1(2);
        xc::Rotation90 direction2(3);
        direction2 += direction1;
        CHECK(direction2.GetRotationIndex() == 1);
    }
}