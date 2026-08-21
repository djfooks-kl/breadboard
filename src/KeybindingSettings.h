#pragma once

#include <unordered_map>

#include "MappedInputEnum.h"

namespace xg
{
	struct KeybindingSettings
	{
		std::unordered_map<int, xg::EMappedInput> m_Bindings;
	};
}