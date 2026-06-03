#pragma once

#include <toml11/toml.hpp>

namespace xg
{
	struct RenderSettings;
	struct UISettings;

	void FillSettings(
		const toml::value& settings,
		xg::RenderSettings& out_RenderSettings,
		xg::UISettings& out_UISettings);
}