#include "RenderableAdder.h"

#include "Core/Rotation90.h"
#include "Rendering/RenderableDescriptor.h"
#include "BreadRenderer.h"

xg::RenderableAdder::RenderableAdder(xg::BreadRenderer& breadRenderer)
	: m_BreadRenderer(breadRenderer)
{
}

void xg::RenderableAdder::Add(
	const xg::RenderableResourceId renderableResourceId,
	const glm::ivec2& position,
	const xc::Rotation90 rotation) const
{
	m_BreadRenderer.AddRenderable(xg::RenderableDescriptor{ renderableResourceId, xg::ERenderableMode::InGame }, position, rotation);
}

xg::PreviewRenderableAdder::PreviewRenderableAdder(xg::BreadRenderer& breadRenderer)
	: m_BreadRenderer(breadRenderer)
{
}

void xg::PreviewRenderableAdder::Add(
	const xg::RenderableResourceId renderableResourceId,
	const glm::ivec2& position,
	const xc::Rotation90 rotation) const
{
	m_BreadRenderer.AddRenderable(xg::RenderableDescriptor{ renderableResourceId, xg::ERenderableMode::Preview }, position, rotation);
}
