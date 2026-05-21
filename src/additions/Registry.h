#pragma once
#include <string>
#include <optional>
#include <vector>

typedef struct HKEY__* HKEY;

namespace Registry
{
	struct Key
	{
		HKEY key;

		std::optional<Key> CreateKey(const std::wstring& path) const;
		std::optional<Key> OpenKey(const std::wstring& path) const;
		std::optional<Key> OpenKeyRead(const std::wstring& path) const;
		bool RemoveKey(const std::wstring& path) const;
		void Close() const;

		std::optional<std::wstring> ReadString(const std::wstring& name) const;
		bool WriteString(const std::wstring& name, const std::wstring& data) const;

		std::optional<std::vector<std::pair<std::wstring, std::wstring>>> EnumStrings() const;
	};

	extern const Key LocalMachineKey;
	std::optional<Key> OpenCurrentUserKey();
}
