#include <Yutrel.h>

class Triangle : public Yutrel::ComponentBase
{
public:
    void onAttach(Yutrel::Application* app) override;

    void onDetach() override;

    void onRender(vk::CommandBuffer cmd_buffer) override;

private:
    void createDrawImage();

private:
    Yutrel::Application* m_app;
    std::shared_ptr<Yutrel::Renderer> m_renderer;
};