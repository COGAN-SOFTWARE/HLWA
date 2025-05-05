workspace "HLWR"
   architecture "x64"
   configurations {
      "Debug", "Release"
   }
   preferredtoolarchitecture "x86_64"

   targetdir "bin/%{prj.name}/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"
   language "C++"
   toolset "clang"
   cppdialect "C++20"

   filter "configurations:Debug"
      defines { "DEBUG" }
      runtime "Debug"
      optimize "Off"
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      runtime "Release"
      optimize "Speed"
      symbols "Off"

project "HLWR"
   local projectDir = "HLWR"
   location (projectDir)
   kind "StaticLib"
   flags { "MultiProcessorCompile" }

   files {
      projectDir.."/**.hpp"
   }