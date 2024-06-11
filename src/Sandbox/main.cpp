#include <Yutrel.h>

using namespace Yutrel;

class TestComponent : public ComponentBase
{
public:
    TestComponent()  = default;
    ~TestComponent() = default;

    void OnAttach(Application* app) override
    {
        LOG_INFO("TestComponent attach");
    }

    void OnDetach() override
    {
    }
};

int main()
{
    ApplicationCreateInfo application_ci;

    auto app = std::make_unique<Application>(application_ci);

    app->AddComponent(std::make_shared<TestComponent>());

    app->Run();

    app.reset();

    return 0;
}