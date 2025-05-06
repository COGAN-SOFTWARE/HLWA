#include "../../include/HLWA/ext_registry.hpp"

#if defined(CS_HLWA_E_REGISTRY)
#if defined(_WIN32)
#include <Windows.h>

namespace CoganSoftware::HLWA::Registry {
	Entry::Entry(const CS_HLWA_STRING& _name, EntryType _type, uint32_t* inDword, std::vector<uint8_t>* inBinary, CS_HLWA_STRING* inString) : name{ _name }, type{ type } {
		creationResult = SetData(inDword, inBinary, inString);
	}
	
	CS_HLWA_R Entry::GetCreationResult() const {
		return creationResult;
	}

	void Entry::ForceDirty() {
		dirty = true;
	}
	bool Entry::IsDirty() const {
		return dirty;
	}

	const CS_HLWA_STRING& Entry::GetName() const {
		return name;
	}
	void Entry::SetName(const CS_HLWA_STRING& _name) {
		name = _name;
		dirty = true;
	}
	
	EntryType Entry::GetType() { return type; };
	CS_HLWA_R Entry::SetType(EntryType _type) {
		type = _type;
		static uint32_t defDword = 0;
		static std::vector<uint8_t> defBinary{};
		static CS_HLWA_STRING defString = CS_HLWA_DEFAULTENTRY;
		CS_HLWA_R r = SetData(&defDword, &defBinary, &defString);
		dirty = true;
		return r;
	};
	CS_HLWA_R Entry::GetData(uint32_t* outDword, std::vector<uint8_t>* outBinary, CS_HLWA_STRING* outString) {
		switch (type) {
		case(EntryType::DWORD):
			if (outDword == nullptr) return CS_HLWA_R_INVALIDARGS;
			*outDword = std::get<uint32_t>(data);
			break;
		case(EntryType::BINARY):
			if (outBinary == nullptr) return CS_HLWA_R_INVALIDARGS;
			*outBinary = std::get<std::vector<uint8_t>>(data);
			break;
		case(EntryType::STRING):
			if (outString == nullptr) return CS_HLWA_R_INVALIDARGS;
			*outString = std::get<CS_HLWA_STRING>(data);
			break;
		}
		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Entry::SetData(uint32_t* inDword, std::vector<uint8_t>* inBinary, CS_HLWA_STRING* inString) {
		switch (type) {
		case(EntryType::DWORD):
			if (inDword == nullptr) return CS_HLWA_R_INVALIDARGS;
			data = *inDword;
			break;
		case(EntryType::BINARY):
			if (inBinary == nullptr) return CS_HLWA_R_INVALIDARGS;
			data = *inBinary;
			break;
		case(EntryType::STRING):
			if (inString == nullptr) return CS_HLWA_R_INVALIDARGS;
			data = *inString;
			break;
		}
		dirty = true;
		return CS_HLWA_R_SUCCESS;
	}


	Key::KeyUser::KeyUser() : users{ 0 } {}
	Key::KeyUser::~KeyUser() {
		if (key) RegCloseKey(key);
	}


	Key::Key(const CS_HLWA_STRING& _path, RootKey root) : path{ _path } {
		parent = new KeyUser{};

		switch (root) {
		case(RootKey::ClassesRoot):
			parent->key = HKEY_CLASSES_ROOT;
			break;
		case(RootKey::CurrentConfig):
			parent->key = HKEY_CURRENT_CONFIG;
			break;
		case(RootKey::CurrentUser):
			parent->key = HKEY_CURRENT_USER;
			break;
		case(RootKey::CurrentUserLocalSettings):
			parent->key = HKEY_CURRENT_USER_LOCAL_SETTINGS;
			break;
		case(RootKey::LocalMachine):
			parent->key = HKEY_LOCAL_MACHINE;
			break;
		case(RootKey::Users):
			parent->key = HKEY_USERS;
			break;
		}

		InitializeKey();
	};
	Key::Key(const Key& other) : parent{ other.parent }, key{ other.key }, path{ other.path }, childKeys{ other.childKeys }, entries{ other.entries }, creationResult{ other.creationResult }, dirty{ other.dirty } {
		if (parent == nullptr) {
			creationResult = CS_HLWA_R_INVALID;
			return;
		}
		if (key == nullptr) {
			creationResult = CS_HLWA_R_INVALID;
			return;
		}

		parent->users++;
		key->users++;
	}
	Key::Key(Key&& other) noexcept : parent{ other.parent }, key{ other.key }, path{ other.path }, childKeys{ other.childKeys }, entries{ other.entries }, creationResult{ other.creationResult }, dirty{ other.dirty } {
		if (parent == nullptr) {
			creationResult = CS_HLWA_R_INVALID;
			return;
		}
		if (key == nullptr) {
			creationResult = CS_HLWA_R_INVALID;
			return;
		}

		parent->users++;
		key->users++;
	}
	Key::~Key() {
		if (parent != nullptr) {
			parent->users--;
			if (parent->users <= 0) delete parent;
		}
		if (key == nullptr) {
			key->users--;
			if (key->users <= 0) delete key;
		}
	}
		
	CS_HLWA_R Key::GetCreationResult() const {
		return creationResult;
	}

	CS_HLWA_R Key::Load() {
		if (parent == nullptr) return CS_HLWA_R_NOTFOUND;
		if (parent->key == nullptr) return CS_HLWA_R_NOTFOUND;
		if (key == nullptr) return CS_HLWA_R_NOTFOUND;
		if (key->key == nullptr) return CS_HLWA_R_NOTFOUND;

#if defined(CS_HLWA_USEUTF8STRINGS)
		char subKeyName[256];
#else
		wchar_t subKeyName[256];
#endif
		DWORD subKeyLength;
		DWORD i = 0;
		while (true) {
			subKeyLength = sizeof(subKeyName);
#if defined(CS_HLWA_USEUTF8STRINGS)
			HRESULT r = RegEnumKeyExA(key->key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#else
			HRESULT r = RegEnumKeyExW(key->key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#endif
			if (r == ERROR_NO_MORE_ITEMS) break;
			if (r != ERROR_SUCCESS) {
				return CS_HLWA_R_INVALID;
			}
			childKeys.push_back({ subKeyName, key });
		}
			
#if defined(CS_HLWA_USEUTF8STRINGS)
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
#if defined(CS_HLWA_USEUTF8STRINGS)
			HRESULT r = RegEnumValueA(key->key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#else
			HRESULT r = RegEnumValueW(key->key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#endif
			if (r == ERROR_NO_MORE_ITEMS) break;
			if (r != ERROR_SUCCESS) {
				return CS_HLWA_R_INVALID;
			}

			if (type == REG_DWORD && dataLength >= sizeof(DWORD)) {
				uint32_t dw = *(DWORD*)data;
				entries.push_back({ valueName, EntryType::DWORD, &dw, nullptr, nullptr });
			} else if (type == REG_BINARY) {
				std::vector<uint8_t> bin(reinterpret_cast<uint8_t*>(data), reinterpret_cast<uint8_t*>(data + dataLength));
				entries.push_back({ valueName, EntryType::BINARY, nullptr, &bin, nullptr });
			} else if (type == REG_SZ) {
#if defined(CS_HLWA_USEUTF8STRINGS)
				CS_HLWA_STRING str(reinterpret_cast<char*>(data), dataLength / sizeof(char));
#else
				CS_HLWA_STRING str(reinterpret_cast<wchar_t*>(data), dataLength / sizeof(wchar_t));
#endif
				entries.push_back({ valueName, EntryType::STRING, nullptr, nullptr, &str });
			} else {
				return CS_HLWA_R_INVALIDTYPE;
			}
		}

		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::DeepLoad() {
		CS_HLWA_R r = Load();
		if (r != CS_HLWA_R_SUCCESS) return r;
		for (auto& key : childKeys) {
			CS_HLWA_R r = key.DeepLoad();
			if (r != CS_HLWA_R_SUCCESS) return r;
		}
		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::Submit() {
		if (parent == nullptr) return CS_HLWA_R_NOTFOUND;
		if (parent->key == nullptr) return CS_HLWA_R_NOTFOUND;
		if (key == nullptr) return CS_HLWA_R_NOTFOUND;
		if (key->key == nullptr) return CS_HLWA_R_NOTFOUND;

		std::vector<CS_HLWA_STRING> diffChildKeys{};
		std::vector<CS_HLWA_STRING> diffEntries{};

		if (dirty) {
#if defined(CS_HLWA_USEUTF8STRINGS)
			char subKeyName[256];
#else
			wchar_t subKeyName[256];
#endif
			DWORD subKeyLength;
			DWORD i = 0;
			while (true) {
				subKeyLength = sizeof(subKeyName);
#if defined(CS_HLWA_USEUTF8STRINGS)
				HRESULT r = RegEnumKeyExA(key->key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#else
				HRESULT r = RegEnumKeyExW(key->key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#endif
				if (r == ERROR_NO_MORE_ITEMS) break;
				if (r != ERROR_SUCCESS) {
					return CS_HLWA_R_INVALID;
				}
				diffChildKeys.push_back({ subKeyName });
			}
				
#if defined(CS_HLWA_USEUTF8STRINGS)
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
#if defined(CS_HLWA_USEUTF8STRINGS)
				HRESULT r = RegEnumValueA(key->key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#else
				HRESULT r = RegEnumValueW(key->key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#endif
				if (r == ERROR_NO_MORE_ITEMS) break;
				if (r != ERROR_SUCCESS) {
					return CS_HLWA_R_INVALID;
				}
				diffEntries.push_back(valueName);
			}
		}

		for (auto& entry : entries) {
			if (!dirty && !entry.IsDirty()) continue;
				
			static uint32_t dword;
			static std::vector<uint8_t> bin;
			static CS_HLWA_STRING str;
			CS_HLWA_R er = entry.GetData(&dword, &bin, &str);
			if (er != CS_HLWA_R_SUCCESS) return er;

			EntryType type = entry.GetType();
				
			HRESULT r;
			switch (type) {
			case(EntryType::DWORD):
#if defined(CS_HLWA_USEUTF8STRINGS)
				r = RegSetValueExA(key->key, entry.GetName().c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(dword), sizeof(uint32_t));
#else
				r = RegSetValueExW(key->key, entry.GetName().c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(dword), sizeof(uint32_t));
#endif
				break;
			case(EntryType::BINARY):
#if defined(CS_HLWA_USEUTF8STRINGS)
				r = RegSetValueExA(key->key, entry.GetName().c_str(), 0, REG_SZ, bin.data(), static_cast<DWORD>(bin.size()));
#else
				r = RegSetValueExW(key->key, entry.GetName().c_str(), 0, REG_SZ, bin.data(), static_cast<DWORD>(bin.size()));
#endif
				break;
			case(EntryType::STRING):
#if defined(CS_HLWA_USEUTF8STRINGS)
				r = RegSetValueExA(key->key, entry.GetName().c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(str.c_str()), static_cast<DWORD>((str.size() + 1) * sizeof(char)));
#else
				r = RegSetValueExW(key->key, entry.GetName().c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(str.c_str()), static_cast<DWORD>((str.size() + 1) * sizeof(wchar_t)));
#endif
				break;
			}
			if (r != ERROR_SUCCESS) return CS_HLWA_R_INVALID;

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
#if defined(CS_HLWA_USEUTF8STRINGS)
			HRESULT r = RegDeleteValueA(key->key, diff.c_str());
#else
			HRESULT r = RegDeleteValueW(key->key, diff.c_str());
#endif
			if (r != ERROR_SUCCESS) return CS_HLWA_R_INVALID;
		}

		for (auto& diff : diffChildKeys) {
#if defined(CS_HLWA_USEUTF8STRINGS)
			HRESULT r = RegDeleteTreeA(key->key, diff.c_str());
#else
			HRESULT r = RegDeleteTreeW(key->key, diff.c_str());
#endif
			if (r != ERROR_SUCCESS) return CS_HLWA_R_INVALID;
		}

		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::DeepSubmit() {
		CS_HLWA_R r = Submit();
		if (r != CS_HLWA_R_SUCCESS) return r;
		for (auto& key : childKeys) {
			CS_HLWA_R r = key.DeepSubmit();
			if (r != CS_HLWA_R_SUCCESS) return r;
		}
		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::EraseTree() {
		if (parent == nullptr) return CS_HLWA_R_NOTFOUND;
		if (parent->key == nullptr) return CS_HLWA_R_NOTFOUND;

#if defined(CS_HLWA_USEUTF8STRINGS)
		HRESULT r = RegDeleteTreeA(parent->key, path.c_str());
#else
		HRESULT r = RegDeleteTreeW(parent->key, path.c_str());
#endif
		if (r != ERROR_SUCCESS) return CS_HLWA_R_INVALID;
		return CS_HLWA_R_SUCCESS;
	}
		
	void Key::ForceDirty() {
		dirty = true;
	}
	bool Key::IsDirty() const {
		return dirty;
	}

	const CS_HLWA_STRING& Key::GetPath() const {
		return path;
	}
		
	void Key::Clear() {
		childKeys.clear();
		entries.clear();
		dirty = true;
	}

	Key& Key::AddChildKey(const CS_HLWA_STRING& name) {
		for (size_t i = 0; i < childKeys.size(); i++) {
			if (childKeys[i].GetPath() == name) {
				return childKeys[i];
			}
		}
		childKeys.push_back({ name, key });
		dirty = true;
		return childKeys.back();
	}
	CS_HLWA_R Key::AddEntry(Entry& entry) {
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
		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::RemoveChildKey(const CS_HLWA_STRING& keyName) {
		for (size_t i = 0; i < childKeys.size(); i++) {
			if (childKeys[i].GetPath() == keyName) {
				childKeys.erase(childKeys.begin() + i);
				dirty = true;
				return CS_HLWA_R_SUCCESS;
			}
		}
		return CS_HLWA_R_NOTFOUND;
	}
	CS_HLWA_R Key::RemoveEntry(const CS_HLWA_STRING& entryName) {
		for (size_t i = 0; i < entries.size(); i++) {
			if (entries[i].GetName() == entryName) {
				entries.erase(entries.begin() + i);
				dirty = true;
				return CS_HLWA_R_SUCCESS;
			}
		}
		return CS_HLWA_R_NOTFOUND;
	}
		
	const std::vector<Key>& Key::GetChildKeys() const {
		return childKeys;
	}
	const std::vector<Entry>& Key::GetEntries() const {
		return entries;
	}

	Key::Key(const CS_HLWA_STRING& _path, KeyUser* _parent) : parent{ _parent }, path{ _path } {
		if (parent == nullptr) {
			creationResult = CS_HLWA_R_INVALID;
			return;
		}

		InitializeKey();
	}

	void Key::InitializeKey() {
#if defined(CS_HLWA_USEUTF8STRINGS)
		const size_t pos = path.find("\\\\");
#else
		const size_t pos = path.find(L"\\\\");
#endif
		if (pos != std::string::npos) {
			creationResult = CS_HLWA_R_INVALID;
		}

		key = new KeyUser{};

		parent->users++;
		key->users++;

		DWORD disposition;
#if defined(CS_HLWA_USEUTF8STRINGS)
		HRESULT r = RegCreateKeyExA(parent->key, path.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &key->key, &disposition);
#else
		HRESULT r = RegCreateKeyExW(parent->key, path.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &key->key, &disposition);
#endif
		if (disposition == REG_OPENED_EXISTING_KEY) {
			creationResult = CS_HLWA_R_KEYALREADYEXISTS | CS_HLWA_R_SUCCESS;
			return;
		}
		if (r != ERROR_SUCCESS) creationResult = CS_HLWA_R_INVALID;
		else creationResult = CS_HLWA_R_SUCCESS;
	}
}
#endif
#endif