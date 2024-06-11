#include "application.h"

namespace Yutrel
{
    Application::Application(const ApplicationCreateInfo& info)
    {
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
    }

    void Application::AddComponent(const std::shared_ptr<ComponentBase>& component)
    {
        m_components.emplace_back(component)->OnAttach(this);
    }

} // namespace Yutrel