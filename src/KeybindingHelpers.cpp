#include "KeybindingHelpers.h"

#include "Core/GLFWLib.h"
#include "MappedInputEnum.h"
#include "KeybindingSettings.h"

namespace
{
	void AddError(std::vector<std::string>& out_errors, const toml::value& value, const char* msg)
	{
		out_errors.push_back(std::format("Invalid keybinding at {}:{}. {}",
			value.location().file_name(),
			value.location().first_line_number(),
			msg));
	}
}

void xg::FillKeybindings(
	const toml::value& settings,
	xg::KeybindingSettings& out_KeybindingSettings,
	std::vector<std::string>& out_errors)
{
	if (settings.is_empty())
	{
		AddError(out_errors, settings, "Settings is empty");
		return;
	}

	if (!settings.is_table())
	{
		AddError(out_errors, settings, "Settings must be a table");
		return;
	}

	if (!settings.contains("bindings"))
	{
		AddError(out_errors, settings, "Settings must contain a 'bindings'");
		return;
	}

	const toml::value& bindingsList = toml::find(settings, "bindings");
	if (!bindingsList.is_array())
	{
		AddError(out_errors, bindingsList, "Settings 'bindings' must be an array");
		return;
	}

	for (const toml::value& value : bindingsList.as_array())
	{
		if (!value.is_table())
		{
			AddError(out_errors, value, "Value must be a table");
			continue;
		}

		if (!value.contains("key"))
		{
			AddError(out_errors, value, "Value must contain a string 'key'");
			continue;
		}

		if (!value.contains("binding"))
		{
			AddError(out_errors, value, "Value must contain a string 'binding'");
			continue;
		}

		const toml::value& tomlKey = toml::find(value, "key");
		const toml::value& tomlBinding = toml::find(value, "binding");

		if (!tomlKey.is_string())
		{
			AddError(out_errors, value, "Value must contain a string 'key'");
			continue;
		}

		if (!tomlBinding.is_string())
		{
			AddError(out_errors, value, "Value must contain a string 'binding'");
			continue;
		}

		const xg::EMappedInput binding = xg::MappedInputFromString(tomlBinding.as_string());
		if (binding == xg::EMappedInput::Invalid)
		{
			AddError(out_errors, value, std::format("Unknown 'binding' string '{}'", tomlBinding.as_string()).c_str());
			continue;
		}

		const std::string& keyString = tomlKey.as_string();

		if (keyString.size() == 1)
		{
			out_KeybindingSettings.m_Bindings[std::toupper(tomlKey.as_string()[0])] = binding;
		}
		else if (keyString == "ctrl")
		{
			out_KeybindingSettings.m_Bindings[GLFW_KEY_LEFT_CONTROL] = binding;
			out_KeybindingSettings.m_Bindings[GLFW_KEY_RIGHT_CONTROL] = binding;
		}
		else if (keyString == "shift")
		{
			out_KeybindingSettings.m_Bindings[GLFW_KEY_LEFT_SHIFT] = binding;
			out_KeybindingSettings.m_Bindings[GLFW_KEY_RIGHT_SHIFT] = binding;
		}
		else if (keyString == "delete")
		{
			out_KeybindingSettings.m_Bindings[GLFW_KEY_DELETE] = binding;
			out_KeybindingSettings.m_Bindings[GLFW_KEY_BACKSPACE] = binding;
		}
		else
		{
			AddError(out_errors, value, std::format("Unknown 'key' string '{}'", tomlKey.as_string()).c_str());
			continue;
		}
	}
}

void xg::FillKeybindings(
	const toml::value& settings,
	xg::KeybindingSettings& out_KeybindingSettings)
{
	std::vector<std::string> errors;
	xg::FillKeybindings(settings, out_KeybindingSettings, errors);
	for (const std::string& error : errors)
	{
		printf("ERROR: %s\n", error.c_str());
	}
}