#pragma once

namespace Yutrel
{
    class YutrelEngine
    {
    public:
        YutrelEngine()          = default;
        virtual ~YutrelEngine() = default;

        void startEngine();
        void shutdownEngine();

        bool tickOneFrame();

    protected:
        void rendererTick();
    };
} // namespace Yutrel