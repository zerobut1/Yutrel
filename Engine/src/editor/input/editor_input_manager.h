// #pragma once

// #include "glm/fwd.hpp"
// #include <glm/glm.hpp>

// namespace Yutrel
// {

//     enum class EditorCommand : unsigned int
//     {
//         camera_left      = 1 << 0,  // A
//         camera_back      = 1 << 1,  // S
//         camera_foward    = 1 << 2,  // W
//         camera_right     = 1 << 3,  // D
//         camera_up        = 1 << 4,  // Q
//         camera_down      = 1 << 5,  // E
//         translation_mode = 1 << 6,  // T
//         rotation_mode    = 1 << 7,  // R
//         scale_mode       = 1 << 8,  // C
//         exit             = 1 << 9,  // Esc
//         delete_object    = 1 << 10, // Delete
//     };

//     class EditorInputManager
//     {
//     public:
//         void initialize();
//         void tick(float delta_time);

//         void setEngineWindowPos(glm::vec2 new_window_pos) { m_engine_window_pos = new_window_pos; };
//         void setEngineWindowSize(glm::vec2 new_window_size) { m_engine_window_size = new_window_size; };

//     private:
//         void registerInput();
//         void processEditorCommand();

//         void onKey(int key, int scancode, int action, int mods);

//     private:
//         glm::vec2 m_engine_window_pos{0.0f, 0.0f};
//         glm::vec2 m_engine_window_size{1920.0f, 1080.0f};

//         unsigned int m_editor_command{0};
//     };
// } // namespace Yutrel