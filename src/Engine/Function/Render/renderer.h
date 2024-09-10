// #pragma once

// #include <vk_mem_alloc.hpp>
// #include <vulkan/vulkan.hpp>

// struct GLFWwindow;

// namespace Yutrel
// {
//     struct RendererCreateInfo
//     {
//     };

//     class Renderer final
//     {
//     public:
//         Renderer() = delete;
//         explicit Renderer(const RendererCreateInfo& info);
//         ~Renderer();

//         Renderer(const Renderer&)            = delete;
//         Renderer& operator=(const Renderer&) = delete;

//         vk::CommandBuffer prepareBeforeRender();
//         void submitRendering();
//         void framePresent();

//         void transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::ImageLayout cur_layout, vk::ImageLayout new_layout);

//     private:
//         void init(const RendererCreateInfo& info);
//         void shutdown();

//         std::shared_ptr<class Frame> getCurrentFrame() const { return m_frames[m_frame_count % s_max_frame]; }

//     private:
//         static constexpr uint8_t s_max_frame{2};

//         std::shared_ptr<class Context> m_context;
//         std::shared_ptr<class ResourceManager> m_resource_manager;

//         uint32_t m_frame_count{0};
//         std::array<std::shared_ptr<Frame>, s_max_frame> m_frames;
//         vk::CommandPool m_cmd_pool{nullptr};
//     };

// } // namespace Yutrel