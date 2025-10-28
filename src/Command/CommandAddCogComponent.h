#pragma once

#include <glm/ext/matrix_int3x3.hpp>

#include "Cogs/CogResourceId.h"

namespace xg::command
{
	struct AddCogComponent
	{
		xg::CogResourceId m_CogId;
	};
}
