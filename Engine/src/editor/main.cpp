#include "editor.h"

#include <Yutrel/Yutrel.h>
#include <Yutrel/entry_point.h>

namespace Yutrel
{
    Application *CreateApplication(YutrelEngine *engine)
    {
        return new YutrelEditor(engine);
    }
} // namespace Yutrel