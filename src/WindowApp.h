#pragma once

#include "BaseApp.h"

struct GLFWcursor;

class WindowApp final : public BaseApp
{
public:
    WindowApp();
    ~WindowApp() override;

    void SetCursor(EMouseCursor cursor) override;

private:
    bool Init() override;
    bool RunInternal(GLFWwindow* window) override;

    GLFWcursor* m_ArrowCursor = nullptr;
    GLFWcursor* m_CrossCursor = nullptr;
};