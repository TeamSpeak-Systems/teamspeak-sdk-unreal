#pragma once

#include "CoreMinimal.h"

#include <string>

/**
 * 
 */
namespace com
{
	namespace teamspeak
	{
		namespace helpers
		{
			auto utf8_to_fstring(const char* data) -> FString;
			auto fstring_to_utf8(const TCHAR* value) -> std::string;
		}
	}
}
