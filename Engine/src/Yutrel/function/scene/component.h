#pragma once

#include "Yutrel/function/render/model.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>

namespace Yutrel
{
    struct TagComponent
    {
        std::string Tag;

        TagComponent()                     = default;
        TagComponent(const TagComponent &) = default;
        TagComponent(const std::string &tag)
            : Tag(tag) {}
    };

    struct TransformComponent
    {
        glm::mat4 Transform{1.0f};

        TransformComponent()                           = default;
        TransformComponent(const TransformComponent &) = default;
        TransformComponent(const glm::mat4 &transform)
            : Transform(transform) {}

        operator glm::mat4 &() { return Transform; }
        operator const glm::mat4 &() const { return Transform; }
    };

    struct SpriteRendererComponent
    {
        glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};

        SpriteRendererComponent()                                = default;
        SpriteRendererComponent(const SpriteRendererComponent &) = default;
        SpriteRendererComponent(const glm::vec4 &color)
            : Color(color) {}
    };

    struct ModelRenderComponent
    {
        std::shared_ptr<Model> model;

        ModelRenderComponent()                             = default;
        ModelRenderComponent(const ModelRenderComponent &) = default;
        ModelRenderComponent(const std::shared_ptr<Model> model)
            : model(model) {}
    };

} // namespace Yutrel