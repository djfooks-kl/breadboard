#pragma once

#include <glm/fwd.hpp>
typedef unsigned int GLuint;

namespace xg
{
	struct VertexBufferObject;

	void VBOAddQuadUV(xg::VertexBufferObject& vbo, GLuint attributePosition);
	void VBOAddIPosition2D(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::ivec2& position);
}