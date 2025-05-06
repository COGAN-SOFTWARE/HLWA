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
typedef struct HKEY__ *HKEY; // Forward declaration.

namespace CoganSoftware::HLWR {
	static CS_GWR_R IsAdmin();

	enum struct EntryType {
		DWORD,  // REG_DWORD
		BINARY, // REG_BINARY
		STRING  // REG_SZ
	};
	struct Entry {
	public:
		Entry(const CS_GWR_STRING& _name, EntryType _type, uint32_t* inDword, std::vector<uint8_t>* inBinary, CS_GWR_STRING* inString);
		~Entry() {};
		
		CS_GWR_R GetCreationResult() const;

		void ForceDirty();
		bool IsDirty() const;

		const CS_GWR_STRING& GetName() const;
		void SetName(const CS_GWR_STRING& _name);

		EntryType GetType();
		// Setting the type also clears the data.
		CS_GWR_R SetType(EntryType _type);
		CS_GWR_R GetData(uint32_t* outDword, std::vector<uint8_t>* outBinary, CS_GWR_STRING* outString);
		CS_GWR_R SetData(uint32_t* inDword, std::vector<uint8_t>* inBinary, CS_GWR_STRING* inString);
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
		Key(const CS_GWR_STRING& _path, RootKey root);
		Key(const Key& other);
		Key(Key&& other) noexcept;
		~Key();
		
		CS_GWR_R GetCreationResult() const;

		CS_GWR_R Load();
		CS_GWR_R DeepLoad();
		CS_GWR_R Submit();
		CS_GWR_R DeepSubmit();
		CS_GWR_R EraseTree();
		
		void ForceDirty();
		bool IsDirty() const;

		const CS_GWR_STRING& GetPath() const;
		
		void Clear();

		Key& AddChildKey(const CS_GWR_STRING& name);
		CS_GWR_R AddEntry(Entry& entry);
		CS_GWR_R RemoveChildKey(const CS_GWR_STRING& keyName);
		CS_GWR_R RemoveEntry(const CS_GWR_STRING& entryName);
		
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

		Key(const CS_GWR_STRING& _path, KeyUser* _parent);

		void InitializeKey();

		KeyUser* parent = nullptr;
		KeyUser* key = nullptr;
		CS_GWR_STRING path;
		std::vector<Key> childKeys;
		std::vector<Entry> entries;
		CS_GWR_R creationResult;
		bool dirty = false;
	};
}
#endif