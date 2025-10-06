#include "Cogs/CogMap.h"

const xg::Cog* xg::CogMap::Get(xg::CogResourceId cogId) const
{
    auto itr = m_Map.find(cogId);
    if (itr != m_Map.end())
    {
        return itr->second.get();
    }
    return nullptr;
}