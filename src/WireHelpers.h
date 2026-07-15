#pragma once

#include <glm/ext/vector_int2.hpp>
#include <vector>

#include "GridHelpers.h"
#include "WireFlags.h"

namespace xg
{
	struct GridAttachments;

	/// \brief Convert a vector to a wire direction flag
	xg::EWireDirection WireDirectionToFlag(const glm::ivec2& v);

	bool HasWireDot(const xg::GridAttachments& attachments);
}