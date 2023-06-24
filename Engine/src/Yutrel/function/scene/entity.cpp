#include "yutrel_pch.h"

#include "entity.h"

namespace Yutrel
{

    Entity::Entity(entt::entity handle, Scene *scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }

} // namespace Yutrel