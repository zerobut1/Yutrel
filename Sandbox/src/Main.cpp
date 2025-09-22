#include "Compute.h"

#include "Log.h"

using namespace Yutrel;

int main()
{
    try
    {
        ApplicationCreateInfo application_ci{};
        application_ci.name   = "Sandbox";
        application_ci.width  = 1920;
        application_ci.height = 1080;

        Application app(application_ci);
        app.addComponent(std::make_unique<Compute>());
        app.run();
    }
    catch (const vk::SystemError& err)
    {
        LOG_ERROR("vk::SystemError - code: {} ", err.code().message());
        LOG_ERROR("vk::SystemError - what: {}", err.what());
    }
    catch (const std::exception& err)
    {
        LOG_ERROR("std::exception: {}", err.what());
    }

    return 0;
}