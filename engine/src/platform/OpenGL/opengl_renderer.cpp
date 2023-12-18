
// #include "yutrel_pch.hpp"

// #include "opengl_renderer.hpp"

// #include "core/application/application.hpp"
// #include "function/window/window.hpp"
// #include "resource/component/transform.hpp"

// #include "glm/ext/matrix_clip_space.hpp"
// #include <GLFW/glfw3.h>
// #include <glad/glad.h>
// #include <glm/ext/matrix_float4x4.hpp>
// #include <glm/ext/matrix_transform.hpp>
// #include <glm/ext/quaternion_trigonometric.hpp>
// #include <glm/trigonometric.hpp>
// #include <imgui.h>
// #include <imgui_impl_glfw.h>
// #include <imgui_impl_opengl3.h>
// #include <imgui_internal.h>

// #include <stdint.h>

// namespace Yutrel
// {
//     OpenGLRenderer::OpenGLRenderer(Ref<Window> window)
//     {
//         LOG_INFO("Initlize OpenGL Renderer");
//         // 窗口
//         glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetWindow()));

//         // 初始化GLAD
//         gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

//         // 关闭垂直同步
//         glfwSwapInterval(0);
//     }

//     OpenGLRenderer::~OpenGLRenderer()
//     {
//         Clear();
//     }

//     // 用以清除默认framebuffer
//     void OpenGLRenderer::Clear()
//     {
//         glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
//         glClearDepth(1.0f);
//         glClearStencil(0);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//     }

//     void OpenGLRenderer::SeamlessCubemap(bool enable)
//     {
//         static bool is_enabled = false;

//         if (enable && !is_enabled)
//         {
//             glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//             is_enabled = true;
//         }
//         else if (!enable && is_enabled)
//         {
//             glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//             is_enabled = false;
//         }
//     }

//     void OpenGLRenderer::DepthTest(bool enable)
//     {
//         static bool is_enabled = false;

//         if (enable && !is_enabled)
//         {
//             glEnable(GL_DEPTH_TEST);
//             glDepthMask(GL_TRUE);
//             glDepthFunc(GL_LEQUAL);
//             glDepthRange(0.0f, 1.0f);
//             is_enabled = true;
//         }
//         else if (!enable && is_enabled)
//         {
//             glDisable(GL_DEPTH_TEST);
//             is_enabled = false;
//         }
//     }

//     void OpenGLRenderer::FaceCulling(bool enable)
//     {
//         static bool is_enabled = false;

//         if (enable && !is_enabled)
//         {
//             glEnable(GL_CULL_FACE);
//             glFrontFace(GL_CCW);
//             glCullFace(GL_BACK);
//             is_enabled = true;
//         }
//         else if (!enable && is_enabled)
//         {
//             glDisable(GL_CULL_FACE);
//             is_enabled = false;
//         }
//     }

//     void OpenGLRenderer::Blend(bool enable)
//     {
//         static bool is_enabled = false;

//         if (enable && !is_enabled)
//         {
//             glEnable(GL_BLEND);
//             is_enabled = true;
//         }
//         else if (!enable && is_enabled)
//         {
//             glDisable(GL_BLEND);
//             is_enabled = false;
//         }
//     }

//     void OpenGLRenderer::SetTranspatent(int mode)
//     {
//         if (mode == 0)
//         {
//             glEnable(GL_DEPTH_TEST);
//             glDepthFunc(GL_LEQUAL);
//             glDepthMask(GL_TRUE);
//             glDisable(GL_BLEND);
//         }
//         else if (mode == 1)
//         {
//             glDepthMask(GL_FALSE);
//             glEnable(GL_BLEND);
//             glBlendFunci(0, GL_ONE, GL_ONE);
//             glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
//             glBlendEquation(GL_FUNC_ADD);
//         }
//         else if (mode == 2)
//         {
//             glDepthFunc(GL_ALWAYS);
//             glEnable(GL_BLEND);
//             glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//         }
//     }

