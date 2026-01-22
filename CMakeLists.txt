cmake_minimum_required(VERSION 3.15)

project(BreadProject VERSION 1.0)

add_library(project_compiler_flags INTERFACE)
target_compile_features(project_compiler_flags INTERFACE cxx_std_20)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC,Emscripten>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

target_compile_options(project_compiler_flags INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W4>>"
)

set(core_files

    src/Core/Font.cpp
    src/Core/Font.h
    src/Core/GLFWLib.h
    src/Core/ResourceId.h
    src/Core/Result.h
    src/Core/Rotation90.cpp
    src/Core/Rotation90.h
    src/Core/ShaderHelpers.cpp
    src/Core/ShaderHelpers.h
    src/Core/ShaderProgram.cpp
    src/Core/ShaderProgram.h
    src/Core/STBImage.cpp
)

set(source_files

    src/BaseApp.cpp
    src/BaseApp.h
    src/BoxRenderer.cpp
    src/BoxRenderer.h
    src/BreadApp.cpp
    src/BreadApp.h
    src/BreadEntityWorld.cpp
    src/BreadEntityWorld.h
    src/BreadRenderer.cpp
    src/BreadRenderer.h
    src/CameraComponent.h
    src/CameraHelpers.cpp
    src/CameraHelpers.h
    src/CameraInputComponent.h
    src/CameraInputSystem.cpp
    src/CameraInputSystem.h
    src/CameraSystem.cpp
    src/CameraSystem.h
    src/CogBoxRenderer.cpp
    src/CogBoxRenderer.h
    src/CogComponent.h
    src/CogCreatedComponent.h
    src/CogSystem.cpp
    src/CogSystem.h
    src/GridRenderer.cpp
    src/GridRenderer.h
    src/InputComponent.h
    src/InputSystem.cpp
    src/InputSystem.h
    src/MouseTrailComponent.h
    src/MouseTrailSystem.cpp
    src/MouseTrailSystem.h
    src/OnStageAddedComponent.h
    src/OnStageComponent.h
    src/OnStageRemovedComponent.h
    src/OnStageSystem.cpp
    src/OnStageSystem.h
    src/TextRenderer.cpp
    src/TextRenderer.h
    src/UI.cpp
    src/UI.h
    src/UIAddCogComponent.h
    src/UIDragDropSystem.cpp
    src/UIDragDropSystem.h
    src/UIDraggingDropComponent.h
    src/UIDragPreviewComponent.h
    src/UIDragPreviewSystem.cpp
    src/UIDragPreviewSystem.h
    src/UIPreviewAddingCogComponent.h
    src/UIRedoComponent.h
    src/UIRotateComponent.h
    src/UIUndoComponent.h
    src/WindowSizeComponent.h
    src/WindowSizeSystem.cpp
    src/WindowSizeSystem.h
    src/WorldMouseComponent.h
)

set(cog_files

    src/Cogs/Battery.cpp
    src/Cogs/Battery.h
    src/Cogs/BatteryComponent.h
    src/Cogs/BatterySystem.cpp
    src/Cogs/BatterySystem.h
    src/Cogs/CogMap.cpp
    src/Cogs/CogMap.h
    src/Cogs/CogPrototype.cpp
    src/Cogs/CogPrototype.h
    src/Cogs/CogResourceId.h
    src/Cogs/Register.cpp
    src/Cogs/Register.h
    src/Cogs/Relay.cpp
    src/Cogs/Relay.h
    src/Cogs/Switch.cpp
    src/Cogs/Switch.h
)

set(command_files

    src/Command/CommandAddCogComponent.h
    src/Command/CommandCreateSystem.cpp
    src/Command/CommandCreateSystem.h
    src/Command/CommandDeleteCogComponent.h
    src/Command/CommandRemovedFromHistoryComponent.h
    src/Command/CommandEntityComponent.h
    src/Command/CommandExecuteComponent.h
    src/Command/CommandListComponent.h
    src/Command/CommandListSystem.cpp
    src/Command/CommandListSystem.h
    src/Command/CommandToQueueComponent.h
    src/Command/CommandUndoComponent.h
)

