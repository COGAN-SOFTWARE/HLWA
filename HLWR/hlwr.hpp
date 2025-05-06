#pragma once

#include <string>
#include <variant>
#include <vector>

#define CS_GWR_R                     int

#define CS_GWR_R_SUCCESS             0b00000001
#define CS_GWR_R_KEYNOTFOUND         0b00000010
#define CS_GWR_R_ENTRYNOTFOUND       0b00000100
#define CS_GWR_R_NOTADMIN            0b00001000
#define CS_GWR_R_INVALIDARGS         0b00010000
#define CS_GWR_R_INVALIDTYPE         0b00100000
#define CS_GWR_R_INVALID             0b01000000 // Typically used if the system cannot retrieve a key/value; suggestive if the program doesn't have the permissions.
#define CS_GWR_R_KEYALREADYEXISTS    0b10000000 // This gets paired with CS_GWR_R_SUCCESS.

#if defined(CS_GWR_USEUTF8STRINGS)
#define CS_GWR_DEFAULTENTRY          ""
#define CS_GWR_STRING                std::string
#else
#define CS_GWR_DEFAULTENTRY          L""        // Default entry in a key uses no string as the name.
#define CS_GWR_STRING                std::wstring
#endif

#if defined(_WIN32)
#include <Windows.h>

