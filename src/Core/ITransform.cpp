#include "Core/ITransform.h"

#include <ostream>

xc::ITransform xc::ITransform::Inverse() const
{
    xc::Rotation90 newRotation(-m_Rotation.GetRotationIndex());
    return { .m_Translation = -newRotation.Apply(m_Translation), .m_Rotation = newRotation };
}

glm::ivec2 xc::ITransform::Apply(const glm::ivec2& v) const
{
    return m_Translation + m_Rotation.Apply(v);
}

glm::ivec2 xc::ITransform::ApplyInverse(const glm::ivec2& worldV) const
{
    return m_Rotation.ApplyInverse(worldV - m_Translation);
}

xc::ITransform xc::ITransform::WithLocalTranslation(const glm::ivec2& v) const
{
    return xc::ITransform{ .m_Translation = Apply(v), .m_Rotation = m_Rotation };
}

xc::ITransform xc::ITransform::operator*(const xc::ITransform& rhs) const
{
    return xc::ITransform{
        .m_Translation = m_Rotation.Apply(rhs.m_Translation) + m_Translation,
        .m_Rotation = m_Rotation + rhs.m_Rotation
    };
}

std::ostream& xc::operator<<(std::ostream& os, const xc::ITransform& obj)
{
    return os <<
        "{ Translate=[" << obj.m_Translation.x << ", " << obj.m_Translation.y << "]"
        ", Rotation90=" << obj.m_Rotation.GetRotationIndex() <<
        "}";
}
