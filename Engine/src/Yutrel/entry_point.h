#pragma once

#include "Yutrel/core/application/application.h"
#include "Yutrel/core/engine.h"

extern Yutrel::Application *Yutrel::CreateApplication(YutrelEngine *engine);

int main(int argc, char **argv)
{
    auto engine = new Yutrel::YutrelEngine();
    engine->startEngine();

    auto editor = Yutrel::CreateApplication(engine);
    editor->initialize();

    editor->run();

    editor->clear();

    engine->shutdownEngine();

    return 0;
}