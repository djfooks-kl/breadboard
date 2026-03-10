#pragma once

#include "ShaderProgramMap.h"

namespace xg
{
	struct RendererMap;
}

namespace xg
{
	void RegisterCogRenderers(xg::RendererMap& map, xg::ShaderProgramMap& shaderProgramMap, bool isDropPreview);
}