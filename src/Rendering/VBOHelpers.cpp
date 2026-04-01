#include "VBOHelpers.h"

#include <vector>
#include <glm/ext/vector_int2.hpp>

#include "Core/GLFWLib.h"
#include "Rendering/VertexBufferObject.h"

namespace
{
	template<typename TData>
	std::vector<TData>& GetData(xg::VertexBufferObject& vbo, const GLuint attributePosition)
	{
		return std::get<std::vector<TData>>(vbo.ModifyVertexAttributeData(attributePosition));
	}
}

void xg::VBOAddQuadUV(xg::VertexBufferObject& vbo, const GLuint attributePosition)
{
	std::vector<int>& data = GetData<int>(vbo, attributePosition);

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

void xg::VBOAddIPosition2D(xg::VertexBufferObject& vbo, GLuint attributePosition, const glm::ivec2& position)
{
	std::vector<int>& data = GetData<int>(vbo, attributePosition);

	data.reserve(data.size() + 8);
	data.push_back(position.x);
	data.push_back(position.y);
	data.push_back(position.x);
	data.push_back(position.y);
	data.push_back(position.x);
	data.push_back(position.y);
	data.push_back(position.x);
	data.push_back(position.y);
}
