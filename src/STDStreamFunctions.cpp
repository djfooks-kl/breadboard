#include "STDStreamFunctions.h"

std::ostream& operator<<(std::ostream& os, const glm::ivec2& v)
{
	os << "[" << v[0] << ", " << v.y << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec2& v)
{
	os << "[" << v[0] << ", " << v.y << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
	os << "[" << v[0] << ", " << v.y << ", " << v.z << "]";
	return os;
}
