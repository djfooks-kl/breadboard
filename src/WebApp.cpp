#include "WebApp.h"

#include <iostream>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include "MouseCursorEnum.h"

namespace
{
    void UpdateInternal(void* userData)
    {
        WebApp& webApp = *static_cast<WebApp*>(userData);
        webApp.Update();
    }

    const char* GetCursorName(const EMouseCursor cursor)
    {
        switch (cursor)
        {
        case EMouseCursor::Arrow: return "default";
        case EMouseCursor::Cross: return "crosshair";
        }
        return "default";
    }
}

WebApp::WebApp()
    : BaseApp()
    , m_ActiveCursor(EMouseCursor::Arrow)
{
}

WebApp::~WebApp()
{
}

bool WebApp::Init()
{
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;
    attr.depth = 1;
    attr.stencil = 0;
    attr.antialias = 1;
    attr.majorVersion = 2;
    attr.minorVersion = 0;

    m_WebGLContextHandle = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(m_WebGLContextHandle);
    return true;
}

bool WebApp::RunInternal(GLFWwindow* /*window*/)
{
    printf("Running WebApp...\n");
    emscripten_set_main_loop_arg(UpdateInternal, static_cast<void*>(this), 0, 1);
    emscripten_webgl_destroy_context(m_WebGLContextHandle);
    return true;
}

void WebApp::SetCursor(EMouseCursor cursor)
{
    if (m_ActiveCursor == cursor)
        return;

    m_ActiveCursor = cursor;
    const char* cursorName = GetCursorName(cursor);
    printf("set cursor: %s\n", cursorName);
    EM_ASM({
        var cursorName = UTF8ToString($0);
        document.getElementById('canvas').style.cursor = cursorName;
    }, cursorName);
}