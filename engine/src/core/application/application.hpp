#pragma once

#include "core/macro.hpp"

#include <gecs/gecs.hpp>

#include <stdint.h>
#include <string>

namespace Yutrel
{
    class Application final
    {
    public:
        static Application& Create();

    public:
        Application(Application& application) = delete;
        void operator=(const Application&)    = delete;

        Application& Init(const std::string& title = "Yutrel App", uint32_t width = 1920, uint32_t height = 1080);

        void Run();

        // 只在Input里使用！！！！！！！！！！！
        gecs::world& GetWorld() { return m_world; }

        //------gecs的封装-------------
        template <auto System>
        Application& AddStartupSystem()
        {
            m_world.cur_registry()->regist_startup_system<System>();
            return *this;
        }

        template <auto System>
        Application& AddSystem()
        {
            m_world.cur_registry()->regist_update_system<System>();
            return *this;
        }

        template <typename T, typename... Args>
        Application& AddResource(Args&&... args)
        {
            m_world.cur_registry()->commands().emplace_resource<T>(std::forward<Args>(args)...);
            return *this;
        }
        //-----------------------------

    private:
        Application()  = default;
        ~Application() = default;

    private:
        gecs::world m_world;
    };

} // namespace Yutrel