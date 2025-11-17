#include <catch2/catch_test_macros.hpp>

#include "Core/ShaderProgram.h"
#include "Core/GLFWLib.h"

namespace
{
    void TestCompile(xc::ShaderProgramOptions options)
    {
        xc::ShaderProgram program(options);
        xc::Result<void> result = program.TryLoad();
        if (!result)
        {
            INFO(result.GetError());
            REQUIRE(result);
        }
    }
}

TEST_CASE("Test shaders compile", "[shaders]")
{
    REQUIRE(glfwInit());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(100, 100, "Compile shaders", NULL, NULL);
    REQUIRE(window);
    glfwMakeContextCurrent(window);
    REQUIRE(gladLoadGLES2(glfwGetProcAddress));

    TestCompile(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/DemoVertex.glsl",
        .m_FragmentPath = "shaders/Demofragment.glsl" });

    TestCompile(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/Boxvertex.glsl",
        .m_FragmentPath = "shaders/Boxfragment.glsl" });

    TestCompile(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/Boxvertex.glsl",
        .m_FragmentPath = "shaders/Textfragment.glsl" });

    TestCompile(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/GridVertex.glsl",
        .m_FragmentPath = "shaders/GridFragment.glsl" });

    TestCompile(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/CogBoxVertex.glsl",
        .m_FragmentPath = "shaders/CogBoxFragment.glsl" });

    glfwDestroyWindow(window);
    glfwTerminate();
}