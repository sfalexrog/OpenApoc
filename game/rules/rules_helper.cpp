#include "game/rules/rules_helper.h"

#include "framework/logger.h"

namespace OpenApoc
{

bool FromString(const UString &str, Vec2<int> &output)
{
	auto splitString = str.split(",");

	if (splitString.size() != 2)
	{
		LogWarning("String \"%s\" doesn't look like a vec2<int> (invalid element count)",
		           str.c_str());
		return false;
	}

	int x, y;
	if (!FromString(splitString[0], x) || !FromString(splitString[1], y))

	{
		LogWarning("String \"%s\" doesn't look like a vec2<int> (non-integer element)",
		           str.c_str());
		return false;
	}
	output = {x, y};
	return true;
}

bool FromString(const UString &str, Vec2<float> &output)
{
	auto splitString = str.split(",");

	if (splitString.size() != 2)
	{
		LogWarning("String \"%s\" doesn't look like a vec2<float> (invalid element count)",
		           str.c_str());
		return false;
	}

	float x, y;
	if (!FromString(splitString[0], x) || !FromString(splitString[1], y))
	{
		LogWarning("String \"%s\" doesn't look like a vec2<float> (non-float element)",
		           str.c_str());
		return false;
	}
	output = {x, y};
	return true;
}

bool FromString(const UString &str, Vec3<int> &output)
{
	auto splitString = str.split(",");

	if (splitString.size() != 3)
	{

		LogWarning("String \"%s\" doesn't look like a vec3<int> (invalid element count)",
		           str.c_str());
		return false;
	}

	int x, y, z;
	if (!FromString(splitString[0], x) || !FromString(splitString[1], y) ||
	    !FromString(splitString[2], z))
	{
		LogWarning("String \"%s\" doesn't look like a vec3<int> (non-integer element)",
		           str.c_str());
		return false;
	}
	output = Vec3<int>{x, y, z};
	return true;
}

bool FromString(const UString &str, Vec3<float> &output)
{
	auto splitString = str.split(",");

	if (splitString.size() != 3)
	{

		LogWarning("String \"%s\" doesn't look like a vec3<float> (invalid element count)",
		           str.c_str());
		return false;
	}

	float x, y, z;
	if (!FromString(splitString[0], x) || !FromString(splitString[1], y) ||
	    !FromString(splitString[2], z))
	{
		LogWarning("String \"%s\" doesn't look like a vec3<float> (non-integer element)",
		           str.c_str());
		return false;
	}
	output = Vec3<int>{x, y, z};
	return true;
}

bool FromString(const UString &str, bool &output)
{
	if (str == "true")
	{
		output = true;
		return true;
	}
	else if (str == "false")
	{
		output = false;
		return true;
	}

	LogWarning("String \"%s\" doesn't liik like a bool", str.c_str());
	return false;
}

bool FromString(const UString &str, UString &output)
{
	output = str;
	return true;
}

bool FromString(const UString &str, float &output)
{
	if (!Strings::IsFloat(str))
	{
		LogWarning("String \"%s\" doesn't look like a float", str.c_str());
		return false;
	}

	output = Strings::ToFloat(str);
	return true;
}

bool FromString(const UString &str, int &output)
{
	if (!Strings::IsInteger(str))
	{
		LogWarning("Element \"%s\" doesn't look like an integer", str.c_str());
		return false;
	}

	output = Strings::ToInteger(str);
	return true;
}

bool FromString(const UString &str, Colour &output)
{
	int r, g, b, a;

	auto splitString = str.split(",");

	if (splitString.size() != 3 && splitString.size() != 4)
	{
		LogWarning("String \"%s\" doesn't look like a colour (invalid element count)", str.c_str());
		return false;
	}

	if (!Strings::IsInteger(splitString[0]) || !Strings::IsInteger(splitString[1]) ||
	    !Strings::IsInteger(splitString[2]))
	{
		LogWarning("String \"%s\" doesn't look like a colour (non-integer element)", str.c_str());
		return false;
	}

	r = Strings::ToInteger(splitString[0]);
	g = Strings::ToInteger(splitString[1]);
	b = Strings::ToInteger(splitString[2]);

	if (splitString.size() == 4)
	{
		if (!Strings::IsInteger(splitString[3]))
		{
			LogWarning("String \"%s\" doesn't look like a colour (non-integer element)",
			           str.c_str());
			return false;
		}
		a = Strings::ToInteger(splitString[3]);
	}
	else
		a = 255;

	if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255 || a < 0 || a > 255)
	{
		LogWarning("String \"%s\" doesn't look like a colour (out-of-range element)", str.c_str());
		return false;
	}

	if (!Strings::IsInteger(str))
	{
		LogWarning("String \"%s\" doesn't look like an integer", str.c_str());
		return false;
	}

	output = Colour{static_cast<unsigned char>(r), static_cast<unsigned char>(g),
	                static_cast<unsigned char>(b), static_cast<unsigned char>(a)};
	return true;
}

}; // namespace OpenApoc
