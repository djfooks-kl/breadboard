#include "MappedInputEnum.h"

#include <string>

const char* xg::ToString(const xg::EMappedInput v)
{
    static_assert(static_cast<int>(xg::EMappedInput::MAX) == 11);
    switch (v)
    {
        case xg::EMappedInput::Invalid: return "Invalid";

        case xg::EMappedInput::Build: return "Build";
        case xg::EMappedInput::Delete: return "Delete";
        case xg::EMappedInput::MoveDown: return "MoveDown";
        case xg::EMappedInput::MoveLeft: return "MoveLeft";
        case xg::EMappedInput::MoveRight: return "MoveRight";
        case xg::EMappedInput::MoveUp: return "MoveUp";
        case xg::EMappedInput::Multiselect: return "Multiselect";
        case xg::EMappedInput::Rotate: return "Rotate";
        case xg::EMappedInput::ZoomIn: return "ZoomIn";
        case xg::EMappedInput::ZoomOut: return "ZoomOut";

        case xg::EMappedInput::MAX: return "MAX";
    }
    return "Unknown";
}

xg::EMappedInput xg::MappedInputFromString(const std::string& s)
{
    static_assert(static_cast<int>(xg::EMappedInput::MAX) == 11);
    if (s == "Build") return xg::EMappedInput::Build;
    if (s == "Delete") return xg::EMappedInput::Delete;
    if (s == "MoveDown") return xg::EMappedInput::MoveDown;
    if (s == "MoveLeft") return xg::EMappedInput::MoveLeft;
    if (s == "MoveRight") return xg::EMappedInput::MoveRight;
    if (s == "MoveUp") return xg::EMappedInput::MoveUp;
    if (s == "Multiselect") return xg::EMappedInput::Multiselect;
    if (s == "Rotate") return xg::EMappedInput::Rotate;
    if (s == "ZoomIn") return xg::EMappedInput::ZoomIn;
    if (s == "ZoomOut") return xg::EMappedInput::ZoomOut;
    return xg::EMappedInput::Invalid;
}
