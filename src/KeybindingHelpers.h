#pragma once

#include <string>
#include <toml11/toml.hpp>
#include <vector>

namespace xg
{
	struct KeybindingSettings;

	void FillKeybindings(
		const toml::value& settings,
		xg::KeybindingSettings& out_KeybindingSettings,
		std::vector<std::string>& out_errors);

	void FillKeybindings(
		const toml::value& settings,
		xg::KeybindingSettings& out_KeybindingSettings);
}