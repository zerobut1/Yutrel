#include "Triangle.h"

using namespace Yutrel;

int main()
{
    ApplicationCreateInfo application_ci{};
    application_ci.name = "Sandbox";

    auto app = std::make_unique<Application>(application_ci);

    app->addComponent(std::make_shared<Triangle>());

    app->run();

    app.reset();

    return 0;
}