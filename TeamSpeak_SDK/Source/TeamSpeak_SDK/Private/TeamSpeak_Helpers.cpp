#include "TeamSpeak_Helpers.h"

namespace com
{
namespace teamspeak
{
namespace helpers
{
	auto utf8_to_fstring(const char* data) -> FString
	{
		if (!data)
			return FString();

		return UTF8_TO_TCHAR(data);
	}

	auto fstring_to_utf8(const TCHAR* value) -> std::string
	{
		if (!value)
			return std::string();

		return TCHAR_TO_UTF8(value);
	}
}
}
}
