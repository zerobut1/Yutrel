#pragma once

#include "core/macro.hpp"
#include "inputcode/key_codes.hpp"
#include "inputcode/mouse_codes.hpp"

#include <gecs/gecs.hpp>
#include <glm/glm.hpp>

#include <stdint.h>
#include <unordered_set>

namespace Yutrel
{
    class WindowResource;

    class Input
    {
    public:
        struct KeyEvent final
        {
            KeyCode key;
            int scancode;
            int action;
            int mods;
        };

        struct MouseButtonEvent final
        {
            MouseCode button;
            int action;
            int mods;
        };

        struct CursorPosEvent final
        {
            glm::vec2 positon;
        };

        struct ResizeEvent final
        {
            uint32_t width;
            uint32_t height;
        };

    public:
        Input()          = default;
        virtual ~Input() = default;

        static void Init(gecs::resource<WindowResource> window,
                         gecs::resource<Input> input,
                         gecs::event_dispatcher<KeyEvent> keyboard_dispatcher,
                         gecs::event_dispatcher<MouseButtonEvent> mouse_btn_dispatcher,
                         gecs::event_dispatcher<CursorPosEvent> mouse_motion_dispatcher,
                         gecs::event_dispatcher<ResizeEvent> resize_dispatcher);

        bool IsKeyPressed(KeyCode key) const;
        bool IsMousePressed(MouseCode mouse) const;
        glm::vec2 GetCursorPos() const;

    private:
        static void KeyboardEventHandle(const KeyEvent& event, gecs::resource<gecs::mut<Input>> input);
        static void MouseButtonEventHandle(const MouseButtonEvent& event, gecs::resource<gecs::mut<Input>> input);
        static void MouseMotionEventHandle(const CursorPosEvent& event, gecs::resource<gecs::mut<Input>> input);
        static void ResizeEventHandle(const ResizeEvent& event, gecs::resource<class RendererResource> render);

        void OnKey(int key, int scancode, int action, int mods);
        void OnMouseButton(int button, int action, int mods);
        void OnCursorPos(double xpos, double ypos);
        void OnResize(int width, int height);

    private:
        std::unordered_set<KeyCode> key_pressed;
        std::unordered_set<MouseCode> mouse_pressed;
        glm::vec2 cursor_position;
    };

} // namespace Yutrel
