#include "SettingsHelpers.h"

#include <glm/vec3.hpp>
#include <toml11/toml.hpp>

#include "RenderSettings.h"

namespace
{
	template<typename T>
	T Parse(const toml::value& settings, const char* name, const T& defaultValue)
	{
		return toml::find_or<T>(settings, name, defaultValue);
	}

	template<typename T>
	T Parse(const toml::value& settings, const char* name, std::string defaultValue)
	{
		return T(toml::find_or<std::string>(settings, name, defaultValue));
	}
}

void xg::FillSettings(const toml::value& settings, xg::RenderSettings& out_RenderSettings)
{
#define ADD_SETTING(TYPE, NAME, DEFAULT_VALUE) out_RenderSettings.m_##NAME = Parse<TYPE>(settings, #NAME, DEFAULT_VALUE);
#include "RenderSettingsList.h"
#undef ADD_SETTING
}