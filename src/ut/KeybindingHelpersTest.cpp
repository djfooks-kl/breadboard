#include "BreadTest.h"

#include "Core/GLFWLib.h"
#include "KeybindingHelpers.h"
#include "KeybindingSettings.h"

namespace
{
    xg::EMappedInput GetBinding(const xg::KeybindingSettings& settings, const int key)
    {
        auto itr = settings.m_Bindings.find(key);
        if (itr == settings.m_Bindings.end())
            return xg::EMappedInput::Invalid;

        return itr->second;
    }
}

TEST_CASE("FillKeybindings", "[xg::KeybindingHelpers]")
{
    SECTION("One binding")
    {
        toml::value binding;
        binding["key"] = "w";
        binding["binding"] = "MoveUp";

        toml::array tomlBindings;
        tomlBindings.push_back(binding);

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 0);
        CHECK(settings.m_Bindings.size() == 1);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
    }

    SECTION("Empty settings -> Error and empty bindings")
    {
        toml::value tomlSettings;
        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.empty());
    }

    SECTION("settings is a string -> Error and empty bindings")
    {
        toml::value tomlSettings = "helloworld";
        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.empty());
    }

    SECTION("Missing bindings -> Error and empty bindings")
    {
        toml::value tomlSettings;
        tomlSettings["somethingElse"];

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.empty());
    }

    SECTION("Binding is not array -> Error and empty bindings")
    {
        toml::value tomlBindings;

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.empty());
    }

    SECTION("Empty bindings -> Empty bindings")
    {
        toml::array tomlBindings;

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 0);
        CHECK(settings.m_Bindings.empty());
    }

    SECTION("Binding value is not a table -> Error but other bindings still parsed")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "w";
            v["binding"] = "MoveUp";
            tomlBindings.push_back(v);
        }
        {
            toml::value v = "hello";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.size() == 1);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
    }

    SECTION("Binding item has key but no binding -> Error but other bindings still parsed")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "w";
            v["binding"] = "MoveUp";
            tomlBindings.push_back(v);
        }
        {
            toml::value v;
            v["key"] = "s";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.size() == 1);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
    }

    SECTION("Binding item has binding but no key -> Error but other bindings still parsed")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "w";
            v["binding"] = "MoveUp";
            tomlBindings.push_back(v);
        }
        {
            toml::value v;
            v["binding"] = "MoveDown";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.size() == 1);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
    }

    SECTION("Binding item has a non-stirng key -> Error but other bindings still parsed")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "w";
            v["binding"] = "MoveUp";
            tomlBindings.push_back(v);
        }
        {
            toml::value v;
            v["key"] = 1;
            v["binding"] = "MoveDown";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.size() == 1);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
    }

    SECTION("Binding item has a non-string binding -> Error but other bindings still parsed")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "w";
            v["binding"] = "MoveUp";
            tomlBindings.push_back(v);
        }
        {
            toml::value v;
            v["key"] = "s";
            v["binding"] = 5;
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.size() == 1);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
    }

    SECTION("Binding item binding string is invalid -> Error but other bindings still parsed")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "w";
            v["binding"] = "MoveUp";
            tomlBindings.push_back(v);
        }
        {
            toml::value v;
            v["key"] = "s";
            v["binding"] = "Yay";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.size() == 1);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
    }

    SECTION("Binding item key string is invalid -> Error but other bindings still parsed")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "w";
            v["binding"] = "MoveUp";
            tomlBindings.push_back(v);
        }
        {
            toml::value v;
            v["key"] = "Yay";
            v["binding"] = "MoveDown";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 1);
        CHECK(settings.m_Bindings.size() == 1);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
    }

    SECTION("Multiple bindings -> Error but other bindings still parsed")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "w";
            v["binding"] = "MoveUp";
            tomlBindings.push_back(v);
        }
        {
            toml::value v;
            v["key"] = "s";
            v["binding"] = "MoveDown";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 0);
        CHECK(settings.m_Bindings.size() == 2);
        CHECK(GetBinding(settings, GLFW_KEY_W) == xg::EMappedInput::MoveUp);
        CHECK(GetBinding(settings, GLFW_KEY_S) == xg::EMappedInput::MoveDown);
    }

    SECTION("'ctrl' key -> Check they are mapped to both left and right GLFW values correctly")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "ctrl";
            v["binding"] = "MoveDown";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 0);
        CHECK(settings.m_Bindings.size() == 2);
        CHECK(GetBinding(settings, GLFW_KEY_LEFT_CONTROL) == xg::EMappedInput::MoveDown);
        CHECK(GetBinding(settings, GLFW_KEY_RIGHT_CONTROL) == xg::EMappedInput::MoveDown);
    }

    SECTION("'shift' key -> Check they are mapped to both left and right GLFW values correctly")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "shift";
            v["binding"] = "MoveDown";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 0);
        CHECK(settings.m_Bindings.size() == 2);
        CHECK(GetBinding(settings, GLFW_KEY_LEFT_SHIFT) == xg::EMappedInput::MoveDown);
        CHECK(GetBinding(settings, GLFW_KEY_RIGHT_SHIFT) == xg::EMappedInput::MoveDown);
    }

    SECTION("'delete' key -> Check they are mapped to GLFW values delete and backspace correctly")
    {
        toml::array tomlBindings;
        {
            toml::value v;
            v["key"] = "delete";
            v["binding"] = "MoveDown";
            tomlBindings.push_back(v);
        }

        toml::value tomlSettings;
        tomlSettings["bindings"] = tomlBindings;

        xg::KeybindingSettings settings;
        std::vector<std::string> errors;
        xg::FillKeybindings(tomlSettings, settings, errors);
        CHECK(errors.size() == 0);
        CHECK(settings.m_Bindings.size() == 2);
        CHECK(GetBinding(settings, GLFW_KEY_DELETE) == xg::EMappedInput::MoveDown);
        CHECK(GetBinding(settings, GLFW_KEY_BACKSPACE) == xg::EMappedInput::MoveDown);
    }
}