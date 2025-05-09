#pragma once

#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)

#include "types.hpp"

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
namespace CoganSoftware::HLWA::Glassware {
	class Glassware;
}
#endif
namespace CoganSoftware::HLWA::DisplayMesh {
	struct DisplaySize {
	public:
		int m_posX;
		int m_posY;
		uint32_t m_sizeX;
		uint32_t m_sizeY;
	};
	struct DisplayInfo {
	public:
		CS_HLWA_STRING m_deviceName;
		uint16_t m_specVersion;
		uint16_t m_driverVersion;
		uint32_t m_bitsPerPixel;
		uint32_t m_frequency;
		DisplaySize m_monitorSize{};
		DisplaySize m_workSize{};
#if defined(_WIN32)
		void* m_handle;
#endif
	};

	class DisplayMesh {
	public:
		DisplayMesh() = delete;
		~DisplayMesh() = delete;

		static void Refresh();

		static DisplayInfo GetPrimaryDisplay();
#if defined(_WIN32)
		static DisplayInfo GetDisplayFromWindow(void* p_handle);
#endif
#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
		static DisplayInfo GetDisplayFromGlassware(CoganSoftware::HLWA::Glassware::Glassware* p_glassware);
#endif
		static DisplayInfo GetDisplayFromCursor();
		static DisplayInfo GetDisplayFromPoint(int p_posX, int p_posY);
	private:
		static inline std::vector<DisplayInfo> g_displays{};
		static inline uint32_t g_primaryDisplay = 0;
		
#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
		friend class CoganSoftware::HLWA::Glassware::Glassware;
#endif
	};
}
#endif