//     void OpenGLRenderer::SetDepthFunc(int mode)
//     {
//         switch (mode)
//         {
//         case 0:
//             glDepthFunc(GL_ALWAYS);
//             break;
//         case 1:
//             glDepthFunc(GL_NEVER);
//             break;
//         case 2:
//             glDepthFunc(GL_LESS);
//             break;
//         case 3:
//             glDepthFunc(GL_EQUAL);
//             break;
//         case 4:
//             glDepthFunc(GL_LEQUAL);
//             break;
//         case 5:
//             glDepthFunc(GL_GREATER);
//             break;
//         case 6:
//             glDepthFunc(GL_NOTEQUAL);
//             break;
//         case 7:
//             glDepthFunc(GL_GEQUAL);
//             break;
//         default:
//             YUTREL_ASSERT(false, "Not Supported DepthFunc!");
//         }
//     }

//     void OpenGLRenderer::SetCullFace(int mode)
//     {
//         if (mode == 0)
//         {
//             glCullFace(GL_BACK);
//         }
//         else if (mode == 1)
//         {
//             glCullFace(GL_FRONT);
//         }
//         else
//         {
//             YUTREL_ASSERT(false, "Not Supported CullFace!");
//         }
//     }

//     void OpenGLRenderer::SetViewport(uint32_t width, uint32_t height)
//     {
//         glViewport(0, 0, width, height);
//     }

//     // 没有buffer的绘制
//     void OpenGLRenderer::RenderShader(Ref<Shader> shader)
//     {
//         auto VAO = VertexArray::Create();
//         VAO->Bind();
//         shader->Bind();

//         glDrawArrays(GL_TRIANGLES, 0, 3);
//     }

//     void OpenGLRenderer::RenderScene(Ref<Shader> shader, bool is_material)
//     {
//         auto querier  = Querier(Application::GetInstance().GetWorld());
//         auto resource = Resources(Application::GetInstance().GetWorld());

//         //-----------------------------
//         // 渲染场景
//         //-----------------------------
//         shader->Bind();

//         auto scene_entities = querier.Query<Scene>();
//         for (auto entity : scene_entities)
//         {
//             auto model        = querier.Get<Yutrel::Ref<Model>>(entity);
//             auto model_matrix = querier.Get<Transform>(entity).transform;
//             shader->SetMat4("u_model", model_matrix);
//             model->Draw(shader, is_material);
//         }
//     }

//     void OpenGLRenderer::RenderScene(Ref<Shader> shader, Ref<CameraController> camera, bool is_material, int blend_mode)
//     {
//         auto querier  = Querier(Application::GetInstance().GetWorld());
//         auto resource = Resources(Application::GetInstance().GetWorld());

//         //-----------------------------
//         // 渲染场景
//         //-----------------------------
//         shader->Bind();

//         auto scene_entities = querier.Query<Scene>();
//         for (auto entity : scene_entities)
//         {
//             auto model        = querier.Get<Yutrel::Ref<Model>>(entity);
//             auto tranform     = querier.Get<Transform>(entity);
//             auto model_matrix = tranform.transform;
//             shader->SetMat4("u_model", model_matrix);
//             model->Draw(shader, camera, is_material, blend_mode, tranform);
//         }
//     }

//     void OpenGLRenderer::RenderSkybox(SkyBox skybox)
//     {
//         auto querier = Querier(Application::GetInstance().GetWorld());
//         auto data    = GetRenderData();
//         auto texture = skybox.environment_map;
//         auto shader  = data->skybox_shader;
//         auto model   = data->cube_model;

//         glDepthFunc(GL_LEQUAL);
//         glFrontFace(GL_CW);
//         shader->Bind();
//         texture->Bind(0);
//         model->Draw(shader);
//         glDepthFunc(GL_LESS);
//         glFrontFace(GL_CCW);
//     }

//     void OpenGLRenderer::DrawIndexed(const Ref<VertexArray> vertexArray)
//     {
//         vertexArray->Bind();
//         glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
//         vertexArray->Unbind();
//     }

//     void OpenGLRenderer::RenderUI(Ref<WindowUI> ui)
//     {
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();
//         ui->RenderUI();
//         ImGui::Render();
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//     }

//     void OpenGLRenderer::InitializeUIRender(WindowUI* window_ui)
//     {
//         Resources resources = Resources(Application::GetInstance().GetWorld());
//         ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(resources.Get<Ref<Window>>()->GetWindow()), true);
//         ImGui_ImplOpenGL3_Init("#version 460");
//     }

//     void OpenGLRenderer::ClearUIRender(WindowUI* window_ui)
//     {
//         ImGui_ImplOpenGL3_Shutdown();
//         ImGui_ImplGlfw_Shutdown();
//         ImGui::DestroyContext();
//     }

// } // namespace Yutrel