#pragma once

#include <memory>
#include <unordered_map>

#include "Cogs/CogPrototype.h"
#include "Cogs/CogResourceId.h"

namespace xg
{
    struct CogMap
    {
        CogMap() {}
        CogMap(const CogMap&) = delete;
        CogMap& operator=(const CogMap&) = delete;

        const xg::CogPrototype* Get(xg::CogResourceId cogId) const;

        const std::unordered_map<xg::CogResourceId, std::unique_ptr<xg::CogPrototype>>& GetMap() const { return m_Map; }

        template<typename T>
        void Register()
        {
            std::unique_ptr<T> cog = std::make_unique<T>();
            m_Map[cog->GetResourceId()] = std::move(cog);
        }

    private:
        std::unordered_map<xg::CogResourceId, std::unique_ptr<xg::CogPrototype>> m_Map;
    };
}