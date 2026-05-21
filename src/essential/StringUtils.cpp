#include <format>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#if _DEBUG
	#include <comdef.h>
#endif
#include "StringUtils.h"

std::wstring StringUtils::Utf8ToWideString(const std::string_view& str)
{
	std::wstring out;

	auto length = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
	if (length != 0)
	{
		out.resize(length);
		length = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), out.data(), (int)out.size());
	}

	if (length == 0)
	{
		out.clear();
		out.reserve(str.size());
		for (char ch : str)
		{
			out.push_back((wchar_t)ch);
		}
	}

	return out;
}

std::string StringUtils::WideStringToUtf8(const std::wstring_view& str)
{
	std::string out;

	auto length = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.length(), nullptr, 0, NULL, NULL);
	if (length != 0)
	{
		out.resize(length);
		length = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.length(), out.data(), (int)out.size(), NULL, NULL);
	}

	if (length == 0)
	{
		out.clear();
		out.reserve(str.size());
		for (wchar_t ch : str)
		{
			out.push_back((char)ch);
		}
	}

	return out;
}

template <typename T>
inline static char* ToString(char* ptr, T n)
{
	*ptr = 0;

	do
	{
		--ptr;
		*ptr = '0' + n % 10;
		n /= 10;
	} while (n != 0);

	return ptr;
}

std::string StringUtils::ToString(unsigned long long n)
{
	char buff[32];
	constexpr int max = sizeof(buff) - 1;

	return ::ToString(buff + max, n);
}

std::string StringUtils::ToString(long long n)
{
	bool sign = false;
	if (n < 0)
	{
		sign = true;
		n = 0 - n;
	}

	char buff[32];
	constexpr int max = sizeof(buff) - 1;

	char* ptr = ::ToString(buff + max, n);

	if (sign)
		*--ptr = '-';
	return ptr;
}

#if _DEBUG
std::string StringUtils::Format(HRESULT hr)
{
	if (hr == E_FAIL)
		return std::format("{:X} - Unknown error", hr);

	_com_error err(hr);
	auto str = StringUtils::WideStringToUtf8(err.ErrorMessage());
	return std::format("{:X} - {}", hr, str);
}

std::wstring StringUtils::FormatW(HRESULT hr)
{
	if (hr == E_FAIL)
		return std::format(L"{:X} - Unknown error", hr);

	_com_error err(hr);
	return std::format(L"{:X} - {}", hr, err.ErrorMessage());
}
#endif

static inline bool IsWhiteSpace(char c)
{
	switch (c)
	{
	case 0x09:
	case 0x0A:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x20:
		return true;

	default:
		return false;
	}
}

std::string StringUtils::Trim(const std::string& str)
{
	if (str.empty())
		return {};

	int start = -1;
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!IsWhiteSpace(str[i]))
		{
			start = i;
			break;
		}
	}
	if (start == -1)
		return {};

	int end = 0;
	for (int i = str.length() - 1; i >= 0; --i)
	{
		if (!IsWhiteSpace(str[i]))
		{
			end = i;
			break;
		}
	}

	return str.substr(start, end - start + 1);
}
