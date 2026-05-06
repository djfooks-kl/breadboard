#pragma once

#include <toml11/toml.hpp>

namespace xg
{
	struct RenderSettings;

	void FillSettings(const toml::value& settings, xg::RenderSettings& out_RenderSettings);
}