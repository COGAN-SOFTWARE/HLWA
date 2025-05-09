#pragma once

#if defined(CS_HLWA_E_REGISTRY) || defined(CS_HLWA_E_ALL)

#include "types.hpp"

#include <variant>

#if defined(_WIN32)
typedef struct HKEY__ *HKEY; // Forward declaration.

namespace CoganSoftware::HLWA::Registry {
	enum struct EntryType {
		DWORD,  // REG_DWORD
		BINARY, // REG_BINARY
		STRING  // REG_SZ
	};
	struct Entry {
	public:
		Entry(const CS_HLWA_STRING& p_name, EntryType p_type, uint32_t* p_inDword, std::vector<uint8_t>* p_inBinary, CS_HLWA_STRING* p_inString);
		~Entry() {};
		
		CS_HLWA_R GetCreationResult() const;

		void ForceDirty();
		bool IsDirty() const;

		const CS_HLWA_STRING& GetName() const;
		void SetName(const CS_HLWA_STRING& p_name);

		EntryType GetType() const;
		CS_HLWA_R SetType(EntryType p_type); // Clears data.
		CS_HLWA_R GetData(uint32_t* p_outDword, std::vector<uint8_t>* p_outBinary, CS_HLWA_STRING* p_outString) const;
		CS_HLWA_R SetData(uint32_t* p_inDword, std::vector<uint8_t>* p_inBinary, CS_HLWA_STRING* p_inString);
	private:
		CS_HLWA_STRING m_name;
		CS_HLWA_R m_creationResult;
		EntryType m_type;
		std::variant<uint32_t, std::vector<uint8_t>, CS_HLWA_STRING> m_data{};
		bool m_dirty = false;
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
		Key(const CS_HLWA_STRING& p_path, RootKey p_root = RootKey::ClassesRoot);
		Key(const Key& p_other);
		Key(Key&& p_other) noexcept;
		~Key();
		
		CS_HLWA_R GetCreationResult() const;

		CS_HLWA_R Load();
		CS_HLWA_R DeepLoad();
		CS_HLWA_R Submit();
		CS_HLWA_R DeepSubmit();
		CS_HLWA_R EraseTree();
		
		void ForceDirty();
		bool IsDirty() const;

		const CS_HLWA_STRING& GetPath() const;
		
		void Clear();

		size_t AddChildKey(const CS_HLWA_STRING& p_name);
		size_t AddEntry(const Entry& p_entry);
		CS_HLWA_R RemoveChildKey(const CS_HLWA_STRING& p_keyName);
		CS_HLWA_R RemoveEntry(const CS_HLWA_STRING& p_entryName);
		
		std::vector<Key>& GetChildKeys();
		std::vector<Entry>& GetEntries();
		const std::vector<Key>& GetChildKeys() const;
		const std::vector<Entry>& GetEntries() const;

		Key& operator=(const Key& p_other) {
			if (this != &p_other) {
				m_parent = p_other.m_parent;
				m_key = p_other.m_key;
				m_path = p_other.m_path;
				m_childKeys = p_other.m_childKeys;
				m_entries = p_other.m_entries;
				m_creationResult = p_other.m_creationResult;
				m_dirty = p_other.m_dirty;

				m_parent->m_users++;
				m_key->m_users++;
			}
			return *this;
		}
		Key& operator=(Key&& p_other) noexcept {
			if (this != &p_other) {
				m_parent = p_other.m_parent;
				m_key = p_other.m_key;
				m_path = p_other.m_path;
				m_childKeys = p_other.m_childKeys;
				m_entries = p_other.m_entries;
				m_creationResult = p_other.m_creationResult;
				m_dirty = p_other.m_dirty;

				m_parent->m_users++;
				m_key->m_users++;
			}
			return *this;
		}
	private:
		struct KeyUser {
			KeyUser();
			~KeyUser();

			HKEY m_key = nullptr;
			int m_users;
		};

		Key(const CS_HLWA_STRING& p_path, KeyUser* p_parent);

		void InitializeKey();

		KeyUser* m_parent = nullptr;
		KeyUser* m_key = nullptr;
		CS_HLWA_STRING m_path;
		std::vector<Key> m_childKeys;
		std::vector<Entry> m_entries;
		CS_HLWA_R m_creationResult;
		bool m_dirty = false;
	};
}
#endif
#endif