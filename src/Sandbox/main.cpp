#include "test_component.h"

using namespace Yutrel;

int main()
{
    ApplicationCreateInfo application_ci{};
    application_ci.name                              = "Sandbox";
    application_ci.device_features.samplerAnisotropy = vk::True;

    auto app = std::make_unique<Application>(application_ci);

    app->addComponent(std::make_shared<TestComponent>());

    app->run();

    app.reset();

    return 0;
}