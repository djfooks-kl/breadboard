#pragma once

namespace xg
{
	struct UISettings
	{
#define ADD_SETTING(TYPE, NAME, DEFAULT_VALUE) TYPE m_##NAME;
#include "UISettingsList.h"
#undef ADD_SETTING
	};
}