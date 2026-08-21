#pragma once

#include <string>

namespace xg
{
	enum class EMappedInput : uint8_t
	{
		Invalid,

		MoveUp,
		MoveLeft,
		MoveDown,
		MoveRight,
		ZoomIn,
		ZoomOut,
		Rotate,
		Delete,
		Multiselect,

		MAX
	};

	const char* ToString(xg::EMappedInput v);
	xg::EMappedInput MappedInputFromString(const std::string& s);
}