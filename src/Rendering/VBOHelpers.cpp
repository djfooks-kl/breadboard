#include "VBOHelpers.h"

#include <cassert>
#include <vector>
#include <glm/ext/vector_int2.hpp>

#include "Core/GLFWLib.h"
#include "Rendering/VertexBufferObject.h"

namespace
{
	template<typename TData>
	std::vector<TData>& GetData(xg::VertexBufferObject& vbo, const GLuint attributePosition, const int stride)
	{
		assert(vbo.GetVertexAttribute(attributePosition).m_Stride == stride);
		return std::get<std::vector<TData>>(vbo.ModifyVertexAttributeData(attributePosition));
	}
}

void xg::VBOAddQuadUV(xg::VertexBufferObject& vbo, const GLuint attributePosition)
{
	std::vector<int>& data = GetData<int>(vbo, attributePosition, 2);

	const int s0 = 0;
	const int t0 = 0;
	const int s1 = 1;
	const int t1 = 1;

	data.reserve(data.size() + 8);
	data.push_back(s0);
	data.push_back(t0);
	data.push_back(s1);
	data.push_back(t0);
	data.push_back(s0);
	data.push_back(t1);
	data.push_back(s1);
	data.push_back(t1);
}

void xg::VBOAddTextureUV(xg::VertexBufferObject& vbo, GLuint attributePosition, const xc::Rotation90 rotation)
{
	std::vector<int>& data = GetData<int>(vbo, attributePosition, 2);

	const xc::Rotation90UVs& uvs = rotation.GetUVs();

	data.reserve(data.size() + 8);
	data.push_back(uvs[0]);
	data.push_back(uvs[1]);
	data.push_back(uvs[2]);
	data.push_back(uvs[3]);
	data.push_back(uvs[4]);
	data.push_back(uvs[5]);
	data.push_back(uvs[6]);
	data.push_back(uvs[7]);
}

void xg::VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const int value)
{
	std::vector<int>& data = GetData<int>(vbo, attributePosition, 1);

	data.reserve(data.size() + 4);
	data.push_back(value);
	data.push_back(value);
	data.push_back(value);
	data.push_back(value);
}

void xg::VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::ivec2& value)
{
	std::vector<int>& data = GetData<int>(vbo, attributePosition, 2);

	data.reserve(data.size() + 8);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.x);
	data.push_back(value.y);
}

void xg::VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::ivec3& value)
{
	std::vector<int>& data = GetData<int>(vbo, attributePosition, 3);

	data.reserve(data.size() + 12);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
}

void xg::VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const float value)
{
	std::vector<float>& data = GetData<float>(vbo, attributePosition, 1);

	data.reserve(data.size() + 4);
	data.push_back(value);
	data.push_back(value);
	data.push_back(value);
	data.push_back(value);
}

void xg::VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::vec2& value)
{
	std::vector<float>& data = GetData<float>(vbo, attributePosition, 2);

	data.reserve(data.size() + 8);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.x);
	data.push_back(value.y);
}

void xg::VBOAdd(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::vec3& value)
{
	std::vector<float>& data = GetData<float>(vbo, attributePosition, 3);

	data.reserve(data.size() + 12);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
}
