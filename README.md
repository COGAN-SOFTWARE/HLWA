# HLWA
---
### About

> HLWA stands for High-Level Windows Abstraction and is an API developed for C++ developers to quickly build code to access the windows API for projects. This project originally was named HLWR (High-Level Windows Registry) however I've been merging all projects together into one main abstraction.

---
### How To Compile

> **Note**: If you want to user the compiled static library, output build location is %root%/bin/HLWA/%buildcfg%-%system%-%architecture%/HLWA.lib.<br>
> **Note**: Be aware, by default, `CS_HLWA_E_ALL` is defined in both `premake5.lua` & `CMakeLists.txt` for project `HLWA` (however each extension or all extensions can be enabled when including the header file).

##### Visual Studio (+ premake5)

- Run `build-premake5-vs22-clang.bat`.
- Open `HLWA.sln`.
- Compile for Build/Release.

##### Visual Studio (+ CMake)

- Run `build-cmake-vs22-clang.bat`.
- Goto `build/`.
- Open `HLWA.sln`.
- Compile for Build/Release.

##### CMake (clang/gcc) **DOES NOT CURRENTLY WORK**

- Run `build-cmake-clang.bat` or `build-cmake-gcc.bat`.
- Run `compile-cmake-debug.bat` or `compile-cmake-release.bat`.     **As of right now, this does not work as intended!!!**

---
### FAQ

> **Q**: I thought this was a lightweight single-header API?<br>
> **A**: As of recent, some code has been moved into an implementation file; however it's still lightweight and can be thrown directly in your project.

> **Q**: Can I use UTF8 instead of UTF16 strings?<br>
> **A**: Yes you can, define `CS_GWR_USEUTF8STRINGS` before including this header file.

> **Q**: Will `Windows.h` leak into my project?<br>
> **A**: No, in a previous update converting from single-header API to a simple lightweight API, we moved the `#include` statement into the implementation file and forward declared `HKEY` internally (this is currently still visible though; no direct windows functionality however).

> **Q**: When will standalone CMake clang/gcc work?<br>
> **A**: I'm not focusing on those issues right now, however if anyone has a solution to the cli errors when compiling the CMake project using standalone compilers, please reach out to us.

> **Q**: The code for user input looks like GLFW, is this true?<br>
> **A**: All credit for how input is interpreted goes to GLFW, while I did do all my touches and features as well as how some things get passed to the user, ultimately it's all derived from GLFW; otherwise everything else (except for repainting in the `WindowProc`) is solely self-developed.

---
### How To Use

##### Enabling Extensions

> The API is split up between extensions, all of which can contribute to one another but built in isolated sections of code. It's recommended to only include `core.hpp` and specify the extension as macros of which you want to use. Here are a list of extensions you can enable:

> **Note**: Documentation for each extension coming soon...

- `CS_HLWA_E_ALL`
- `CS_HLWA_E_ACCENTS`
- `CS_HLWA_E_DISPLAYMESH`
- `CS_HLWA_E_GLASSWARE`
- `CS_HLWA_E_JUMPLIST`        **WIP; Planned (experimenting).**
- `CS_HLWA_E_LOGPROCESSOR`
- `CS_HLWA_E_REGISTRY`
- `CS_HLWA_E_TASKBAR`
- `CS_HLWA_E_TOAST`           **WIP; Planned (experimenting).**
- `CS_HLWA_E_TRAY`            **WIP; Planned (experimenting).**
- `CS_HLWA_E_UTILS`

##### Examples

> Examples have been moved into separate projects under `examples/`. Each extension provides at least 1 basic example.