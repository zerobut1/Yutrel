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
        void shutdownEngine();

        bool tickOneFrame(float delta_time);

        float calculateDeltaTime();
        int getFPS() const { return m_fps; }

    protected:
        void rendererTick(float delta_time);

    private:
        void calculateFPS(float delta_time);

    private:
        float m_lastframetime{0};
        float m_average_duration{0.0f};
        int m_frame_count{0};
        int m_fps{0};
    };
} // namespace Yutrel