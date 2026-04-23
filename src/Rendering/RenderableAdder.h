#pragma once

#include "IRenderableAdder.h"
#include "RendererMap.h"

namespace xg
{
	class RenderableAdder : public xg::IRenderableAdder
	{
	public:
		RenderableAdder(const char* name, xg::CogRendererMap& rendererMap);

		void Add(
			const xg::RenderableResourceId renderableResourceId,
			const xc::ITransform& transform,
			const int flags,
			const glm::ivec2& infoUV) const override;

	private:
		const char* m_Name;
		xg::CogRendererMap& m_RendererMap;
	};
}