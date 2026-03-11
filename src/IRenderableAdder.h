#pragma once

#include "Rendering/RenderableResourceId.h"

#include <glm/ext/vector_int2.hpp>

namespace xc
{
	class Rotation90;
}

namespace xg
{
	struct IRenderableAdder
	{
		virtual void Add(
			const xg::RenderableResourceId renderableResourceId,
			const glm::ivec2& position,
			const xc::Rotation90 rotation,
			const glm::ivec2& infoUV = glm::ivec2(0, 0)) const = 0;
	};
}