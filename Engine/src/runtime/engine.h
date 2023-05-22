#pragma once

namespace Yutrel
{
    class YutrelEngine
    {
        static const float s_fps_alpha;

    public:
        YutrelEngine()          = default;
        virtual ~YutrelEngine() = default;

        void startEngine();
        bool tickOneFrame(float delta_time);
        void shutdownEngine();

        float calculateDeltaTime();
        int getFPS() const { return m_fps; }

    private:
        void rendererTick(float delta_time);

        void calculateFPS(float delta_time);

    private:
        float m_lastframetime    = 0.0f;
        float m_average_duration = 0.0f;
        int m_frame_count        = 0;
        int m_fps                = 0;
    };
} // namespace Yutrel