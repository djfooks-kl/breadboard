#pragma once

#include <string>

namespace xg
{
    enum class EMappedInput : uint8_t
    {
        Invalid,

        Build,
        Delete,
        MoveDown,
        MoveLeft,
        MoveRight,
        MoveUp,
        Multiselect,
        Rotate,
        ZoomIn,
        ZoomOut,

        MAX
    };

    const char* ToString(xg::EMappedInput v);
    xg::EMappedInput MappedInputFromString(const std::string& s);
}