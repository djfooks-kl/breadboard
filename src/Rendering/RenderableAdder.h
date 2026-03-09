#pragma once

#include "IRenderableAdder.h"
#include "BreadRenderer.h"

namespace xg
{
	class RenderableAdder : public xg::IRenderableAdder
	{
	public:
		RenderableAdder(xg::BreadRenderer& breadRenderer);

		void Add(
			const xg::RenderableResourceId renderableResourceId,
			const glm::ivec2& position,
			const xc::Rotation90 rotation) const override;

	private:
		xg::BreadRenderer& m_BreadRenderer;
	};

	class PreviewRenderableAdder : public xg::IRenderableAdder
	{
	public:
		PreviewRenderableAdder(xg::BreadRenderer& breadRenderer);

		void Add(
			const xg::RenderableResourceId renderableResourceId,
			const glm::ivec2& position,
			const xc::Rotation90 rotation) const override;

	private:
		xg::BreadRenderer& m_BreadRenderer;
	};
}