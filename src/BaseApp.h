#pragma once

#include <memory>

enum class EMouseCursor : uint8_t;

class BreadApp;

struct GLFWwindow;

class BaseApp
{
public:
    BaseApp();
    virtual ~BaseApp();

    virtual bool Run();

    virtual void SetCursor(EMouseCursor cursor) = 0;

    void Update();

    void ProcessKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
    void ProcessCursorInput(GLFWwindow* window, double xpos, double ypos);

protected:
    GLFWwindow* m_Window = nullptr;
private:
    virtual bool Init() = 0;
    virtual bool RunInternal(GLFWwindow* window) = 0;

    double m_LastFrame = 0.0;

    std::unique_ptr<BreadApp> m_GameApp;
};