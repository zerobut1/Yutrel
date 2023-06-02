#include <Yutrel/Yutrel.h>
#include <Yutrel/main.h>

#include "editor.h"

namespace Yutrel
{
    Application *CreateApplication(YutrelEngine *engine)
    {
        return new YutrelEditor(engine);
    }
} // namespace Yutrel