#pragma once

#include <array>
#include <glm/ext/matrix_float2x2.hpp>
#include <glm/ext/matrix_int2x2.hpp>
#include <iosfwd>

namespace xc
{
	using Rotation90UVs = std::array<int, 8>;

	class Rotation90
	{
	public:
		Rotation90() = default;
		Rotation90(int rotationIndex);

		void RotateClockwise(int rotationIndexOffset);

		int GetRotationIndex() const;

		const glm::imat2x2& GetIMatrix() const;
		const glm::mat2x2& GetFMatrix() const;

		const xc::Rotation90UVs& GetUVs() const;

		[[nodiscard]] glm::vec2 Apply(const glm::vec2& v) const;
		[[nodiscard]] glm::ivec2 Apply(const glm::ivec2& v) const;
		[[nodiscard]] glm::vec2 ApplyInverse(const glm::vec2& v) const;
		[[nodiscard]] glm::ivec2 ApplyInverse(const glm::ivec2& v) const;

		bool operator==(const xc::Rotation90& other) const = default;
		Rotation90 operator+(const xc::Rotation90& other) const;
		void operator+=(const xc::Rotation90& other);

	private:
		int m_Index = 0;
	};

	std::ostream& operator<<(std::ostream& os, const Rotation90& obj);
}