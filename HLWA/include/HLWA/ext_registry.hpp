#pragma once

#if defined(CS_HLWA_E_REGISTRY)

#include "types.hpp"

#include <string>
#include <variant>
#include <vector>

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
		Entry(const CS_HLWA_STRING& _name, EntryType _type, uint32_t* inDword, std::vector<uint8_t>* inBinary, CS_HLWA_STRING* inString);
		~Entry() {};
		
		CS_HLWA_R GetCreationResult() const;

		void ForceDirty();
		bool IsDirty() const;

		const CS_HLWA_STRING& GetName() const;
		void SetName(const CS_HLWA_STRING& _name);

		EntryType GetType();
		// Setting the type also clears the data.
		CS_HLWA_R SetType(EntryType _type);
		CS_HLWA_R GetData(uint32_t* outDword, std::vector<uint8_t>* outBinary, CS_HLWA_STRING* outString);
		CS_HLWA_R SetData(uint32_t* inDword, std::vector<uint8_t>* inBinary, CS_HLWA_STRING* inString);
	private:
		CS_HLWA_STRING name;
		CS_HLWA_R creationResult;
		EntryType type;
		std::variant<uint32_t, std::vector<uint8_t>, CS_HLWA_STRING> data{};
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
		Key(const CS_HLWA_STRING& _path, RootKey root);
		Key(const Key& other);
		Key(Key&& other) noexcept;
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

		Key& AddChildKey(const CS_HLWA_STRING& name);
		CS_HLWA_R AddEntry(Entry& entry);
		CS_HLWA_R RemoveChildKey(const CS_HLWA_STRING& keyName);
		CS_HLWA_R RemoveEntry(const CS_HLWA_STRING& entryName);
		
		const std::vector<Key>& GetChildKeys() const;
		const std::vector<Entry>& GetEntries() const;

		Key& operator=(const Key& other) {
			if (this != &other) {
				parent = other.parent;
				key = other.key;
				path = other.path;
				childKeys = other.childKeys;
				entries = other.entries;
				creationResult = other.creationResult;
				dirty = other.dirty;

				parent->users++;
				key->users++;
			}
			return *this;
		}
		Key& operator=(Key&& other) noexcept {
			if (this != &other) {
				parent = other.parent;
				key = other.key;
				path = other.path;
				childKeys = other.childKeys;
				entries = other.entries;
				creationResult = other.creationResult;
				dirty = other.dirty;

				parent->users++;
				key->users++;
			}
			return *this;
		}
	private:
		struct KeyUser {
			KeyUser();
			~KeyUser();

			HKEY key = nullptr;
			int users;
		};

		Key(const CS_HLWA_STRING& _path, KeyUser* _parent);

		void InitializeKey();

		KeyUser* parent = nullptr;
		KeyUser* key = nullptr;
		CS_HLWA_STRING path;
		std::vector<Key> childKeys;
		std::vector<Entry> entries;
		CS_HLWA_R creationResult;
		bool dirty = false;
	};
}
#endif
#endif