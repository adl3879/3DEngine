#include "RenderCommand.h"

#include <glad/glad.h>

namespace Engine
{
static GLenum GetType(const RendererEnum &bufferType)
{
    switch (bufferType)
    {
        case RendererEnum::ARRAY_BUFFER: return GL_ARRAY_BUFFER;
        case RendererEnum::ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
        case RendererEnum::FLOAT: return GL_FLOAT;
        case RendererEnum::UFLOAT: return GL_BYTE;
        case RendererEnum::BYTE: return GL_BYTE;
        case RendererEnum::UBYTE: return GL_UNSIGNED_BYTE;
        case RendererEnum::INT: return GL_INT;
        case RendererEnum::UINT: return GL_UNSIGNED_INT;
        case RendererEnum::TRIANGLES: return GL_TRIANGLES;
        case RendererEnum::LINES: return GL_LINES;
        case RendererEnum::DEPTH_TEST: return GL_DEPTH_TEST;
        case RendererEnum::FACE_CULL: return GL_CULL_FACE;
        case RendererEnum::STATIC_DRAW: return GL_STATIC_DRAW;
        case RendererEnum::DYNAMIC_DRAW: return GL_DYNAMIC_DRAW;
        case RendererEnum::STREAM_DRAW: return GL_STREAM_DRAW;
		case RendererEnum::BLEND: return GL_BLEND;
    }

    return 0;
}

void RenderCommand::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void RenderCommand::SetClearColor(const glm::vec3 &color) { glClearColor(color.r, color.g, color.b, 1.0f); }

void RenderCommand::Enable(const RendererEnum enumType) { glEnable(GetType(enumType)); }

void RenderCommand::Disable(const RendererEnum enumType) { glDisable(GetType(enumType)); }

void RenderCommand::DrawMultiElements(const RendererEnum mode, const int count, const RendererEnum type,
                                      const void *const *indices, unsigned int drawCount)
{
    glMultiDrawElements(GetType(mode), &count, GetType(type), indices, drawCount);
}

void RenderCommand::DrawElements(const RendererEnum mode, const int count, const RendererEnum type, const void *indices)
{
    glDrawElements(GetType(mode), count, GetType(type), indices);
}

void RenderCommand::DrawArrays(int from, int count) { glDrawArrays(GL_TRIANGLES, from, count); }

void RenderCommand::DrawLines(int from, int count) { glDrawArrays(GL_LINES, from, count); }
} // namespace Engine
