#pragma once

#include <unordered_set>

#include "MappedInputEnum.h"

namespace xg
{
    struct MappedInputComponent
    {
        std::unordered_set<xg::EMappedInput> m_KeyDown;
        std::unordered_set<xg::EMappedInput> m_KeyPress;
        std::unordered_set<xg::EMappedInput> m_KeyRelease;
    };
}