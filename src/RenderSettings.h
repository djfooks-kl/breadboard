#pragma once

#include <glm/vec3.hpp>

#include "RBGColor.h"

namespace xg
{
	struct RenderSettings
	{
#define ADD_SETTING(TYPE, NAME, DEFAULT_VALUE) TYPE m_##NAME;
#include "RenderSettingsList.h"
#undef ADD_SETTING
	};
}