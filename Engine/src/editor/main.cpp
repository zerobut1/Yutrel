#include "yutrel_pch.h"

#include "editor/editor.h"
#include "runtime/engine.h"

int main(int argc, char **argv)
{
    Yutrel::YutrelEngine *engine = new Yutrel::YutrelEngine();
    engine->startEngine();

    Yutrel::YutrelEditor *editor = new Yutrel::YutrelEditor();
    editor->initialize(engine);

    editor->run();

    editor->clear();

    engine->shutdownEngine();

    return 0;
}