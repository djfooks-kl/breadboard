#pragma once

#include <glm/ext/matrix_int2x2.hpp>
#include <glm/ext/matrix_float2x2.hpp>

namespace xc
{
	class Rotation90
	{
	public:
		void RotateRight(int rotationIndexOffset);

		int GetRotationIndex() const;

		const glm::imat2x2& GetIRotation() const;
		const glm::mat2x2& GetFRotation() const;

	private:
		int m_Index = 0;
	};
}