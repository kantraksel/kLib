// kLib: Last changed 21.05.2026
#include "Registry.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace Registry;

const Key Registry::LocalMachineKey = { HKEY_LOCAL_MACHINE };

std::optional<Key> Registry::OpenCurrentUserKey()
{
	HKEY key;
	auto result = RegOpenCurrentUser(KEY_ALL_ACCESS, &key);
	return result == ERROR_SUCCESS ? std::optional<Key>{ key } : std::nullopt;
}

std::optional<Key> Key::CreateKey(const std::wstring& path) const
{
	HKEY outKey;
	auto result = RegCreateKeyEx(key, path.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &outKey, nullptr);
	return result == ERROR_SUCCESS ? std::optional<Key>{ outKey } : std::nullopt;
}

std::optional<Key> Key::OpenKey(const std::wstring& path) const
{
	HKEY outKey;
	auto result = RegOpenKeyEx(key, path.c_str(), 0, KEY_ALL_ACCESS, &outKey);
	return result == ERROR_SUCCESS ? std::optional<Key>{ outKey } : std::nullopt;
}

std::optional<Key> Key::OpenKeyRead(const std::wstring& path) const
{
	HKEY outKey;
	auto result = RegOpenKeyEx(key, path.c_str(), 0, KEY_READ, &outKey);
	return result == ERROR_SUCCESS ? std::optional<Key>{ outKey } : std::nullopt;
}

void Key::Close() const
{
	RegCloseKey(key);
}

bool Key::RemoveKey(const std::wstring& path) const
{
	return RegDeleteKey(key, path.c_str()) == ERROR_SUCCESS;
}

std::optional<std::wstring> Key::ReadString(const std::wstring& name) const
{
	DWORD size, type;
	auto result = RegQueryValueEx(key, name.c_str(), 0, &type, nullptr, &size);
	if (result != ERROR_SUCCESS || type != REG_SZ)
		return {};

	std::wstring out;
	out.resize(size / sizeof(wchar_t));

	result = RegQueryValueEx(key, name.c_str(), 0, nullptr, (BYTE*)out.data(), &size);
	if (result != ERROR_SUCCESS)
		return {};

	size /= sizeof(wchar_t);
	if (size > 0 && out[size - 1] == 0)
		size -= 1;
	out.resize(size);
	return out;
}

bool Key::WriteString(const std::wstring& name, const std::wstring& data) const
{
	auto size = (data.size() + 1) * sizeof(wchar_t);
	return RegSetValueEx(key, name.c_str(), 0, REG_SZ, (const BYTE*)data.c_str(), (DWORD)size) == ERROR_SUCCESS;
}

std::optional<std::vector<std::pair<std::wstring, std::wstring>>> Key::EnumStrings() const
{
	std::vector<std::pair<std::wstring, std::wstring>> values;

	for (DWORD index = 0;; ++index)
	{
		DWORD nameSize = 256;
		DWORD dataSize = 0;
		DWORD type;

		std::wstring name;
		name.resize(nameSize - 1);

		auto result = RegEnumValue(key, index, name.data(), &nameSize, NULL, &type, nullptr, &dataSize);
		if (result != ERROR_SUCCESS)
			break;
		if (type != REG_SZ)
			continue;

		name.resize(nameSize);
		nameSize += 1;

		std::wstring data;
		data.resize(dataSize / sizeof(wchar_t));

		result = RegEnumValue(key, index, name.data(), &nameSize, NULL, nullptr, (BYTE*)data.data(), &dataSize);
		if (result != ERROR_SUCCESS)
			break;

		name.resize(nameSize);

		dataSize /= sizeof(wchar_t);
		if (dataSize > 0 && data[dataSize - 1] == 0)
			dataSize -= 1;
		data.resize(dataSize);

		values.push_back(std::make_pair(name, data));
	}

	return values;
}
