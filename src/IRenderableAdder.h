#pragma once

#include "Rendering/RenderableResourceId.h"

#include <glm/ext/vector_int2.hpp>

namespace xc
{
	class ITransform;
}

namespace xg
{
	struct IRenderableAdder
	{
		virtual void Add(
			const xg::RenderableResourceId renderableResourceId,
			const xc::ITransform& transform,
			const int flags = 0,
			const glm::ivec2& infoUV = glm::ivec2(0, 0)) const = 0;
	};
}