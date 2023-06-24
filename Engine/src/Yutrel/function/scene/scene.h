#pragma once

#include <entt/entt.hpp>

namespace Yutrel
{

    class Entity;

    class Scene
    {

    public:
        friend class Entity;

        Scene();
        ~Scene();

        Entity CreateEntity(const std::string &name = std::string());

        void OnUpdate(double delta_time);

    private:
        entt::registry m_Registry;
    };

} // namespace Yutrel