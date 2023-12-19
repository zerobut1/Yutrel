/**
 * 渲染器
 * 与API有关
 * 默认资源之一
 * 提供一些默认加载的渲染资源、渲染选项设置以及渲染指令
 * todo 改为submit模式
 */
#pragma once

#include "function/window/window.hpp"
// #include "resource/asset/framebuffer.hpp"
// #include "resource/asset/shader.hpp"
// #include "resource/component/camera/camera_controller.hpp"
// #include "resource/component/component.hpp"
// #include "resource/component/model/model.hpp"
// #include "resource/component/ui/window_ui.hpp"

// #include "glm/glm.hpp"

struct GLFWwindow;

namespace Yutrel
{
    // struct RenderData
    // {
    //     Ref<Model> cube_model;
    //     Ref<Model> plane_model;
    //     Ref<Model> sphere_model;

    //     Ref<Shader> framebuffer_shader;
    //     Ref<Shader> skybox_shader;

    //     Ref<ComputeShader> irradiance_shader;
    //     Ref<ComputeShader> prefilter_shader;

    //     Ref<Texture> brdf_LUT;
    // };

    /*
     * 渲染器
     */
    // class Renderer
    // {
    // public:
    //     static void InitRenderData(Yutrel::Commands& cmd, Yutrel::Resources resources);

    // public:
    //     Ref<RenderData> GetRenderData() const { return m_render_data; }

    //     virtual void Clear() = 0;

    //     virtual void SeamlessCubemap(bool enable) = 0;
    //     virtual void DepthTest(bool enable)       = 0;
    //     virtual void FaceCulling(bool enable)     = 0;
    //     virtual void Blend(bool enable)           = 0;

    //     virtual void SetTranspatent(int mode) = 0;
    //     virtual void SetDepthFunc(int mode)   = 0;
    //     virtual void SetCullFace(int mode)    = 0;

    //     virtual void SetViewport(uint32_t width, uint32_t height) = 0;

    //     virtual void RenderScene(Ref<Shader> shader, bool is_material = false)                                                   = 0;
    //     virtual void RenderScene(Ref<Shader> shader, Ref<CameraController> camera, bool is_material = false, int blend_mode = 0) = 0;
    //     virtual void RenderShader(Ref<Shader> shader)                                                                            = 0;
    //     virtual void RenderSkybox(SkyBox skybox)                                                                                 = 0;

    //     virtual void DrawIndexed(const Ref<VertexArray> vertexArray) = 0;

    //     // ui
    //     // todo 重写
    //     virtual void InitializeUIRender(WindowUI* window_ui) = 0;
    //     virtual void ClearUIRender(WindowUI* window_ui)      = 0;
    //     virtual void RenderUI(Ref<WindowUI> ui)              = 0;

    // protected:
    //     static Ref<RenderData> m_render_data;
    // };

    struct RendererInitInfo
    {
        GLFWwindow* raw_window;
        uint32_t width;
        uint32_t height;
    };

    class Renderer
    {
    public:
        virtual void Init(RendererInitInfo info) = 0;

        virtual void Clear() = 0;
    };

    class RenderResource
    {
    public:
        RenderResource();
        ~RenderResource();

        static void Init(gecs::resource<RenderResource> render,
                         gecs::resource<WindowResource> window);

    private:
        Ref<Renderer> m_renderer;
    };
} // namespace Yutrel