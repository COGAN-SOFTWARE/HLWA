workspace "HLWA"
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

project "HLWA"
   local projectDir = "HLWA"
   location (projectDir)
   kind "StaticLib"
   flags { "MultiProcessorCompile" }

   defines { "CS_HLWA_E_ALL" }

   files {
      projectDir.."/**.cpp",
      projectDir.."/**.hpp"
   }

project "GlasswareExample"
   local projectDir = "examples/GlasswareExample"
   location (projectDir)
   kind "ConsoleApp"
   flags { "MultiProcessorCompile" }

   files {
      projectDir.."/**.cpp",
      projectDir.."/**.hpp"
   }
   
   includedirs {
      "HLWA/include"
   }
   links {
      "HLWA"
   }

   filter "configurations:Debug"
      libdirs {
         "bin/HLWA/Debug-windows-x86_64"
      }

   filter "configurations:Release"
      libdirs {
         "bin/HLWA/Release-windows-x86_64"
      }

project "JumplistExample"
   local projectDir = "examples/JumplistExample"
   location (projectDir)
   kind "ConsoleApp"
   flags { "MultiProcessorCompile" }

   files {
      projectDir.."/**.cpp",
      projectDir.."/**.hpp"
   }
   
   includedirs {
      "HLWA/include"
   }
   links {
      "HLWA"
   }

   filter "configurations:Debug"
      libdirs {
         "bin/HLWA/Debug-windows-x86_64"
      }

   filter "configurations:Release"
      libdirs {
         "bin/HLWA/Release-windows-x86_64"
      }

project "RegistryExample"
   local projectDir = "examples/RegistryExample"
   location (projectDir)
   kind "ConsoleApp"
   flags { "MultiProcessorCompile" }

   files {
      projectDir.."/**.cpp",
      projectDir.."/**.hpp"
   }
   
   includedirs {
      "HLWA/include"
   }
   links {
      "HLWA"
   }

   filter "configurations:Debug"
      libdirs {
         "bin/HLWA/Debug-windows-x86_64"
      }

   filter "configurations:Release"
      libdirs {
         "bin/HLWA/Release-windows-x86_64"
      }

project "TaskbarExample"
   local projectDir = "examples/TaskbarExample"
   location (projectDir)
   kind "ConsoleApp"
   flags { "MultiProcessorCompile" }

   files {
      projectDir.."/**.cpp",
      projectDir.."/**.hpp"
   }
   
   includedirs {
      "HLWA/include"
   }
   links {
      "HLWA"
   }

   filter "configurations:Debug"
      libdirs {
         "bin/HLWA/Debug-windows-x86_64"
      }

   filter "configurations:Release"
      libdirs {
         "bin/HLWA/Release-windows-x86_64"
      }