list(APPEND source_files ${core_files})
source_group("Core" FILES ${core_files})

list(APPEND source_files ${cog_files})
source_group("Cogs" FILES ${cog_files})

list(APPEND source_files ${command_files})
source_group("Command" FILES ${command_files})

if (EMSCRIPTEN)
    message(STATUS "This is an Emscripten build.")
    list(APPEND source_files
        src/WebApp.cpp
        src/WebApp.h)

    target_link_options(project_compiler_flags INTERFACE
        -sUSE_WEBGL2=1
        -sMIN_WEBGL_VERSION=2
        -sUSE_GLFW=3
        -sSTACK_SIZE=1mb
        -sEXPORTED_RUNTIME_METHODS=cwrap
        -sALLOW_MEMORY_GROWTH=1
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@./data)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    configure_file(scripts/server.sh.in server.sh)
else()
    list(APPEND source_files
        src/WindowApp.cpp
        src/WindowApp.h
        3rdParty/glad/src/gles2.c)
    set(glfw_dir 3rdParty/glfw-3.4)
    set(glfw_include_dir ${glfw_dir}/include)
    add_subdirectory(${glfw_dir})
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BreadProject)

add_library(BreadProjectLib STATIC ${source_files})
target_include_directories(BreadProjectLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (EMSCRIPTEN)
    target_compile_definitions(BreadProjectLib PRIVATE DATA_DIR="./data/")
else()
    # TODO don't hardcode this path!
    target_compile_definitions(BreadProjectLib PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
endif()

add_executable(BreadProject src/main.cpp)
target_link_libraries(BreadProject PUBLIC BreadProjectLib)

if (NOT EMSCRIPTEN)
    target_include_directories(BreadProjectLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glad/include)
    target_include_directories(BreadProjectLib PUBLIC ${glfw_include_dir})
    target_link_libraries(BreadProjectLib PUBLIC glfw ${GLFW_LIBRARIES})
endif()

add_subdirectory(3rdParty/glm-1.0.1)
target_link_libraries(BreadProjectLib PUBLIC glm::glm-header-only)

target_include_directories(BreadProjectLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/stb)
target_include_directories(BreadProjectLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/nlohmann-json)

add_subdirectory(3rdParty/imgui-1.92.1)
target_include_directories(BreadProjectLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui-1.92.1)
target_link_libraries(BreadProjectLib PUBLIC ImGui)

add_subdirectory(3rdParty/flecs-4.1.1)
target_link_libraries(BreadProjectLib PUBLIC flecs)

target_compile_definitions(BreadProjectLib PRIVATE
    "$<$<COMPILE_LANG_AND_ID:CXX,Emscripten>:EMSCRIPTEN_BUILD>"
)

target_link_libraries(BreadProjectLib PUBLIC project_compiler_flags)

if (NOT EMSCRIPTEN)
    add_subdirectory(3rdParty/Catch2)

    add_executable(BreadTests

        src/ut/CogSystemTest.cpp
        src/ut/CommandCreateSystemTest.cpp
        src/ut/CommandListSystemTest.cpp
        src/ut/OnStageSystemTest.cpp
        src/ut/ResourceIdTest.cpp
        src/ut/ResultTest.cpp
        src/ut/Rotation90Test.cpp
        src/ut/ShadersTest.cpp
        src/ut/UIDragDropSystemTest.cpp
        src/ut/UIDragPreviewSystemTest.cpp
    )

    target_link_libraries(BreadTests PRIVATE Catch2::Catch2WithMain)
    target_link_libraries(BreadTests PRIVATE BreadProjectLib)

    include(Catch)
    catch_discover_tests(BreadTests)
endif()
