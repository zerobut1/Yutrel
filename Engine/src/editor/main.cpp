#include <Yutrel/Yutrel.h>

#include <iostream>

void test1(Yutrel::Commands& cmd,
           Yutrel::Querier querier,
           Yutrel::Resources resources,
           Yutrel::Events& events)
{
    std::cout << "test1" << std::endl;
}

void test2(Yutrel::Commands& cmd,
           Yutrel::Querier querier,
           Yutrel::Resources resources,
           Yutrel::Events& events)
{
    std::cout << "test2" << std::endl;
}

int main()
{
    Yutrel::Application app;

    // app.GetWorld()
        // .AddSystem(test1)
        // .AddSystem(test2);

    app.Init();

    app.Run();

    app.Shutdown();
}