#pragma once

#include <glm/ext/matrix_int3x3.hpp>

#include "Core/Rotation90.h"
#include "Cogs/CogResourceId.h"

namespace xg::command
{
	struct AddCogComponent
	{
		xg::CogResourceId m_CogId;
		xc::Rotation90 m_Rotation;
	};
}
