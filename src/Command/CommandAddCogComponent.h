#pragma once

#include <glm/ext/vector_int2.hpp>

#include "Core/Rotation90.h"
#include "Cogs/CogResourceId.h"

namespace xg::command
{
	struct AddCogComponent
	{
		xg::CogResourceId m_CogId;
		glm::ivec2 m_Position;
		xc::Rotation90 m_Rotation;
	};
}
