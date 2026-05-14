#include "WindowApp.h"

#include <iostream>
#include <imgui.h>

#include "Core/GLFWLib.h"
#include "MouseCursorEnum.h"

WindowApp::WindowApp()
    : BaseApp()
{
}

WindowApp::~WindowApp()
{
    glfwDestroyCursor(m_ArrowCursor);
    glfwDestroyCursor(m_CrossCursor);
}

bool WindowApp::Init()
{
    if (!gladLoadGLES2(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glfwSwapInterval(1);

    m_ArrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_CrossCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

    return true;
}

bool WindowApp::RunInternal(GLFWwindow* window)
{
    printf("Running WindowApp...\n");

    ImGui::GetStyle().FontScaleDpi = 1.5f;

    while (!glfwWindowShouldClose(window))
    {
        Update();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return true;
}

void WindowApp::SetCursor(EMouseCursor cursor)
{
    GLFWcursor* glfwCursor = m_ArrowCursor;
    switch (cursor)
    {
    case EMouseCursor::Arrow: glfwCursor = m_ArrowCursor; break;
    case EMouseCursor::Cross: glfwCursor = m_CrossCursor; break;
    }

    glfwSetCursor(m_Window, glfwCursor);
}