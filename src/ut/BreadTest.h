#include "STDStreamFunctions.h"
#include <catch2/catch_test_macros.hpp>

#define CHECK_NEARLY_GLM_EQUAL(A, B, MARGIN) \
	{ \
		INFO(A << " ~= " << B << " (margin=" << MARGIN << ")"); \
		CHECK(glm::distance(A, B) < MARGIN); \
	}