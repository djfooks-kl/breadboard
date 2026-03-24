#pragma once

#include "ShaderProgramMap.h"

namespace xg
{
	struct RendererMap;
}

namespace xg
{
	enum class ERenderingMode
	{
		Normal,
		Preview,
		DropPreview
	};

	void RegisterCogRenderers(xg::RendererMap& map, xg::ShaderProgramMap& shaderProgramMap, xg::ERenderingMode mode);
}