#pragma once

#include <glm/ext/matrix_int2x2.hpp>
#include <glm/ext/matrix_float2x2.hpp>
#include <iosfwd>

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
		Rotation90 operator+(const xc::Rotation90& other) const;
		void operator+=(const xc::Rotation90& other);

	private:
		int m_Index = 0;
	};

	std::ostream& operator<<(std::ostream& os, const Rotation90& obj);
}