#include "yutrel_pch.h"

#include "Scene.h"

#include "component.h"
#include "entity.h"

#include <glm/glm.hpp>

namespace Yutrel
{

    static void DoMath(const glm::mat4 &transform)
    {
    }

    static void OnTransformConstruct(entt::registry &registry, entt::entity entity)
    {
    }

    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(const std::string &name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<TransformComponent>();
        auto &tag = entity.AddComponent<TagComponent>();
        tag.Tag   = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::OnUpdate(double delta_time)
    {
        auto group = m_Registry.group<TransformComponent>(entt::get<ModelRenderComponent>);
        for (auto entity : group)
        {
            auto [transform, model] = group.get<TransformComponent, ModelRenderComponent>(entity);

            // Renderer2D::DrawQuad(transform, sprite.Color);
        }
    }

} // namespace Yutrel