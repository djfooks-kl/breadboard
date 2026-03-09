#include "RenderableAdder.h"

#include "Core/Rotation90.h"
#include "Rendering/RenderableResourceId.h"
#include "Rendering/RendererMap.h"

xg::RenderableAdder::RenderableAdder(const char* name, xg::RendererMap& rendererMap)
	: m_Name(name)
	, m_RendererMap(rendererMap)
{
}

void xg::RenderableAdder::Add(
	const xg::RenderableResourceId renderableResourceId,
	const glm::ivec2& position,
	const xc::Rotation90 rotation) const
{
	auto* renderer = m_RendererMap.Get(renderableResourceId);
	if (!renderer)
	{
		printf("Could not find renderer for resource with id '%s' renderer map name '%s'",
			renderableResourceId.GetName(),
			m_Name);
		return;
	}
	renderer->AddRenderable(position, rotation);
}
