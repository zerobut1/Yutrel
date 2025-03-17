#include "Application.h"
#include "RenderTarget.h"

class Compute : public Yutrel::ComponentBase
{
public:
    void onAttach(Yutrel::Application* app) override;

    void onDetach() override;

    void onRender(vk::CommandBuffer cmd_buffer) override;

    void onResize(uint32_t width, uint32_t height) override;

private:
    void initDescriptors();

    void initPipeline();

    void draw(vk::CommandBuffer cmd_buffer);

private:
    Yutrel::Application* m_app;
    std::shared_ptr<Yutrel::Renderer> m_renderer;

    std::unique_ptr<Yutrel::RenderTarget> m_main_rt;

    uint32_t m_viewport_width{0};
    uint32_t m_viewport_height{0};

    vk::DescriptorSet m_descriptor_set;
    vk::DescriptorSetLayout m_descriptor_set_layout;

    vk::PipelineLayout m_pipeline_layout;
    vk::Pipeline m_pipeline;

    struct PushConstants
    {
        uint32_t viewport_width;
        uint32_t viewport_height;
    } m_push_constants;
};