#pragma once

#include <glm/ext/matrix_int2x2.hpp>
#include <glm/ext/matrix_float2x2.hpp>

namespace xc
{
	class Rotation90
	{
	public:
		Rotation90() = default;
		Rotation90(int rotationIndex);

		void RotateRight(int rotationIndexOffset);

		int GetRotationIndex() const;

		const glm::imat2x2& GetIMatrix() const;
		const glm::mat2x2& GetFMatrix() const;

		bool operator==(const xc::Rotation90& other) const = default;

	private:
		int m_Index = 0;
	};
}