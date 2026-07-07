#pragma once

namespace xc
{
	template<typename TItem>
	bool RangeContains(const auto& list, const TItem& item)
	{
		auto itr = std::find(std::begin(list), std::end(list), item);
		return itr != std::end(list);
	}
}