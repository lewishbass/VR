
#include "ObjectUtils.hpp"

glm::mat4 createPerspectiveProjection(float fovY, float aspect, float nearPlane, float farPlane)
{
    float f = 1.0f / tan(fovY / 2.0f);
    glm::mat4 proj = glm::mat4(0.0f);
    proj[0][0] = f / aspect;
    proj[1][1] = f;
    proj[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    proj[2][3] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    proj[3][2] = -1.0f;
    return proj;
}


