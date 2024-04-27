#include "IMath.h"

#include <glad/glad.h>
#include "InputManager.h"

namespace Engine
{
namespace Math
{
bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &outTranslation, glm::vec3 &outRotation, glm::vec3 &outScale)
{
    // from glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), epsilon<T>())) return false;

    // First, isolate perspective.  This is the messiest.
    if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
    {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    outTranslation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3], Pdum3;

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j) Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    outScale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    outScale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    outScale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    outRotation.y = asin(-Row[0][2]);
    if (cos(outRotation.y) != 0)
    {
        outRotation.x = atan2(Row[1][2], Row[2][2]);
        outRotation.z = atan2(Row[0][1], Row[0][0]);
    }
    else
    {
        outRotation.x = atan2(-Row[2][0], Row[1][1]);
        outRotation.z = 0;
    }

    return true;
}

glm::vec3 ScreenToWorld(const glm::vec2 &mouse, const glm::vec2 &screenSize, const glm::mat4 &projection, const glm::mat4 &view)
{
	// Normalized device space
	glm::vec2 ndc;
	ndc.x = (2.0f * mouse.x) / screenSize.x - 1.0f;
	ndc.y = 1.0f - (2.0f * mouse.y) / screenSize.y;

	// Homogeneous clip space
	glm::vec4 clipSpace(ndc.x, ndc.y, -1.0f, 1.0f);

	// Eye space
	glm::vec4 eyeSpace = glm::inverse(projection) * clipSpace;
	eyeSpace = glm::vec4(eyeSpace.x, eyeSpace.y, -1.0f, 0.0f);

	// World space
	glm::vec4 worldSpace = glm::inverse(view) * eyeSpace;
	worldSpace = glm::normalize(worldSpace);

	return glm::vec3(worldSpace);
}

glm::vec3 QuatToDirection(const glm::quat &quat) { return glm::normalize(quat * glm::vec3(0, 0, -1)); }

} // namespace Math
} // namespace Engine
