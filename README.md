# HLWR
---
### About

> HLWR stands for High-Level Windows Registry and is an API developed for C++ developers to quickly build code to access the windows registry for projects.

---
### How To Compile

> **Note**: If you want to user the compiled static library, output build location is <root>/bin/HLWR/<buildcfg>-<system>-<architecture>/HLWR.lib.

##### Visual Studio (+ premake5)

> This project uses premake5 to build the visual studio files; simply download `premake5.exe` directly into this and run `build-premake5-vs22-clang.bat`; **If you do not want to use premake5, this project comes with the vs22 solution & project files in the source code**. To compile, just build either for Debug or Release directly within visual studio.

##### Visual Studio (+ CMake)

> Run `build-cmake-vs22-clang.bat`, this will generate all the files under a new folder `build`. Open the `HWLR.sln` in that folder and build from there. The compiled static library is placed similarly as before but <root> is now <root>/build.

##### CMake

> **As of now, I have not fixed the non-vs22 compiler bugs; this option does not work without additional cmake code/cli**. Run `build-cmake-clang.bat` or `build-cmake-gcc.bat`, this will generate all the files under a new folder `build`. Then run either `compile-cmake-debug.bat` or `compile-cmake-release.bat`. The compiled static library is placed similarly as before but <root> is now <root>/build.

> TODO

---
### FAQ

**Q**: I thought this was a lightweight single-header API?
**A**: As of recent, some code has been moved into an implementation file; however it's still lightweight and can be thrown directly in your project.

**Q**: Can I use UTF8 instead of UTF16 strings?
**A**: Yes you can, define `CS_GWR_USEUTF8STRINGS` before including this header file.

**Q**: Will `Windows.h` leak into my project?
**A**: No, in a previous update converting from single-header API to a simple lightweight API, we moved the `#include` statement into the implementation file and forward declared `HKEY` internally (this is currently still visible though; no direct windows functionality however).

---
### How To Use

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