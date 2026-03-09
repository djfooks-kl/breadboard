#pragma once

#include "Rendering/RenderableResourceId.h"

#include <glm/fwd.hpp>

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
			const xc::Rotation90 rotation) const = 0;
	};
}