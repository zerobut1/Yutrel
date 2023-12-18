#include "yutrel_pch.hpp"

#include "time.hpp"

#include "platform/Windows/windows_time.hpp"

namespace Yutrel
{
    Ref<Time> Time::Create()
    {
        return CreateRef<WindowsTime>();
    }

    // void Time::Update(Commands& cmd, Querier querier, Resources resources, Events& events)
    // {
    //     auto time = resources.Get<Ref<Time>>();
    //     time->Tick();
    // }

} // namespace Yutrel