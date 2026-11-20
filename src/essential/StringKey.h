#pragma once
#include <string>
#include <optional>

/// <summary>
/// String wrapper. Allows using string_view when performing read operations on containers that use string as a key.
/// </summary>
class StringKey
{
private:
	std::optional<std::string> mString;
	std::string_view mView;

public:
	StringKey() = default;

	StringKey(const StringKey& other) : mString(other.mString)
	{
		if (mString)
			mView = mString.value();
	}

	StringKey& operator=(const StringKey& other)
	{
		mString = other.mString;
		if (mString)
			mView = mString.value();
	}

	StringKey(StringKey&& other) : mString(std::move(other.mString))
	{
		if (mString)
			mView = mString.value();
	}

	StringKey& operator=(StringKey&& other)
	{
		mString = std::move(other.mString);
		if (mString)
			mView = mString.value();
	}

	StringKey(const std::string& str) : mString(str)
	{
		mView = mString.value();
	}

	StringKey& operator=(std::string& str)
	{
		mString = str;
		mView = mString.value();
	}

	StringKey(std::string&& str) : mString(std::move(str))
	{
		mView = mString.value;
	}

	StringKey& operator=(std::string&& str)
	{
		mString = std::move(str);
		mView = mString.value();
	}

	StringKey(const std::string_view& str) : mView(str)
	{
	}

	StringKey& operator=(std::string_view& str)
	{
		mString.reset();
		mView = str;
	}

	std::strong_ordering operator<=>(const StringKey& str) const noexcept
	{
		return mView <=> str.mView;
	}

	bool operator==(const StringKey& str) const noexcept
	{
		return mView == str.mView;
	}

	std::string_view view() const noexcept
	{
		return mView;
	}
};
