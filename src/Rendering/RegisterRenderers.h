#pragma once

#include "Rendering/RendererMap.h"
#include "ShaderProgramMap.h"

namespace xg
{
	struct RenderSettings;

	enum class ERenderingMode
	{
		Normal,
		Preview,
		PreviewInvalid,
		DropPreview
	};

	void RegisterCogRenderers(
		const xg::RenderSettings& settings,
		xg::CogRendererMap& map,
		xg::ShaderProgramMap& shaderProgramMap,
		xg::ERenderingMode mode);

	void RegisterWireRenderers(
		const xg::RenderSettings& settings,
		xg::WireRendererMap& map,
		xg::ShaderProgramMap& shaderProgramMap,
		xg::ERenderingMode mode);
}