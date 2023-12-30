#pragma once

#include <glm/glm.hpp>

namespace Engine
{
enum class RendererEnum
{
    INT,
    UINT,
    BYTE,
    UBYTE,
    FLOAT,
    UFLOAT,
    ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER,
    TRIANGLES,
    LINES,
    DEPTH_ATTACHMENT,
    COLOR_ATTACHMENT0,
    COLOR_ATTACHMENT1,
    DEPTH_TEST,
    FACE_CULL,
    STATIC_DRAW,
    DYNAMIC_DRAW,
    STREAM_DRAW,
	BLEND,
	CULL_FACE,
};

class RenderCommand
{
  public:
    static void Clear();
    static void SetClearColor(const glm::vec3 &color);

    static void Enable(const RendererEnum enumType);
    static void Disable(const RendererEnum enumType);

    static void DrawMultiElements(const RendererEnum mode, const int count, const RendererEnum type,
                                  const void *const *indices, unsigned int drawCount);
    static void DrawElements(const RendererEnum mode, const int count, const RendererEnum type, const void *indices);
    static void DrawArrays(int first, int count);

    static void DrawLines(int first, int count);
    static void DrawLineLoop(int first, int count);
    static void DrawLineStrip(int first, int count);
};
} // namespace Engine
