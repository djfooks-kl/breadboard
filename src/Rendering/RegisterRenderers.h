#pragma once

#include "Rendering/RendererMap.h"
#include "ShaderProgramMap.h"

namespace xg
{
	enum class ERenderingMode
	{
		Normal,
		Preview,
		PreviewInvalid,
		DropPreview
	};

	void RegisterCogRenderers(xg::CogRendererMap& map, xg::ShaderProgramMap& shaderProgramMap, xg::ERenderingMode mode);

	void RegisterWireRenderers(xg::WireRendererMap& map, xg::ShaderProgramMap& shaderProgramMap, xg::ERenderingMode mode);
}