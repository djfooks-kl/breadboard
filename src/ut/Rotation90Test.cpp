#include <catch2/catch_test_macros.hpp>

#include "Core/Rotation90.h"

TEST_CASE("Simple Rotation90 tests", "[xc::Rotation90]")
{
    {
        xc::Rotation90 direction;
        CHECK(direction.GetRotationIndex() == 0);
        CHECK(direction.GetIRotation() == glm::imat2x2(1, 0, 0, 1));
        CHECK(direction.GetFRotation() == glm::mat2x2(1.f, 0.f, 0.f, 1.f));
    }

    {
        xc::Rotation90 direction;
        direction.RotateRight(1);
        CHECK(direction.GetRotationIndex() == 1);
        CHECK(direction.GetIRotation() == glm::imat2x2(0, -1, 1, 0));
        CHECK(direction.GetFRotation() == glm::mat2x2(0.f, -1.f, 1.f, 0.f));
    }

    {
        xc::Rotation90 direction;
        direction.RotateRight(2);
        CHECK(direction.GetRotationIndex() == 2);
        CHECK(direction.GetIRotation() == glm::imat2x2(-1, 0, 0, -1));
        CHECK(direction.GetFRotation() == glm::mat2x2(-1.f, 0.f, 0.f, -1.f));
    }

    {
        xc::Rotation90 direction;
        direction.RotateRight(3);
        CHECK(direction.GetRotationIndex() == 3);
        CHECK(direction.GetIRotation() == glm::imat2x2(0, 1, -1, 0));
        CHECK(direction.GetFRotation() == glm::mat2x2(0.f, 1.f, -1.f, 0.f));
    }

    {
        xc::Rotation90 direction;
        direction.RotateRight(1);
        direction.RotateRight(1);
        CHECK(direction.GetRotationIndex() == 2);
    }

    {
        xc::Rotation90 direction;
        direction.RotateRight(3);
        direction.RotateRight(1);
        CHECK(direction.GetRotationIndex() == 0);
    }

    {
        xc::Rotation90 direction;
        direction.RotateRight(9);
        CHECK(direction.GetRotationIndex() == 1);
    }

    {
        xc::Rotation90 direction;
        direction.RotateRight(-1);
        CHECK(direction.GetRotationIndex() == 3);
    }

    {
        xc::Rotation90 direction;
        direction.RotateRight(-5);
        CHECK(direction.GetRotationIndex() == 3);
    }
}