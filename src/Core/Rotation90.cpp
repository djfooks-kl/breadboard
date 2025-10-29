#include "Core/Rotation90.h"

#include <array>

namespace
{
	const std::array<glm::imat2x2, 4> s_IRotations = {
		glm::imat2x2(1, 0, 0, 1),
		glm::imat2x2(0, -1, 1, 0),
		glm::imat2x2(-1, 0, 0, -1),
		glm::imat2x2(0, 1, -1, 0)
	};

	const std::array<glm::mat2x2, 4> s_FRotations = {
		glm::mat2x2(1.f, 0.f, 0.f, 1.f),
		glm::mat2x2(0.f, -1.f, 1.f, 0.f),
		glm::mat2x2(-1.f, 0.f, 0.f, -1.f),
		glm::mat2x2(0.f, 1.f, -1.f, 0.f)
	};
}

void xc::Rotation90::RotateRight(int rotationIndexOffset)
{
	m_Index += rotationIndexOffset;
	m_Index = (m_Index % 4 + 4) % 4;
}

int xc::Rotation90::GetRotationIndex() const
{
	return m_Index;
}

const glm::imat2x2& xc::Rotation90::GetIRotation() const
{
	return s_IRotations[m_Index];
}

const glm::mat2x2& xc::Rotation90::GetFRotation() const
{
	return s_FRotations[m_Index];
}