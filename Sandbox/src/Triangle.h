#include "Application.h"
#include "RenderTarget.h"

class Triangle : public Yutrel::ComponentBase
{
public:
    void onAttach(Yutrel::Application* app) override;

    void onDetach() override;

    void onRender(vk::CommandBuffer cmd_buffer) override;

    void onResize(uint32_t width, uint32_t height) override;

private:
    Yutrel::Application* m_app;
    std::shared_ptr<Yutrel::Renderer> m_renderer;

    std::unique_ptr<Yutrel::RenderTarget> m_main_rt;

    uint32_t m_viewport_width{0};
    uint32_t m_viewport_height{0};
};