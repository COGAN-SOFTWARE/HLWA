# HLWR

> HLWR stands for High-Level Windows Registry and is an API developed for C++ developers to quickly build code to access the windows registry for projects.

# How To Compile

### Using Visual Studio

> A pre-built vs22 solution comes with the project; premake5 can be installed into the root directory and executed to rebuild the solution. To compile, simply open up the solution and build it from the editor; the project is intended for primarily clang.

### Using CMake

TODO

### Direct Inclusion

> HLWR is designed as a single-header library which means it's perfectly acceptable to include straight into your project's source code and not require to compile this as a separate static library.

# How To Use

> HLWR is designed to be super user friendly. We separate the logic identity of the registry into two main objects; keys and entries are the two main objects you'll be handling. But before you handle those, it's a good thing to check if you have admin privileges. This is a built-in feature as a simple function call: `CoganSoftware::HLWR::IsAdmin()`; returns a CS_GWR_R bit that can either be `CS_GWR_R_SUCCESS` or `CS_GWR_R_NOTADMIN`. Keys are those folder-like items you can view in the registery editor; then there's what we call "entries" (not officially named that by Windows), it's the items that appear as a name type value trio inside keys.

> To begin, here's an example of opening a key and adding an entry.

```cpp
using <hlwr.hpp>
using namespace CoganSoftware::HLWR

int main() {
	if (!(IsAdmin() & CS_GWR_R_SUCCESS)) return 1; // Failed to start as admin.

	Key myKey = Key{ L"MyExample" }; // This creates/opens to an existing key. The validation can be checked by calling `GetCreationResult()`.

	myKey.Load(); // It is recommended to call `Load()` first before handling the key; this loads all child keys and entries. If you want the child keys to also be fully loaded and so on and so forth, you can call `DeepLoad()`.

	std::wstring str = L"Example Default Entry!!!";
	Entry defaultEntry = Entry{
		CS_GWR_DEFAULTENTRY, // This is a `std::wstring` input, it's the name of the entry; here we provide the default entry name string which exposed is simply just L"". We write this as a macro for future proofing constants or if we support additional platforms that have a similar registry-like system.
		EntryType::STRING,
		nullptr,             // For entries, we add each data type as a ptr that can be nullptr if not the desired EntryType; this is present in `SetType()` and `GetData()`
		nullptr,
		str
	};

	myKey.AddEntry(defaultEntry);

	myKey.Submit();      // This is the process to submit the changed information, this will optimally determine if it needs to delete all entries if entry count has changed at any point or just change the current ones. If you want to also submit all child key information (do not do unless you know you've loaded all of them), call `DeepSubmit()`.

	return 0;
}
```