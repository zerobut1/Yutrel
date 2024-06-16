#include <Yutrel.h>

class TestComponent : public Yutrel::ComponentBase
{
public:
    void onAttach(Yutrel::Application* app) override;

    void onDetach() override;

    void onRender(vk::CommandBuffer cmd_buffer) override;

private:
    Yutrel::Application* m_app;
};