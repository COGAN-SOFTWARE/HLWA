#include "../../include/HLWA/ext_displaymesh.hpp"

#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
#include "../../include/HLWA/ext_glassware.hpp"
#endif

#if defined(_WIN32)
#if defined(CS_HLWA_USEUTF8STRINGS)
#undef UNICODE
#endif
#include <Windows.h>
#endif

namespace CoganSoftware::HLWA::DisplayMesh {
#if defined(_WIN32)
#if defined(CS_HLWA_USEUTF8STRINGS)
	HMONITOR GetMonitorHandle(const char* deviceName) {
#else
	HMONITOR GetMonitorHandle(const wchar_t* deviceName) {
#endif
		struct UserMonitor {
			HMONITOR foundMonitor = nullptr;
#if defined(CS_HLWA_USEUTF8STRINGS)
			const char* deviceName;
#else
			const wchar_t* deviceName;
#endif
		} userMntr = { nullptr, deviceName };

		EnumDisplayMonitors(NULL, NULL,
			[](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
				auto* userMntr = reinterpret_cast<UserMonitor*>(lParam);
				MONITORINFOEX monitorInfoEx;
				ZeroMemory(&monitorInfoEx, sizeof(monitorInfoEx));
				monitorInfoEx.cbSize = sizeof(MONITORINFOEX);

				if (GetMonitorInfo(hMonitor, &monitorInfoEx)) {
#if defined(CS_HLWA_USEUTF8STRINGS)
					if (strcmp(monitorInfoEx.szDevice, userMntr->deviceName) == 0) {
#else
					if (wcscmp(monitorInfoEx.szDevice, userMntr->deviceName) == 0) {
#endif
						userMntr->foundMonitor = hMonitor;
						return FALSE; // Stop enumeration.
					}
				}
				return TRUE; // Continue enumeration.
			},
			reinterpret_cast<LPARAM>(&userMntr)
		);

		return userMntr.foundMonitor;
	}
#endif

	void DisplayMesh::Refresh() {
		g_displays.clear();

#if defined(_WIN32)
		DISPLAY_DEVICE dd;
		DEVMODE dm;

		uint32_t deviceIndex = 0;
		while (true) {
			ZeroMemory(&dd, sizeof(dd));
			ZeroMemory(&dm, sizeof(dm));
			dd.cb = sizeof(dd);
			dm.dmSize = sizeof(dm);
			if (!EnumDisplayDevices(nullptr, deviceIndex, &dd, 0)) break;
			if (EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
				HMONITOR monitor = GetMonitorHandle(dd.DeviceName);

				MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
				GetMonitorInfo(monitor, &monitorInfo);

				DisplayInfo info{};
				info.m_deviceName = dd.DeviceName;
				info.m_specVersion = dm.dmSpecVersion;
				info.m_driverVersion = dm.dmDriverVersion;
				info.m_bitsPerPixel = dm.dmBitsPerPel;
				info.m_frequency = dm.dmDisplayFrequency;
				info.m_monitorSize.m_posX = monitorInfo.rcMonitor.left;
				info.m_monitorSize.m_posY = monitorInfo.rcMonitor.top;
				info.m_monitorSize.m_sizeX = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
				info.m_monitorSize.m_sizeY = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
				info.m_workSize.m_posX = monitorInfo.rcWork.left;
				info.m_workSize.m_posY = monitorInfo.rcWork.top;
				info.m_workSize.m_sizeX = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
				info.m_workSize.m_sizeY = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
				info.m_handle = monitor;

				if (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) g_primaryDisplay = g_displays.size();
				
				g_displays.push_back(info);
			}

			deviceIndex++;
		}
#endif
	}

	DisplayInfo DisplayMesh::GetPrimaryDisplay() {
#if defined(_WIN32)
		if (g_primaryDisplay >= g_displays.size()) return {};
		return g_displays[g_primaryDisplay];
#endif
	}
#if defined(_WIN32)
	DisplayInfo DisplayMesh::GetDisplayFromWindow(void* p_handle) {
		HMONITOR monitor = MonitorFromWindow((HWND)p_handle, MONITOR_DEFAULTTONEAREST);
		for (auto& display : g_displays) {
			if (display.m_handle == monitor) return display;
		}
		return {};
	}
#endif
#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
	DisplayInfo DisplayMesh::GetDisplayFromGlassware(Glassware::Glassware* p_glassware) {
#if defined(_WIN32)
		return GetDisplayFromWindow((HWND)p_glassware->GetHandle());
#else
		return {};
#endif
	}
#endif
	DisplayInfo DisplayMesh::GetDisplayFromCursor() {
#if defined(_WIN32)
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		HMONITOR monitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
		for (auto& display : g_displays) {
			if (display.m_handle == monitor) return display;
		}
#endif
	}
	DisplayInfo DisplayMesh::GetDisplayFromPoint(int p_posX, int p_posY) {
#if defined(_WIN32)
		POINT cursorPos{ p_posX, p_posY };
		HMONITOR monitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
		for (auto& display : g_displays) {
			if (display.m_handle == monitor) return display;
		}
#endif
	}

}
#endif