namespace CoganSoftware::HLWR {
	static inline CS_GWR_R IsAdmin() {
		BOOL isElevated = FALSE;
		HANDLE token = NULL;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
			TOKEN_ELEVATION elevation;
			DWORD size;

			if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
				isElevated = elevation.TokenIsElevated;
			}
			CloseHandle(token);
		} else {
			return CS_GWR_R_NOTADMIN;
		}

		return CS_GWR_R_SUCCESS;
	}

	enum struct EntryType {
		DWORD,  // REG_DWORD
		BINARY, // REG_BINARY
		STRING  // REG_SZ
	};
	struct Entry {
	public:
		Entry(const CS_GWR_STRING& _name, EntryType _type, uint32_t* inDword, std::vector<uint8_t>* inBinary, CS_GWR_STRING* inString) : name{ _name }, type{ type } {
			creationResult = SetData(inDword, inBinary, inString);
		};
		~Entry() {};
		
		CS_GWR_R GetCreationResult() { return creationResult; };

		void ForceDirty() { dirty = true; };
		bool IsDirty() const { return dirty; };

		const CS_GWR_STRING& GetName() const { return name; };
		void SetName(const CS_GWR_STRING& _name) { name = _name; dirty = true; };

		EntryType GetType() { return type; };
		// Setting the type also clears the data.
		CS_GWR_R SetType(EntryType _type) {
			type = _type;
			static uint32_t defDword = 0;
			static std::vector<uint8_t> defBinary{};
			static CS_GWR_STRING defString = CS_GWR_DEFAULTENTRY;
			CS_GWR_R r = SetData(&defDword, &defBinary, &defString);
			dirty = true;
			return r;
		};
		CS_GWR_R GetData(uint32_t* outDword, std::vector<uint8_t>* outBinary, CS_GWR_STRING* outString) {
			switch (type) {
			case(EntryType::DWORD):
				if (outDword == nullptr) return CS_GWR_R_INVALIDARGS;
				*outDword = std::get<uint32_t>(data);
				break;
			case(EntryType::BINARY):
				if (outBinary == nullptr) return CS_GWR_R_INVALIDARGS;
				*outBinary = std::get<std::vector<uint8_t>>(data);
				break;
			case(EntryType::STRING):
				if (outString == nullptr) return CS_GWR_R_INVALIDARGS;
				*outString = std::get<CS_GWR_STRING>(data);
				break;
			}
			return CS_GWR_R_SUCCESS;
		}
		CS_GWR_R SetData(uint32_t* inDword, std::vector<uint8_t>* inBinary, CS_GWR_STRING* inString) {
			switch (type) {
			case(EntryType::DWORD):
				if (inDword == nullptr) return CS_GWR_R_INVALIDARGS;
				data = *inDword;
				break;
			case(EntryType::BINARY):
				if (inBinary == nullptr) return CS_GWR_R_INVALIDARGS;
				data = *inBinary;
				break;
			case(EntryType::STRING):
				if (inString == nullptr) return CS_GWR_R_INVALIDARGS;
				data = *inString;
				break;
			}
			dirty = true;
			return CS_GWR_R_SUCCESS;
		}
	private:
		CS_GWR_STRING name;
		CS_GWR_R creationResult;
		EntryType type;
		std::variant<uint32_t, std::vector<uint8_t>, CS_GWR_STRING> data{};
		bool dirty = false;
	};
	
	enum struct RootKey {
		ClassesRoot,
		CurrentConfig,
		CurrentUser,
		CurrentUserLocalSettings,
		LocalMachine,
		Users
	};
	class Key {
	public:
		Key(const CS_GWR_STRING& _path, RootKey root) : path{ _path } {
			switch (root) {
			case(RootKey::ClassesRoot):
				parent = HKEY_CLASSES_ROOT;
				break;
			case(RootKey::CurrentConfig):
				parent = HKEY_CURRENT_CONFIG;
				break;
			case(RootKey::CurrentUser):
				parent = HKEY_CURRENT_USER;
				break;
			case(RootKey::CurrentUserLocalSettings):
				parent = HKEY_CURRENT_USER_LOCAL_SETTINGS;
				break;
			case(RootKey::LocalMachine):
				parent = HKEY_LOCAL_MACHINE;
				break;
			case(RootKey::Users):
				parent = HKEY_USERS;
				break;
			}

			InitializeKey();
		};
		Key(const Key& other) : key{ other.key }, owner{ false } {}
		Key(Key&& other) noexcept : key{ std::move(other.key) }, owner{ true } {}
		~Key() {
			if (owner && key) RegCloseKey(key);
		};
		
		CS_GWR_R GetCreationResult() { return creationResult; };

		CS_GWR_R Load() {
#if defined(CS_GWR_USEUTF8STRINGS)
			char subKeyName[256];
#else
			wchar_t subKeyName[256];
#endif
			DWORD subKeyLength;
			DWORD i = 0;
			while (true) {
				subKeyLength = sizeof(subKeyName);
#if defined(CS_GWR_USEUTF8STRINGS)
				HRESULT r = RegEnumKeyExA(key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#else
				HRESULT r = RegEnumKeyExW(key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#endif
				if (r == ERROR_NO_MORE_ITEMS) break;
				if (r != ERROR_SUCCESS) {
					return CS_GWR_R_INVALID;
				}
				childKeys.push_back({ subKeyName, key });
			}
			
#if defined(CS_GWR_USEUTF8STRINGS)
			char valueName[256];
#else
			wchar_t valueName[256];
#endif
			BYTE data[1024];
			DWORD valueLength;
			DWORD dataLength;
			DWORD type;
			i = 0;
			while (true) {
				valueLength = sizeof(valueName);
				dataLength = sizeof(data);
#if defined(CS_GWR_USEUTF8STRINGS)
				HRESULT r = RegEnumValueA(key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#else
				HRESULT r = RegEnumValueW(key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#endif
				if (r == ERROR_NO_MORE_ITEMS) break;
				if (r != ERROR_SUCCESS) {
					return CS_GWR_R_INVALID;
				}

				if (type == REG_DWORD && dataLength >= sizeof(DWORD)) {
					uint32_t dw = *(DWORD*)data;
					entries.push_back({ valueName, EntryType::DWORD, &dw, nullptr, nullptr });
				} else if (type == REG_BINARY) {
					std::vector<uint8_t> bin(reinterpret_cast<uint8_t*>(data), reinterpret_cast<uint8_t*>(data + dataLength));
					entries.push_back({ valueName, EntryType::BINARY, nullptr, &bin, nullptr });
				} else if (type == REG_SZ) {
#if defined(CS_GWR_USEUTF8STRINGS)
					CS_GWR_STRING str(reinterpret_cast<char*>(data), dataLength / sizeof(char));
#else
					CS_GWR_STRING str(reinterpret_cast<wchar_t*>(data), dataLength / sizeof(wchar_t));
#endif
					entries.push_back({ valueName, EntryType::STRING, nullptr, nullptr, &str });
				} else {
					return CS_GWR_R_INVALIDTYPE;
				}
			}

			return CS_GWR_R_SUCCESS;
		};
		CS_GWR_R DeepLoad() {
			CS_GWR_R r = Load();
			if (r != CS_GWR_R_SUCCESS) return r;
			for (auto& key : childKeys) {
				CS_GWR_R r = key.DeepLoad();
				if (r != CS_GWR_R_SUCCESS) return r;
			}
			return CS_GWR_R_SUCCESS;
		};
		CS_GWR_R Submit() {
			std::vector<CS_GWR_STRING> diffChildKeys{};
			std::vector<CS_GWR_STRING> diffEntries{};

			if (dirty) {
#if defined(CS_GWR_USEUTF8STRINGS)
				char subKeyName[256];
#else
				wchar_t subKeyName[256];
#endif
				DWORD subKeyLength;
				DWORD i = 0;
				while (true) {
					subKeyLength = sizeof(subKeyName);
#if defined(CS_GWR_USEUTF8STRINGS)
					HRESULT r = RegEnumKeyExA(key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#else
					HRESULT r = RegEnumKeyExW(key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#endif
					if (r == ERROR_NO_MORE_ITEMS) break;
					if (r != ERROR_SUCCESS) {
						return CS_GWR_R_INVALID;
					}
					diffChildKeys.push_back({ subKeyName });
				}
				
#if defined(CS_GWR_USEUTF8STRINGS)
				char valueName[256];
#else
				wchar_t valueName[256];
#endif
				BYTE data[1024];
				DWORD valueLength;
				DWORD dataLength;
				DWORD type;
				DWORD i = 0;
				while (true) {
					valueLength = sizeof(valueName);
					dataLength = sizeof(data);
#if defined(CS_GWR_USEUTF8STRINGS)
					HRESULT r = RegEnumValueA(key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#else
					HRESULT r = RegEnumValueW(key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#endif
					if (r == ERROR_NO_MORE_ITEMS) break;
					if (r != ERROR_SUCCESS) {
						return CS_GWR_R_INVALID;
					}
					diffEntries.push_back(valueName);
				}
			}

			for (auto& entry : entries) {
				if (!dirty && !entry.IsDirty()) continue;
				
				static uint32_t dword;
				static std::vector<uint8_t> bin;
				static CS_GWR_STRING str;
				CS_GWR_R er = entry.GetData(&dword, &bin, &str);
				if (er != CS_GWR_R_SUCCESS) return er;

				EntryType type = entry.GetType();
				
				HRESULT r;
				switch (type) {
				case(EntryType::DWORD):
#if defined(CS_GWR_USEUTF8STRINGS)
					r = RegSetValueExA(key, entry.GetName().c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(dword), sizeof(uint32_t));
#else
					r = RegSetValueExW(key, entry.GetName().c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(dword), sizeof(uint32_t));
#endif
					break;
				case(EntryType::BINARY):
#if defined(CS_GWR_USEUTF8STRINGS)
					r = RegSetValueExA(key, entry.GetName().c_str(), 0, REG_SZ, bin.data(), static_cast<DWORD>(bin.size()));
#else
					r = RegSetValueExW(key, entry.GetName().c_str(), 0, REG_SZ, bin.data(), static_cast<DWORD>(bin.size()));
#endif
					break;
				case(EntryType::STRING):
#if defined(CS_GWR_USEUTF8STRINGS)
					r = RegSetValueExA(key, entry.GetName().c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(str.c_str()), static_cast<DWORD>((str.size() + 1) * sizeof(char)));
#else
					r = RegSetValueExW(key, entry.GetName().c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(str.c_str()), static_cast<DWORD>((str.size() + 1) * sizeof(wchar_t)));
#endif
					break;
				}
				if (r != ERROR_SUCCESS) return CS_GWR_R_INVALID;

				auto it = std::find(diffEntries.begin(), diffEntries.end(), entry.GetName());
				if (it != diffEntries.end()) {
					diffEntries.erase(it);
				}
			}

			for (auto& childKey : childKeys) {
				auto it = std::find(diffChildKeys.begin(), diffChildKeys.end(), childKey.GetPath());
				if (it != diffChildKeys.end()) {
					diffChildKeys.erase(it);
				}
			}

			for (auto& diff : diffEntries) {
#if defined(CS_GWR_USEUTF8STRINGS)
				HRESULT r = RegDeleteValueA(key, diff.c_str());
#else
				HRESULT r = RegDeleteValueW(key, diff.c_str());
#endif
				if (r != ERROR_SUCCESS) return CS_GWR_R_INVALID;
			}

			for (auto& diff : diffChildKeys) {
#if defined(CS_GWR_USEUTF8STRINGS)
				HRESULT r = RegDeleteTreeA(key, diff.c_str());
#else
				HRESULT r = RegDeleteTreeW(key, diff.c_str());
#endif
				if (r != ERROR_SUCCESS) return CS_GWR_R_INVALID;
			}

			return CS_GWR_R_SUCCESS;
		};
		CS_GWR_R DeepSubmit() {
			CS_GWR_R r = Submit();
			if (r != CS_GWR_R_SUCCESS) return r;
			for (auto& key : childKeys) {
				CS_GWR_R r = key.DeepSubmit();
				if (r != CS_GWR_R_SUCCESS) return r;
			}
			return CS_GWR_R_SUCCESS;
		}
		CS_GWR_R EraseTree() {
#if defined(CS_GWR_USEUTF8STRINGS)
			HRESULT r = RegDeleteTreeA(parent, path.c_str());
#else
			HRESULT r = RegDeleteTreeW(parent, path.c_str());
#endif
			if (r != ERROR_SUCCESS) return CS_GWR_R_INVALID;
			return CS_GWR_R_SUCCESS;
		};
		
		void ForceDirty() { dirty = true; };
		bool IsDirty() const { return dirty; };

		const CS_GWR_STRING& GetPath() const { return path; };
		
		void Clear() {
			childKeys.clear();
			entries.clear();
			dirty = true;
		};

		Key& AddChildKey(const CS_GWR_STRING& name) {
			for (size_t i = 0; i < childKeys.size(); i++) {
				if (childKeys[i].GetPath() == name) {
					return childKeys[i];
				}
			}
			childKeys.push_back({ name, key });
			dirty = true;
			return childKeys.back();
		};
		CS_GWR_R AddEntry(Entry& entry) {
			entry.ForceDirty();
			bool found = false;
			for (size_t i = 0; i < childKeys.size(); i++) {
				if (entries[i].GetName() == entry.GetName()) {
					found = true;
					entries[i] = entry;
					break;
				}
			}
			if (!found) {
				entries.push_back(entry);
				dirty = true;
			}
			return CS_GWR_R_SUCCESS;
		};
		CS_GWR_R RemoveChildKey(const CS_GWR_STRING& keyName) {
			for (size_t i = 0; i < childKeys.size(); i++) {
				if (childKeys[i].GetPath() == keyName) {
					childKeys.erase(childKeys.begin() + i);
					dirty = true;
					return CS_GWR_R_SUCCESS;
				}
			}
			return CS_GWR_R_KEYNOTFOUND;
		};
		CS_GWR_R RemoveEntry(const CS_GWR_STRING& entryName) {
			for (size_t i = 0; i < entries.size(); i++) {
				if (entries[i].GetName() == entryName) {
					entries.erase(entries.begin() + i);
					dirty = true;
					return CS_GWR_R_SUCCESS;
				}
			}
			return CS_GWR_R_ENTRYNOTFOUND;
		};
		
		const std::vector<Key>& GetChildKeys() const { return childKeys; };
		const std::vector<Entry>& GetEntries() const { return entries; };
	private:
		Key(const CS_GWR_STRING& _path, HKEY _parent = HKEY_CLASSES_ROOT) : path{ _path }, parent{ _parent } {
			InitializeKey();
		};

		void InitializeKey() {
#if defined(CS_GWR_USEUTF8STRINGS)
			const size_t pos = path.find("\\\\");
#else
			const size_t pos = path.find(L"\\\\");
#endif
			if (pos != std::string::npos) {
				creationResult = CS_GWR_R_INVALID;
			}

			DWORD disposition;
#if defined(CS_GWR_USEUTF8STRINGS)
			HRESULT r = RegCreateKeyExA(parent, path.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &key, &disposition);
#else
			HRESULT r = RegCreateKeyExW(parent, path.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &key, &disposition);
#endif
			if (disposition == REG_OPENED_EXISTING_KEY) {
				creationResult = CS_GWR_R_KEYALREADYEXISTS | CS_GWR_R_SUCCESS;
				return;
			}
			if (r != ERROR_SUCCESS) creationResult = CS_GWR_R_INVALID;
			else creationResult = CS_GWR_R_SUCCESS;
		}

		HKEY parent;
		HKEY key;
		CS_GWR_STRING path;
		std::vector<Key> childKeys;
		std::vector<Entry> entries;
		CS_GWR_R creationResult;
		bool dirty = false;
		bool owner = true;
	};
}
#endif