#pragma once

#include <glm/fwd.hpp>

#include "Core/Rotation90.h"

typedef unsigned int GLuint;

namespace xg
{
	struct VertexBufferObject;

	void VBOAddQuadUV(xg::VertexBufferObject& vbo, GLuint attributePosition);
	void VBOAddTextureUV(xg::VertexBufferObject& vbo, GLuint attributePosition, const xc::Rotation90 rotation);

	void VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const int value);
	void VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::ivec2& value);
	void VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::ivec3& value);

	void VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const float value);
	void VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::vec2& value);
	void VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::vec3& value);

}