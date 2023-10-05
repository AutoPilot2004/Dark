#pragma once
#include <GL/glew.h>
#include <vector>
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Dark
{
	struct VertexAttribute
	{
		VertexAttribute(GLint size, int originalTypeSize, GLenum type, GLboolean normalized)
			: size(size), originalTypeSize(originalTypeSize), type(type), normalized(normalized) {}

		GLint size;
		int originalTypeSize;
		GLenum type;
		GLboolean normalized;
	};

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void gen();
		void del();

		template<typename T> struct AttribTraitsT;
		template<> struct AttribTraitsT<int> { static constexpr GLenum glType = GL_INT; };
		template<> struct AttribTraitsT<unsigned int> { static constexpr GLenum glType = GL_UNSIGNED_INT; };
		template<> struct AttribTraitsT<float> { static constexpr GLenum glType = GL_FLOAT; };
		template<> struct AttribTraitsT<char> { static constexpr GLenum glType = GL_BYTE; };
		template<> struct AttribTraitsT<unsigned char> { static constexpr GLenum glType = GL_UNSIGNED_BYTE; };
		
		template<bool B> struct AttribTraitsB;
		template<> struct AttribTraitsB<true> { static constexpr GLboolean glNormalized = GL_TRUE; };
		template<> struct AttribTraitsB<false> { static constexpr GLboolean glNormalized = GL_FALSE; };

		template<typename TYPE, bool NORMALIZED>
		inline void addVertexAttribute(GLint size) {
			m_attributes.emplace_back(size, sizeof(TYPE), AttribTraitsT<TYPE>::glType, AttribTraitsB<NORMALIZED>::glNormalized);
		}

		void build(VertexBuffer& vertexBuffer, IndexBuffer& indexBuffer);

		void bind() const;
		void unbind() const;

	private:
		GLuint m_vaoID;

		std::vector<VertexAttribute> m_attributes;

		bool m_idGenerated;
	};
}