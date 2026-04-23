#include "RenderableAdder.h"

#include "Core/Rotation90.h"
#include "Rendering/RenderableResourceId.h"
#include "Rendering/RendererMap.h"

xg::RenderableAdder::RenderableAdder(const char* name, xg::CogRendererMap& rendererMap)
	: m_Name(name)
	, m_RendererMap(rendererMap)
{
}

void xg::RenderableAdder::Add(
	const xg::RenderableResourceId renderableResourceId,
	const xc::ITransform& transform,
	const int flags,
	const glm::ivec2& infoUV) const
{
	auto* renderer = m_RendererMap.Get(renderableResourceId);
	if (!renderer)
	{
		printf("Could not find renderer for resource with id '%s' renderer map name '%s'\n",
			renderableResourceId.GetName(),
			m_Name);
		return;
	}
	renderer->AddRenderable(transform, flags, infoUV);
}
