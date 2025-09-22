#include "Compute.h"

using namespace Yutrel;

int main()
{
    ApplicationCreateInfo application_ci{};
    application_ci.name   = "Sandbox";
    application_ci.width  = 1920;
    application_ci.height = 1080;

    Application app(application_ci);
    app.addComponent(std::make_unique<Compute>());
    app.run();

    return 0;
}