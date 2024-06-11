#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Yutrel
{
    struct ApplicationCreateInfo
    {
        std::string name{"Yutrel App"};
        int32_t width{1920};
        int32_t height{1080};
    };

    class ComponentBase;

    class Application
    {
    public:
        explicit Application(const ApplicationCreateInfo& info);
        virtual ~Application();

        Application(Application& application) = delete;
        void operator=(const Application&)    = delete;

        void Run();

        void AddComponent(const std::shared_ptr<ComponentBase>& component);

    private:
        std::vector<std::shared_ptr<ComponentBase>> m_components;
    };

    class ComponentBase
    {
    public:
        virtual ~ComponentBase() = default;

        virtual void OnAttach(Application* app) = 0;
        virtual void OnDetach()                 = 0;
    };

} // namespace Yutrel