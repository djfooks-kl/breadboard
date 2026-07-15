#include "WireHelpers.h"

#include "WireFlags.h"
#include "GridAttachmentsMap.h"

namespace
{
	const xg::TWireDirectionFlags s_SameDirectionFlags0   = xg::TWireDirectionFlags{} | xg::EWireDirection::N | xg::EWireDirection::S;
	const xg::TWireDirectionFlags s_SameDirectionFlags45  = xg::TWireDirectionFlags{} | xg::EWireDirection::NE | xg::EWireDirection::SW;
	const xg::TWireDirectionFlags s_SameDirectionFlags90  = xg::TWireDirectionFlags{} | xg::EWireDirection::E | xg::EWireDirection::W;
	const xg::TWireDirectionFlags s_SameDirectionFlags135 = xg::TWireDirectionFlags{} | xg::EWireDirection::SE | xg::EWireDirection::NW;
}

xg::EWireDirection xg::WireDirectionToFlag(const glm::ivec2& v)
{
	if (v.y > 0)
	{
		return
			v.x > 0 ? xg::EWireDirection::NE :
			v.x < 0 ? xg::EWireDirection::NW :
			xg::EWireDirection::N;
	}
	else if (v.y < 0)
	{
		return
			v.x > 0 ? xg::EWireDirection::SE :
			v.x < 0 ? xg::EWireDirection::SW :
			xg::EWireDirection::S;
	}
	else
	{
		return
			v.x > 0 ? xg::EWireDirection::E :
			v.x < 0 ? xg::EWireDirection::W :
			xg::EWireDirection::None;
	}
}

bool xg::HasWireDot(const xg::GridAttachments& attachments)
{
	return attachments.m_HasWireCheckpoint ||
		(attachments.m_WireDirectionFlags.HasAny() && attachments.m_HasNode);
}