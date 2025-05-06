#include "../../include/HLWA/ext_registry.hpp"

#if defined(CS_HLWA_E_REGISTRY) || defined(CS_HLWA_E_ALL)
#if defined(_WIN32)
#include <Windows.h>

namespace CoganSoftware::HLWA::Registry {
	Entry::Entry(const CS_HLWA_STRING& p_name, EntryType p_type, uint32_t* p_inDword, std::vector<uint8_t>* p_inBinary, CS_HLWA_STRING* p_inString) : m_name{ p_name }, m_type{ p_type } {
		m_creationResult = SetData(p_inDword, p_inBinary, p_inString);
	}
	
	CS_HLWA_R Entry::GetCreationResult() const {
		return m_creationResult;
	}

	void Entry::ForceDirty() {
		m_dirty = true;
	}
	bool Entry::IsDirty() const {
		return m_dirty;
	}

	const CS_HLWA_STRING& Entry::GetName() const {
		return m_name;
	}
	void Entry::SetName(const CS_HLWA_STRING& p_name) {
		m_name = p_name;
		m_dirty = true;
	}
	
	EntryType Entry::GetType() { return m_type; };
	CS_HLWA_R Entry::SetType(EntryType p_type) {
		m_type = p_type;
		static uint32_t defDword = 0;
		static std::vector<uint8_t> defBinary{};
		static CS_HLWA_STRING defString = CS_HLWA_DEFAULTENTRY;
		CS_HLWA_R r = SetData(&defDword, &defBinary, &defString);
		m_dirty = true;
		return r;
	};
	CS_HLWA_R Entry::GetData(uint32_t* p_outDword, std::vector<uint8_t>* p_outBinary, CS_HLWA_STRING* p_outString) {
		switch (m_type) {
		case(EntryType::DWORD):
			if (p_outDword == nullptr) return CS_HLWA_R_INVALIDARGS;
			*p_outDword = std::get<uint32_t>(m_data);
			break;
		case(EntryType::BINARY):
			if (p_outBinary == nullptr) return CS_HLWA_R_INVALIDARGS;
			*p_outBinary = std::get<std::vector<uint8_t>>(m_data);
			break;
		case(EntryType::STRING):
			if (p_outString == nullptr) return CS_HLWA_R_INVALIDARGS;
			*p_outString = std::get<CS_HLWA_STRING>(m_data);
			break;
		}
		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Entry::SetData(uint32_t* p_inDword, std::vector<uint8_t>* p_inBinary, CS_HLWA_STRING* p_inString) {
		switch (m_type) {
		case(EntryType::DWORD):
			if (p_inDword == nullptr) return CS_HLWA_R_INVALIDARGS;
			m_data = *p_inDword;
			break;
		case(EntryType::BINARY):
			if (p_inBinary == nullptr) return CS_HLWA_R_INVALIDARGS;
			m_data = *p_inBinary;
			break;
		case(EntryType::STRING):
			if (p_inString == nullptr) return CS_HLWA_R_INVALIDARGS;
			m_data = *p_inString;
			break;
		}
		m_dirty = true;
		return CS_HLWA_R_SUCCESS;
	}


	Key::KeyUser::KeyUser() : m_users{ 0 } {}
	Key::KeyUser::~KeyUser() {
		if (m_key) RegCloseKey(m_key);
	}


	Key::Key(const CS_HLWA_STRING& p_path, RootKey p_root) : m_path{ p_path } {
		m_parent = new KeyUser{};

		switch (p_root) {
		case(RootKey::ClassesRoot):
			m_parent->m_key = HKEY_CLASSES_ROOT;
			break;
		case(RootKey::CurrentConfig):
			m_parent->m_key = HKEY_CURRENT_CONFIG;
			break;
		case(RootKey::CurrentUser):
			m_parent->m_key = HKEY_CURRENT_USER;
			break;
		case(RootKey::CurrentUserLocalSettings):
			m_parent->m_key = HKEY_CURRENT_USER_LOCAL_SETTINGS;
			break;
		case(RootKey::LocalMachine):
			m_parent->m_key = HKEY_LOCAL_MACHINE;
			break;
		case(RootKey::Users):
			m_parent->m_key = HKEY_USERS;
			break;
		}

		InitializeKey();
	};
	Key::Key(const Key& p_other) : m_parent{ p_other.m_parent }, m_key{ p_other.m_key }, m_path{ p_other.m_path }, m_childKeys{ p_other.m_childKeys }, m_entries{ p_other.m_entries }, m_creationResult{ p_other.m_creationResult }, m_dirty{ p_other.m_dirty } {
		if (m_parent == nullptr) {
			m_creationResult = CS_HLWA_R_INVALID;
			return;
		}
		if (m_key == nullptr) {
			m_creationResult = CS_HLWA_R_INVALID;
			return;
		}

		m_parent->m_users++;
		m_key->m_users++;
	}
	Key::Key(Key&& p_other) noexcept : m_parent{ p_other.m_parent }, m_key{ p_other.m_key }, m_path{ p_other.m_path }, m_childKeys{ p_other.m_childKeys }, m_entries{ p_other.m_entries }, m_creationResult{ p_other.m_creationResult }, m_dirty{ p_other.m_dirty } {
		if (m_parent == nullptr) {
			m_creationResult = CS_HLWA_R_INVALID;
			return;
		}
		if (m_key == nullptr) {
			m_creationResult = CS_HLWA_R_INVALID;
			return;
		}

		m_parent->m_users++;
		m_key->m_users++;
	}
	Key::~Key() {
		if (m_parent != nullptr) {
			m_parent->m_users--;
			if (m_parent->m_users <= 0) delete m_parent;
		}
		if (m_key == nullptr) {
			m_key->m_users--;
			if (m_key->m_users <= 0) delete m_key;
		}
	}
		
	CS_HLWA_R Key::GetCreationResult() const {
		return m_creationResult;
	}

	CS_HLWA_R Key::Load() {
		if (m_parent == nullptr) return CS_HLWA_R_NOTFOUND;
		if (m_parent->m_key == nullptr) return CS_HLWA_R_NOTFOUND;
		if (m_key == nullptr) return CS_HLWA_R_NOTFOUND;
		if (m_key->m_key == nullptr) return CS_HLWA_R_NOTFOUND;

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
			HRESULT r = RegEnumKeyExA(m_key->m_key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#else
			HRESULT r = RegEnumKeyExW(m_key->m_key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#endif
			if (r == ERROR_NO_MORE_ITEMS) break;
			if (r != ERROR_SUCCESS) {
				return CS_HLWA_R_INVALID;
			}
			m_childKeys.push_back({ subKeyName, m_key });
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
			HRESULT r = RegEnumValueA(m_key->m_key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#else
			HRESULT r = RegEnumValueW(m_key->m_key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#endif
			if (r == ERROR_NO_MORE_ITEMS) break;
			if (r != ERROR_SUCCESS) {
				return CS_HLWA_R_INVALID;
			}

			if (type == REG_DWORD && dataLength >= sizeof(DWORD)) {
				uint32_t dw = *(DWORD*)data;
				m_entries.push_back({ valueName, EntryType::DWORD, &dw, nullptr, nullptr });
			} else if (type == REG_BINARY) {
				std::vector<uint8_t> bin(reinterpret_cast<uint8_t*>(data), reinterpret_cast<uint8_t*>(data + dataLength));
				m_entries.push_back({ valueName, EntryType::BINARY, nullptr, &bin, nullptr });
			} else if (type == REG_SZ) {
#if defined(CS_HLWA_USEUTF8STRINGS)
				CS_HLWA_STRING str(reinterpret_cast<char*>(data), dataLength / sizeof(char));
#else
				CS_HLWA_STRING str(reinterpret_cast<wchar_t*>(data), dataLength / sizeof(wchar_t));
#endif
				m_entries.push_back({ valueName, EntryType::STRING, nullptr, nullptr, &str });
			} else {
				return CS_HLWA_R_INVALIDTYPE;
			}
		}

		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::DeepLoad() {
		CS_HLWA_R r = Load();
		if (r != CS_HLWA_R_SUCCESS) return r;
		for (auto& key : m_childKeys) {
			CS_HLWA_R r = key.DeepLoad();
			if (r != CS_HLWA_R_SUCCESS) return r;
		}
		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::Submit() {
		if (m_parent == nullptr) return CS_HLWA_R_NOTFOUND;
		if (m_parent->m_key == nullptr) return CS_HLWA_R_NOTFOUND;
		if (m_key == nullptr) return CS_HLWA_R_NOTFOUND;
		if (m_key->m_key == nullptr) return CS_HLWA_R_NOTFOUND;

		std::vector<CS_HLWA_STRING> diffChildKeys{};
		std::vector<CS_HLWA_STRING> diffEntries{};

		if (m_dirty) {
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
				HRESULT r = RegEnumKeyExA(m_key->m_key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
#else
				HRESULT r = RegEnumKeyExW(m_key->m_key, i++, subKeyName, &subKeyLength, nullptr, nullptr, nullptr, nullptr);
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
				HRESULT r = RegEnumValueA(m_key->m_key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#else
				HRESULT r = RegEnumValueW(m_key->m_key, i++, valueName, &valueLength, NULL, &type, data, &dataLength);
#endif
				if (r == ERROR_NO_MORE_ITEMS) break;
				if (r != ERROR_SUCCESS) {
					return CS_HLWA_R_INVALID;
				}
				diffEntries.push_back(valueName);
			}
		}

		for (auto& entry : m_entries) {
			if (!m_dirty && !entry.IsDirty()) continue;
				
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
				r = RegSetValueExA(m_key->m_key, entry.GetName().c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(dword), sizeof(uint32_t));
#else
				r = RegSetValueExW(m_key->m_key, entry.GetName().c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(dword), sizeof(uint32_t));
#endif
				break;
			case(EntryType::BINARY):
#if defined(CS_HLWA_USEUTF8STRINGS)
				r = RegSetValueExA(m_key->vkey, entry.GetName().c_str(), 0, REG_SZ, bin.data(), static_cast<DWORD>(bin.size()));
#else
				r = RegSetValueExW(m_key->m_key, entry.GetName().c_str(), 0, REG_SZ, bin.data(), static_cast<DWORD>(bin.size()));
#endif
				break;
			case(EntryType::STRING):
#if defined(CS_HLWA_USEUTF8STRINGS)
				r = RegSetValueExA(m_key->m_key, entry.GetName().c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(str.c_str()), static_cast<DWORD>((str.size() + 1) * sizeof(char)));
#else
				r = RegSetValueExW(m_key->m_key, entry.GetName().c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(str.c_str()), static_cast<DWORD>((str.size() + 1) * sizeof(wchar_t)));
#endif
				break;
			}
			if (r != ERROR_SUCCESS) return CS_HLWA_R_INVALID;

			auto it = std::find(diffEntries.begin(), diffEntries.end(), entry.GetName());
			if (it != diffEntries.end()) {
				diffEntries.erase(it);
			}
		}

		for (auto& childKey : m_childKeys) {
			auto it = std::find(diffChildKeys.begin(), diffChildKeys.end(), childKey.GetPath());
			if (it != diffChildKeys.end()) {
				diffChildKeys.erase(it);
			}
		}

		for (auto& diff : diffEntries) {
#if defined(CS_HLWA_USEUTF8STRINGS)
			HRESULT r = RegDeleteValueA(m_key->m_key, diff.c_str());
#else
			HRESULT r = RegDeleteValueW(m_key->m_key, diff.c_str());
#endif
			if (r != ERROR_SUCCESS) return CS_HLWA_R_INVALID;
		}

		for (auto& diff : diffChildKeys) {
#if defined(CS_HLWA_USEUTF8STRINGS)
			HRESULT r = RegDeleteTreeA(m_key->m_key, diff.c_str());
#else
			HRESULT r = RegDeleteTreeW(m_key->m_key, diff.c_str());
#endif
			if (r != ERROR_SUCCESS) return CS_HLWA_R_INVALID;
		}

		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::DeepSubmit() {
		CS_HLWA_R r = Submit();
		if (r != CS_HLWA_R_SUCCESS) return r;
		for (auto& key : m_childKeys) {
			CS_HLWA_R r = key.DeepSubmit();
			if (r != CS_HLWA_R_SUCCESS) return r;
		}
		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Key::EraseTree() {
		if (m_parent == nullptr) return CS_HLWA_R_NOTFOUND;
		if (m_parent->m_key == nullptr) return CS_HLWA_R_NOTFOUND;

#if defined(CS_HLWA_USEUTF8STRINGS)
		HRESULT r = RegDeleteTreeA(m_parent->m_key, path.c_str());
#else
		HRESULT r = RegDeleteTreeW(m_parent->m_key, m_path.c_str());
#endif
		if (r != ERROR_SUCCESS) return CS_HLWA_R_INVALID;
		return CS_HLWA_R_SUCCESS;
	}
		
	void Key::ForceDirty() {
		m_dirty = true;
	}
	bool Key::IsDirty() const {
		return m_dirty;
	}

	const CS_HLWA_STRING& Key::GetPath() const {
		return m_path;
	}
		
	void Key::Clear() {
		m_childKeys.clear();
		m_entries.clear();
		m_dirty = true;
	}

	size_t Key::AddChildKey(const CS_HLWA_STRING& p_name) {
		for (size_t i = 0; i < m_childKeys.size(); i++) {
			if (m_childKeys[i].GetPath() == p_name) {
				return i;
			}
		}
		m_childKeys.push_back({ p_name, m_key });
		m_dirty = true;
		return m_childKeys.size() - 1;
	}
	size_t Key::AddEntry(Entry& p_entry) {
		p_entry.ForceDirty();
		for (size_t i = 0; i < m_childKeys.size(); i++) {
			if (m_entries[i].GetName() == p_entry.GetName()) {
				m_entries[i] = p_entry;
				return i;
			}
		}
		m_entries.push_back(p_entry);
		m_dirty = true;
		return m_entries.size() - 1;
	}
	CS_HLWA_R Key::RemoveChildKey(const CS_HLWA_STRING& p_keyName) {
		for (size_t i = 0; i < m_childKeys.size(); i++) {
			if (m_childKeys[i].GetPath() == p_keyName) {
				m_childKeys.erase(m_childKeys.begin() + i);
				m_dirty = true;
				return CS_HLWA_R_SUCCESS;
			}
		}
		return CS_HLWA_R_NOTFOUND;
	}
	CS_HLWA_R Key::RemoveEntry(const CS_HLWA_STRING& p_entryName) {
		for (size_t i = 0; i < m_entries.size(); i++) {
			if (m_entries[i].GetName() == p_entryName) {
				m_entries.erase(m_entries.begin() + i);
				m_dirty = true;
				return CS_HLWA_R_SUCCESS;
			}
		}
		return CS_HLWA_R_NOTFOUND;
	}
		
	const std::vector<Key>& Key::GetChildKeys() const {
		return m_childKeys;
	}
	const std::vector<Entry>& Key::GetEntries() const {
		return m_entries;
	}

	Key::Key(const CS_HLWA_STRING& p_path, KeyUser* p_parent) : m_parent{ p_parent }, m_path{ p_path } {
		if (m_parent == nullptr) {
			m_creationResult = CS_HLWA_R_INVALID;
			return;
		}

		InitializeKey();
	}

	void Key::InitializeKey() {
#if defined(CS_HLWA_USEUTF8STRINGS)
		const size_t pos = m_path.find("\\\\");
#else
		const size_t pos = m_path.find(L"\\\\");
#endif
		if (pos != std::string::npos) {
			m_creationResult = CS_HLWA_R_INVALID;
		}

		m_key = new KeyUser{};

		m_parent->m_users++;
		m_key->m_users++;

		DWORD disposition;
#if defined(CS_HLWA_USEUTF8STRINGS)
		HRESULT r = RegCreateKeyExA(m_parent->m_key, m_path.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &m_key->m_key, &disposition);
#else
		HRESULT r = RegCreateKeyExW(m_parent->m_key, m_path.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &m_key->m_key, &disposition);
#endif
		if (disposition == REG_OPENED_EXISTING_KEY) {
			m_creationResult = CS_HLWA_R_ALREADYEXISTS | CS_HLWA_R_SUCCESS;
			return;
		}
		if (r != ERROR_SUCCESS) m_creationResult = CS_HLWA_R_INVALID;
		else m_creationResult = CS_HLWA_R_SUCCESS;
	}
}
#endif
#endif