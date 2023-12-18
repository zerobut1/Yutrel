#include "yutrel_pch.hpp"

#include "input.hpp"

#include "core/application/application.hpp"
#include "function/window/window.hpp"

#include <GLFW/glfw3.h>

#include <functional>

namespace Yutrel
{
    void Input::Init(gecs::resource<WindowResource> window,
                     gecs::resource<Input> input,
                     gecs::event_dispatcher<KeyEvent> keyboard_dispatcher,
                     gecs::event_dispatcher<MouseButtonEvent> mouse_btn_dispatcher,
                     gecs::event_dispatcher<CursorPosEvent> mouse_motion_dispatcher)
    {
        LOG_INFO("Initialize Input Resource");

        keyboard_dispatcher.sink().add<Input::KeyboardEventHandle>();
        mouse_btn_dispatcher.sink().add<Input::MouseButtonEventHandle>();
        mouse_motion_dispatcher.sink().add<Input::MouseMotionEventHandle>();

        window->GetWindow()->RegisterOnKeyFunc(std::bind(&Input::OnKey, *input, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        window->GetWindow()->RegisterOnMouseButtonFunc(std::bind(&Input::OnMouseButton, *input, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        window->GetWindow()->RegisterOnCursorPosFunc(std::bind(&Input::OnCursorPos, *input, std::placeholders::_1, std::placeholders::_2));
    }

    bool Input::IsKeyPressed(KeyCode key) const
    {
        return key_pressed.count(key);
    }

    bool Input::IsMousePressed(MouseCode mouse) const
    {
        return mouse_pressed.count(mouse);
    }

    glm::vec2 Input::GetCursorPos() const
    {
        return cursor_position;
    }

    void Input::KeyboardEventHandle(const KeyEvent& event, gecs::resource<gecs::mut<Input>> input)
    {
        if (event.action == GLFW_PRESS)
        {
            input->key_pressed.insert(event.key);
        }
        else if (event.action == GLFW_RELEASE)
        {
            input->key_pressed.erase(event.key);
        }
    }

    void Input::MouseButtonEventHandle(const MouseButtonEvent& event, gecs::resource<gecs::mut<Input>> input)
    {
        if (event.action == GLFW_PRESS)
        {
            input->mouse_pressed.insert(event.button);
        }
        else if (event.action == GLFW_RELEASE)
        {
            input->mouse_pressed.erase(event.button);
        }
    }

    void Input::MouseMotionEventHandle(const CursorPosEvent& event, gecs::resource<gecs::mut<Input>> input)
    {
        input->cursor_position = event.positon;
    }

    void Input::OnKey(int key, int scancode, int action, int mods)
    {
        KeyEvent event{};
        event.key      = key;
        event.scancode = scancode;
        event.action   = action;
        event.mods     = mods;

        Application::Create().GetWorld().cur_registry()->event_dispatcher<KeyEvent>().enqueue(event);
    }

    void Input::OnMouseButton(int button, int action, int mods)
    {
        MouseButtonEvent event{};
        event.button = button;
        event.action = action;
        event.mods   = mods;

        Application::Create().GetWorld().cur_registry()->event_dispatcher<MouseButtonEvent>().enqueue(event);
    }

    void Input::OnCursorPos(double xpos, double ypos)
    {
        CursorPosEvent event{};
        event.positon = glm::vec2(xpos, ypos);

        Application::Create().GetWorld().cur_registry()->event_dispatcher<CursorPosEvent>().enqueue(event);
    }

} // namespace Yutrel