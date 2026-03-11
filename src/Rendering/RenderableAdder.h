#pragma once

#include "IRenderableAdder.h"
#include "RendererMap.h"

namespace xg
{
	class RenderableAdder : public xg::IRenderableAdder
	{
	public:
		RenderableAdder(const char* name, xg::RendererMap& rendererMap);

		void Add(
			const xg::RenderableResourceId renderableResourceId,
			const glm::ivec2& position,
			const xc::Rotation90 rotation,
			const glm::ivec2& infoUV) const override;

	private:
		const char* m_Name;
		xg::RendererMap& m_RendererMap;
	};
}