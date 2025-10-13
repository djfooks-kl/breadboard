#pragma once

#include <glm/ext/matrix_int3x3.hpp>

#include "Cog/CogResourceId.h"

namespace xg::command
{
	struct AddCogComponent
	{
		xg::CogResourceId m_Cog;
		imat3x3 m_Transform;
	};
}
