#pragma once

#include <vector>
#include <variant>

typedef int GLint;
typedef unsigned int GLuint;

namespace xg
{
    using TVertexAttributeBuffer = std::variant<
        std::vector<int>,
        std::vector<float>
    >;

    struct VertexAttribute
    {
        TVertexAttributeBuffer m_Data;
        int m_Stride = 2;
        GLuint m_AttributePosition = 0;
        GLuint m_GLBuffer = 0;
    };

    struct VertexBufferObject
    {
        ~VertexBufferObject();

        /// \brief Add an integer vertex attribute to this vertex buffer object
        /// \param attributePosition the position of the attribute in the shader
        /// \param stride the number of values per vertex
        void AddIVertexAttribute(GLuint attributePosition, int stride);

        /// \brief Add an float vertex attribute to this vertex buffer object
        /// \param attributePosition the position of the attribute in the shader
        /// \param stride the number of values per vertex
        void AddFVertexAttribute(GLuint attributePosition, int stride);

        /// \brief Set an attribute to use pre-existing data
        ///        This is a non owning reference. The dataRef cannot be moved.
        /// \param attributePosition the position of the attribute in the shader
        /// \param dataRef the data to reference
        void SetVertexAttributeDataRef(GLuint attributePosition, TVertexAttributeBuffer* dataRef);

        /// \brief Modify the attribute data for a given attribute
        ///        The buffer will be updated next time BindAndUpdateBuffers is called
        /// \param attributePosition the position of the attribute in the shader
        TVertexAttributeBuffer& ModifyVertexAttributeData(GLuint attributePosition);

        /// \brief Get the attribute data for a given attribute
        /// \param attributePosition the position of the attribute in the shader
        const VertexAttribute& GetVertexAttribute(GLuint attributePosition) const;

        /// \brief Add a quad to the index buffer
        void AddQuad();

        /// \brief Check if the VBO has any triangles to render
        bool IsEmpty() const;

        /// \brief Remove all vertex data
        void RemoveAllData();

        /// \brief Update the GL buffers and bind them to the VBO
        ///        Does nothing if the buffers have not been changed
        void UpdateAndBindBuffers();

        /// \brief Calls glBindVertexArray for this VBO
        void Bind();

        /// \brief Calls glDrawElements for this VBO
        void Draw();

    private:
        VertexAttribute& AddVertexAttribute(GLuint attributePosition, int stride);

        std::vector<VertexAttribute> m_VertexAttributes;

        std::vector<unsigned int> m_Indices;
        GLuint m_IndicesBuffer = 0;
        int m_VertexCount = 0;

        GLuint m_VBO = 0;
        bool m_BuffersDirty = false;
    };
}