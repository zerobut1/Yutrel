// #include "editor_input_manager.h"

// #include "editor_global_context.h"

// namespace Yutrel
// {
//     void EditorInputManager::initialize()
//     {
//         registerInput();
//     }

//     void EditorInputManager::tick(float delta_time)
//     {
//         processEditorCommand();
//     }

//     void EditorInputManager::registerInput()
//     {
//         // g_editor_global_context.m_window_system->registerOnResetFunc(std::bind(&EditorInputManager::onReset, this));
//         // g_editor_global_context.m_window_system->registerOnCursorPosFunc(
//         //     std::bind(&EditorInputManager::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
//         // g_editor_global_context.m_window_system->registerOnCursorEnterFunc(
//         //     std::bind(&EditorInputManager::onCursorEnter, this, std::placeholders::_1));
//         // g_editor_global_context.m_window_system->registerOnScrollFunc(
//         //     std::bind(&EditorInputManager::onScroll, this, std::placeholders::_1, std::placeholders::_2));
//         // g_editor_global_context.m_window_system->registerOnMouseButtonFunc(
//         //     std::bind(&EditorInputManager::onMouseButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
//         // g_editor_global_context.m_window_system->registerOnWindowCloseFunc(
//         //     std::bind(&EditorInputManager::onWindowClosed, this));
//         // g_editor_global_context.m_window_system->registerOnKeyFunc(std::bind(&EditorInputManager::onKey,
//         //                                                                      this,
//         //                                                                      std::placeholders::_1,
//         //                                                                      std::placeholders::_2,
//         //                                                                      std::placeholders::_3,
//         //                                                                      std::placeholders::_4));
//     }

//     void EditorInputManager::processEditorCommand()
//     {
//         // float camera_speed            = m_camera_speed;
//         // std::shared_ptr editor_camera = g_editor_global_context.m_scene_manager->getEditorCamera();
//         // Quaternion camera_rotate      = editor_camera->rotation().inverse();
//         // Vector3 camera_relative_pos(0, 0, 0);

//         // if ((unsigned int)EditorCommand::camera_foward & m_editor_command)
//         // {
//         //     camera_relative_pos += camera_rotate *Vector3{0, camera_speed, 0};
//         // }
//         // if ((unsigned int)EditorCommand::camera_back & m_editor_command)
//         // {
//         //     camera_relative_pos += camera_rotate *Vector3{0, -camera_speed, 0};
//         // }
//         // if ((unsigned int)EditorCommand::camera_left & m_editor_command)
//         // {
//         //     camera_relative_pos += camera_rotate *Vector3{-camera_speed, 0, 0};
//         // }
//         // if ((unsigned int)EditorCommand::camera_right & m_editor_command)
//         // {
//         //     camera_relative_pos += camera_rotate *Vector3{camera_speed, 0, 0};
//         // }
//         // if ((unsigned int)EditorCommand::camera_up & m_editor_command)
//         // {
//         //     camera_relative_pos += Vector3{0, 0, camera_speed};
//         // }
//         // if ((unsigned int)EditorCommand::camera_down & m_editor_command)
//         // {
//         //     camera_relative_pos += Vector3{0, 0, -camera_speed};
//         // }
//         // if ((unsigned int)EditorCommand::delete_object & m_editor_command)
//         // {
//         //     g_editor_global_context.m_scene_manager->onDeleteSelectedGObject();
//         // }

//         // editor_camera->move(camera_relative_pos);
//     }

//     void EditorInputManager::onKey(int key, int scancode, int action, int mods)
//     {
//         if (action == GLFW_PRESS)
//         {
//             switch (key)
//             {
//             case GLFW_KEY_A:
//                 m_editor_command |= (unsigned int)EditorCommand::camera_left;
//                 break;
//             case GLFW_KEY_S:
//                 m_editor_command |= (unsigned int)EditorCommand::camera_back;
//                 break;
//             case GLFW_KEY_W:
//                 m_editor_command |= (unsigned int)EditorCommand::camera_foward;
//                 break;
//             case GLFW_KEY_D:
//                 m_editor_command |= (unsigned int)EditorCommand::camera_right;
//                 break;
//             case GLFW_KEY_Q:
//                 m_editor_command |= (unsigned int)EditorCommand::camera_up;
//                 break;
//             case GLFW_KEY_E:
//                 m_editor_command |= (unsigned int)EditorCommand::camera_down;
//                 break;
//             case GLFW_KEY_T:
//                 m_editor_command |= (unsigned int)EditorCommand::translation_mode;
//                 break;
//             case GLFW_KEY_R:
//                 m_editor_command |= (unsigned int)EditorCommand::rotation_mode;
//                 break;
//             case GLFW_KEY_C:
//                 m_editor_command |= (unsigned int)EditorCommand::scale_mode;
//                 break;
//             case GLFW_KEY_DELETE:
//                 m_editor_command |= (unsigned int)EditorCommand::delete_object;
//                 break;
//             default:
//                 break;
//             }
//         }
//         else if (action == GLFW_RELEASE)
//         {
//             switch (key)
//             {
//             case GLFW_KEY_ESCAPE:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::exit);
//                 break;
//             case GLFW_KEY_A:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_left);
//                 break;
//             case GLFW_KEY_S:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_back);
//                 break;
//             case GLFW_KEY_W:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_foward);
//                 break;
//             case GLFW_KEY_D:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_right);
//                 break;
//             case GLFW_KEY_Q:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_up);
//                 break;
//             case GLFW_KEY_E:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_down);
//                 break;
//             case GLFW_KEY_T:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::translation_mode);
//                 break;
//             case GLFW_KEY_R:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::rotation_mode);
//                 break;
//             case GLFW_KEY_C:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::scale_mode);
//                 break;
//             case GLFW_KEY_DELETE:
//                 m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::delete_object);
//                 break;
//             default:
//                 break;
//             }
//         }
//     }

// } // namespace Yutrel