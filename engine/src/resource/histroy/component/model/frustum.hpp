/**
 * 视见体、包围盒等
 * 用于视锥体裁剪
 */
#pragma once

#include "resource/component/camera/camera_controller.hpp"
#include "resource/component/transform.hpp"

#include "glm/glm.hpp"
#include <array>
#include <memory>

namespace Yutrel
{

    struct Plane
    {
        glm::vec3 normal = {0.f, 1.f, 0.f}; // unit vector
        float distance   = 0.f;             // Distance with origin

        Plane() = default;

        Plane(const glm::vec3& p1, const glm::vec3& norm)
            : normal(glm::normalize(norm)),
              distance(glm::dot(normal, p1))
        {
        }

        float GetSignedDistanceToPlane(const glm::vec3& point) const
        {
            return glm::dot(normal, point) - distance;
        }
    };

    struct Frustum
    {
        Plane topFace;
        Plane bottomFace;

        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;

        Frustum() = default;
        Frustum(Ref<CameraController> camera_controller)
        {
            auto camera = camera_controller->GetCamera();

            const float halfVSide        = 100.0f * tanf(camera_controller->GetZoomLevel() * 0.5f);
            const float halfHSide        = halfVSide * camera_controller->GetAspectRatio();
            const glm::vec3 frontMultFar = 100.0f * camera->getDirection();

            nearFace   = {camera->getPosition() + 0.1f * camera->getDirection(), camera->getDirection()};
            farFace    = {camera->getPosition() + frontMultFar, -camera->getDirection()};
            rightFace  = {camera->getPosition(), glm::cross(frontMultFar - camera->GetRight() * halfHSide, camera->getUp())};
            leftFace   = {camera->getPosition(), glm::cross(camera->getUp(), frontMultFar + camera->GetRight() * halfHSide)};
            topFace    = {camera->getPosition(), glm::cross(camera->GetRight(), frontMultFar - camera->getUp() * halfVSide)};
            bottomFace = {camera->getPosition(), glm::cross(frontMultFar + camera->getUp() * halfVSide, camera->GetRight())};
        }
    };

    struct BoundingVolume
    {
        virtual bool IsOnFrustum(const Frustum& camFrustum, const Transform& transform) const = 0;

        virtual bool IsOnOrForwardPlane(const Plane& plane) const = 0;

        bool IsOnFrustum(const Frustum& camFrustum) const
        {
            return (IsOnOrForwardPlane(camFrustum.leftFace) &&
                    IsOnOrForwardPlane(camFrustum.rightFace) &&
                    IsOnOrForwardPlane(camFrustum.topFace) &&
                    IsOnOrForwardPlane(camFrustum.bottomFace) &&
                    IsOnOrForwardPlane(camFrustum.nearFace) &&
                    IsOnOrForwardPlane(camFrustum.farFace));
        };
    };

    struct AABB : public BoundingVolume
    {
        glm::vec3 center{0.f, 0.f, 0.f};
        glm::vec3 extents{0.f, 0.f, 0.f};

        AABB(const glm::vec3& min, const glm::vec3& max)
            : BoundingVolume{}, center{(max + min) * 0.5f}, extents{max.x - center.x, max.y - center.y, max.z - center.z}
        {
        }

        AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
            : BoundingVolume{}, center{inCenter}, extents{iI, iJ, iK}
        {
        }

        std::array<glm::vec3, 8> getVertice() const
        {
            std::array<glm::vec3, 8> vertice;
            vertice[0] = {center.x - extents.x, center.y - extents.y, center.z - extents.z};
            vertice[1] = {center.x + extents.x, center.y - extents.y, center.z - extents.z};
            vertice[2] = {center.x - extents.x, center.y + extents.y, center.z - extents.z};
            vertice[3] = {center.x + extents.x, center.y + extents.y, center.z - extents.z};
            vertice[4] = {center.x - extents.x, center.y - extents.y, center.z + extents.z};
            vertice[5] = {center.x + extents.x, center.y - extents.y, center.z + extents.z};
            vertice[6] = {center.x - extents.x, center.y + extents.y, center.z + extents.z};
            vertice[7] = {center.x + extents.x, center.y + extents.y, center.z + extents.z};
            return vertice;
        }

        // see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
        bool IsOnOrForwardPlane(const Plane& plane) const final
        {
            // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
            const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
                            extents.z * std::abs(plane.normal.z);

            return -r <= plane.GetSignedDistanceToPlane(center);
        }

        bool IsOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
        {
            // Get global scale thanks to our transform
            const glm::vec3 globalCenter{transform.transform * glm::vec4(center, 1.f)};

            // Scaled orientation
            const glm::vec3 right   = transform.right * extents.x;
            const glm::vec3 up      = transform.up * extents.y;
            const glm::vec3 forward = transform.forward * extents.z;

            const float newIi = std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, right)) +
                                std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, up)) +
                                std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, forward));

            const float newIj = std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, right)) +
                                std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, up)) +
                                std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, forward));

            const float newIk = std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, right)) +
                                std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, up)) +
                                std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, forward));

            const AABB globalAABB(globalCenter, newIi, newIj, newIk);

            return (globalAABB.IsOnOrForwardPlane(camFrustum.leftFace) &&
                    globalAABB.IsOnOrForwardPlane(camFrustum.rightFace) &&
                    globalAABB.IsOnOrForwardPlane(camFrustum.topFace) &&
                    globalAABB.IsOnOrForwardPlane(camFrustum.bottomFace) &&
                    globalAABB.IsOnOrForwardPlane(camFrustum.nearFace) &&
                    globalAABB.IsOnOrForwardPlane(camFrustum.farFace));
        }
    };
} // namespace Yutrel