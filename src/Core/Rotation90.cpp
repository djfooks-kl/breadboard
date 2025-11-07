#include "Core/Rotation90.h"

#include <array>
#include <ostream>

namespace
{
	const std::array<glm::imat2x2, 4> s_IMatrices = {
		glm::imat2x2(1, 0, 0, 1),
		glm::imat2x2(0, -1, 1, 0),
		glm::imat2x2(-1, 0, 0, -1),
		glm::imat2x2(0, 1, -1, 0)
	};

	const std::array<glm::mat2x2, 4> s_FMatrices = {
		glm::mat2x2(1.f, 0.f, 0.f, 1.f),
		glm::mat2x2(0.f, -1.f, 1.f, 0.f),
		glm::mat2x2(-1.f, 0.f, 0.f, -1.f),
		glm::mat2x2(0.f, 1.f, -1.f, 0.f)
	};

	int WrapIndex(const int v)
	{
		return (v % 4 + 4) % 4;
	}
}

xc::Rotation90::Rotation90(int rotationIndex)
	: m_Index(WrapIndex(rotationIndex))
{
}

void xc::Rotation90::RotateRight(int rotationIndexOffset)
{
	m_Index = WrapIndex(m_Index + rotationIndexOffset);
}

int xc::Rotation90::GetRotationIndex() const
{
	return m_Index;
}

const glm::imat2x2& xc::Rotation90::GetIMatrix() const
{
	return s_IMatrices[m_Index];
}

const glm::mat2x2& xc::Rotation90::GetFMatrix() const
{
	return s_FMatrices[m_Index];
}

xc::Rotation90 xc::Rotation90::operator+(const xc::Rotation90& other) const
{
	return xc::Rotation90(WrapIndex(m_Index + other.m_Index));
}

void xc::Rotation90::operator+=(const xc::Rotation90& other)
{
	RotateRight(other.m_Index);
}

std::ostream& xc::operator<<(std::ostream& os, const xc::Rotation90& obj)
{
	return os << "Rotation90(" << obj.GetRotationIndex() << ")";
}