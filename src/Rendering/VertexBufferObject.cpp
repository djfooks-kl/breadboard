#include "Rendering/VertexBufferObject.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"

namespace
{
    template<class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template<typename TDATA>
    GLuint TryCreateAndBindBuffer(const GLenum target, const std::vector<TDATA>& data, GLuint buffer)
    {
        if (!buffer)
        {
            glGenBuffers(1, &buffer);
        }
        glBindBuffer(target, buffer);
        glBufferData(target, data.size() * sizeof(TDATA), &data.front(), GL_STATIC_DRAW);
        return buffer;
    }
}

xg::VertexBufferObject::~VertexBufferObject()
{
    glDeleteVertexArrays(1, &m_VBO);
    for (VertexAttribute& vertexAttribute : m_VertexAttributes)
    {
        glDeleteBuffers(1, &vertexAttribute.m_GLBuffer);
    }
}

void xg::VertexBufferObject::AddQuad()
{
    // TODO use one shared array since this is the same data every time

    //   2---3
    //   | \ |
    //   0---1
    // anti-clockwise winding
    m_Indices.reserve(m_Indices.size() + 6);
    m_Indices.push_back(m_VertexCount + 0);
    m_Indices.push_back(m_VertexCount + 1);
    m_Indices.push_back(m_VertexCount + 2);
    m_Indices.push_back(m_VertexCount + 1);
    m_Indices.push_back(m_VertexCount + 2);
    m_Indices.push_back(m_VertexCount + 3);

    m_VertexCount += 4;

    m_BuffersDirty = true;
}

bool xg::VertexBufferObject::IsEmpty() const
{
    return m_VertexCount == 0;
}

xg::VertexAttribute& xg::VertexBufferObject::AddVertexAttribute(const GLuint attributePosition, const int stride)
{
    if (m_VertexAttributes.size() <= attributePosition)
        m_VertexAttributes.resize(attributePosition + 1);

    xg::VertexAttribute& vertexAttribute = m_VertexAttributes[attributePosition];
    vertexAttribute.m_AttributePosition = attributePosition;
    vertexAttribute.m_Stride = stride;
    return vertexAttribute;
}

void xg::VertexBufferObject::AddIVertexAttribute(const GLuint attributePosition, const int stride)
{
    xg::VertexAttribute& vertexAttribute = AddVertexAttribute(attributePosition, stride);
    vertexAttribute.m_Data.emplace<std::vector<int>>();
}

void xg::VertexBufferObject::AddFVertexAttribute(const GLuint attributePosition, const int stride)
{
    xg::VertexAttribute& vertexAttribute = AddVertexAttribute(attributePosition, stride);
    vertexAttribute.m_Data.emplace<std::vector<float>>();
}

xg::TVertexAttributeBuffer& xg::VertexBufferObject::ModifyVertexAttributeData(GLuint attributePosition)
{
    m_BuffersDirty = true;
    return m_VertexAttributes[attributePosition].m_Data;
}

void xg::VertexBufferObject::RemoveAllData()
{
    for (xg::VertexAttribute& vertexAttribute : m_VertexAttributes)
    {
        std::visit(
            [](auto& data)
            {
                data.clear();
            }
        , vertexAttribute.m_Data);
    }
    m_Indices.clear();
    m_VertexCount = 0;
    m_BuffersDirty = true;
}

void xg::VertexBufferObject::UpdateAndBindBuffers()
{
    if (!m_BuffersDirty)
        return;

    if (m_VBO == 0)
    {
        glGenVertexArrays(1, &m_VBO);
    }

    glBindVertexArray(m_VBO);

    for (xg::VertexAttribute& vertexAttribute : m_VertexAttributes)
    {
        std::visit(
            [&](auto& data)
            {
                vertexAttribute.m_GLBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, data, vertexAttribute.m_GLBuffer);
            }, vertexAttribute.m_Data);
    }
    m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

    for (VertexAttribute& vertexAttribute : m_VertexAttributes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexAttribute.m_GLBuffer);

        std::visit(overloaded{
            [&](std::vector<int>& /*data*/)
            {
                glVertexAttribPointer(
                    vertexAttribute.m_AttributePosition,
                    vertexAttribute.m_Stride,
                    GL_INT,
                    GL_FALSE,
                    vertexAttribute.m_Stride * sizeof(int),
                    (void*)0);
            },
            [&](std::vector<float>& /*data*/)
            {
                glVertexAttribPointer(
                    vertexAttribute.m_AttributePosition,
                    vertexAttribute.m_Stride,
                    GL_FLOAT,
                    GL_FALSE,
                    vertexAttribute.m_Stride * sizeof(float),
                    (void*)0);
            }
        }, vertexAttribute.m_Data);

        glEnableVertexAttribArray(vertexAttribute.m_AttributePosition);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);

    m_BuffersDirty = false;
}

void xg::VertexBufferObject::Bind()
{
    glBindVertexArray(m_VBO);
}

void xg::VertexBufferObject::Draw()
{
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}