// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_MOUSE_GLFW_H
#define GDK_INPUT_MOUSE_GLFW_H

#include <memory>

#include <gdk/mouse.h>

struct GLFWwindow;

namespace gdk
{
    /// \brief mouse using glfw for implementation
    /// \attention glfwPollEvents() must be called periodically in order to update the mouse state
    class mouse_glfw : public mouse
    {
        std::shared_ptr<GLFWwindow> m_pWindow;

    public:
        virtual bool getButtonDown(const mouse::Button aKeyCode) override;

        virtual bool getButtonDown(const Button aKeyCode) override;
        virtual Vector2 getCursorPosition() override;

        mouse_glfw(decltype(m_pWindow) pWindow)
        : m_pWindow(pWindow)
        {}
    };
}

#endif