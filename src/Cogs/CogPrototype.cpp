#include "CogPrototype.h"

namespace
{
    static const std::vector<glm::ivec2> s_EmptyWireNodes = {};
}

xg::CogPrototype::~CogPrototype() = default;

const std::vector<glm::ivec2>& xg::CogPrototype::GetWireNodes() const
{
	return s_EmptyWireNodes;
}

void xg::CogPrototype::AddStaticRenderables(
    const xc::ITransform& /*transform*/,
    const xg::IRenderableAdder& /*renderableAdder*/) const
{
}