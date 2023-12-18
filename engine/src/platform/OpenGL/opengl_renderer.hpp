// #pragma once

// #include "function/render/renderer.hpp"
// #include "platform/OpenGL/opengl_framebuffer.hpp"
// #include "platform/OpenGL/opengl_shader.hpp"
// #include "platform/OpenGL/opengl_texture.hpp"
// #include "platform/OpenGL/opengl_vertex_array.hpp"
// #include "resource/component/camera/camera_controller.hpp"

// #include "GLFW/glfw3.h"

// #include <memory>
// #include <stdint.h>
// #include <vector>

// namespace Yutrel
// {
//     /*
//      * OpenGL 渲染器的实现
//      */
//     class OpenGLRenderer : public Renderer
//     {
//     public:
//         OpenGLRenderer(Ref<Window> window);
//         virtual ~OpenGLRenderer();

//         virtual void Clear() override;

//         virtual void SeamlessCubemap(bool enable) override;
//         virtual void DepthTest(bool enable) override;
//         virtual void FaceCulling(bool enable) override;
//         virtual void Blend(bool enable) override;

//         virtual void SetTranspatent(int mode) override;
//         virtual void SetDepthFunc(int mode) override;
//         virtual void SetCullFace(int mode) override;

//         virtual void SetViewport(uint32_t width, uint32_t height) override;

//         virtual void RenderScene(Ref<Shader> shader, bool is_material = false) override;
//         virtual void RenderScene(Ref<Shader> shader, Ref<CameraController> camera, bool is_material = false, int blend_mode = 0) override;
//         virtual void RenderShader(Ref<Shader> shader) override;
//         virtual void RenderSkybox(SkyBox skybox) override;

//         virtual void DrawIndexed(const Ref<VertexArray> vertexArray) override;

//         // ui
//         virtual void InitializeUIRender(WindowUI* window_ui) override;
//         virtual void ClearUIRender(WindowUI* window_ui) override;
//         virtual void RenderUI(Ref<WindowUI> ui) override;
//     };

// } // namespace Yutrel