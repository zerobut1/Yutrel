#include <Yutrel/Yutrel.h>

int main()
{
    auto app = Yutrel::CreateApplication();

    // app->AddSystem(XXXX)
    // ->AddSystem(XXXX);

    app->Init();

    app->Run();
}

/*想要的启动方式

#include <Yutrel.h>


*/