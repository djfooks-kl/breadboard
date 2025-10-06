#pragma once

#include <memory>
#include <unordered_map>

#include "Cogs/Cog.h"
#include "Cogs/CogResourceId.h"

namespace xg
{
    struct CogMap
    {
        CogMap() {}
        CogMap(const CogMap&) = delete;
        CogMap& operator=(const CogMap&) = delete;

        const xg::Cog* Get(xg::CogResourceId cogId) const;

        const std::unordered_map<xg::CogResourceId, std::unique_ptr<xg::Cog>>& GetMap() const { return m_Map; }

        template<typename T>
        void Register()
        {
            m_Map[T::GetResourceId()] = std::make_unique<T>();
        }

    private:
        std::unordered_map<xg::CogResourceId, std::unique_ptr<xg::Cog>> m_Map;
    };
}