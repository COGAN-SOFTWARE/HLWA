#include "../../include/HLWA/ext_utils.hpp"

#if defined(CS_HLWA_E_UTILS) || defined(CS_HLWA_E_ACCENTS) || defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_REGISTRY) || defined(CS_HLWA_E_ALL)

#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
#include "../../include/HLWA/ext_displaymesh.hpp"
#endif
#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
#include "../../include/HLWA/ext_glassware.hpp"
#endif
#if defined(CS_HLWA_E_REGISTRY) || defined(CS_HLWA_E_ALL)
#include "../../include/HLWA/ext_registry.hpp"
#endif

#if defined(_WIN32)
#if defined(CS_HLWA_USEUTF8STRINGS)
#undef UNICODE
#endif
#include <Windows.h>
#include <shobjidl.h>
//#include <shlobj.h>
#endif

namespace CoganSoftware::HLWA::Utils {
#if defined(CS_HLWA_E_UTILS) || defined(CS_HLWA_E_ALL)
	CS_HLWA_R IsAdmin() {
#if defined(_WIN32)
		BOOL isElevated = FALSE;
		HANDLE token = NULL;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
			TOKEN_ELEVATION elevation;
			DWORD size;

			if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
				isElevated = elevation.TokenIsElevated;
			}
			CloseHandle(token);
		} else {
			return CS_HLWA_R_NOTADMIN;
		}

		return (isElevated == true) ? CS_HLWA_R_SUCCESS : CS_HLWA_R_NOTADMIN;
#else
		return CS_HLWA_R_NOTADMIN;
#endif
	}
	CS_HLWA_R ForceRelativePath(CS_HLWA_STRING p_path) {
#if defined(_WIN32)
		size_t pos = p_path.find_last_of(CS_HLWA_STRVAL("\\/"));
		if (pos != std::string::npos) {
			p_path = p_path.substr(0, pos);
			SetCurrentDirectory(p_path.c_str());
			return true;
		}
		return false;
#endif
	}
#endif
	
#if defined(_WIN32) && (defined(CS_HLWA_E_REGISTRY) || defined(CS_HLWA_E_ALL))
	CS_HLWA_R SubmitFileTypeKey(const FileRegistry& p_fileRegistry, const AppIDRegistry& p_appRegistry) {
		auto contentType = p_fileRegistry.contentType;
		auto perceivedType = p_fileRegistry.perceivedType;
		CS_HLWA_STRING def = CS_HLWA_STRVAL("");

		Registry::Key fileKey{ (CS_HLWA_STRVAL(".") + p_fileRegistry.extension).c_str(), Registry::RootKey::ClassesRoot };
		if (!(fileKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return fileKey.GetCreationResult();
		auto r = fileKey.Load();
		if (!(r & CS_HLWA_R_SUCCESS)) return r;
		fileKey.AddEntry({ CS_HLWA_STRVAL("Content Type"), Registry::EntryType::STRING, nullptr, nullptr, &contentType });
		fileKey.AddEntry({ CS_HLWA_STRVAL("PerceivedType"), Registry::EntryType::STRING, nullptr, nullptr, &perceivedType });

		auto openKeyIndex = fileKey.AddChildKey(CS_HLWA_STRVAL("OpenWithProgids"));
		auto& openKey = fileKey.GetChildKeys()[openKeyIndex];
		if (!(openKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return openKey.GetCreationResult();
		r = openKey.Load();
		if (!(r & CS_HLWA_R_SUCCESS)) return r;
		openKey.AddEntry({ p_appRegistry.appID + CS_HLWA_STRVAL(".") + p_fileRegistry.extension, Registry::EntryType::STRING, nullptr, nullptr, &def });
		
		fileKey.DeepSubmit();
		openKey.DeepSubmit();

		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R SubmitAppKey(const FileRegistry& p_fileRegistry, const AppIDRegistry& p_appRegistry) {
		auto desc = p_fileRegistry.description;
		auto appID = p_appRegistry.appID;
		auto iconPath = p_fileRegistry.iconPath;
		auto appPath1 = CS_HLWA_STRVAL("\"") + p_appRegistry.appPath + CS_HLWA_STRVAL("\"");
		auto appPath2 = appPath1 + CS_HLWA_STRVAL("\"%1\"");
		
		Registry::Key appKey{ p_appRegistry.appID + CS_HLWA_STRING(CS_HLWA_STRVAL(".")) + p_fileRegistry.extension, Registry::RootKey::ClassesRoot };
		if (!(appKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return appKey.GetCreationResult();
		auto r = appKey.DeepLoad();
		if (!(r & CS_HLWA_R_SUCCESS)) return r;
		appKey.AddEntry({ CS_HLWA_DEFAULTENTRY, Registry::EntryType::STRING, nullptr, nullptr, &desc });
		appKey.AddEntry({ CS_HLWA_STRVAL("AppUserModelID"), Registry::EntryType::STRING, nullptr, nullptr, &appID });
		
		auto defaultIconKeyIndex = appKey.AddChildKey(CS_HLWA_STRVAL("DefaultIcon"));
		auto& defaultIconKey = appKey.GetChildKeys()[defaultIconKeyIndex];
		if (!(defaultIconKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return defaultIconKey.GetCreationResult();
		r = defaultIconKey.Load();
		if (!(r & CS_HLWA_R_SUCCESS)) return r;
		defaultIconKey.AddEntry({ CS_HLWA_DEFAULTENTRY, Registry::EntryType::STRING, nullptr, nullptr, &iconPath });
		
		auto openKeyIndex = appKey.AddChildKey(CS_HLWA_STRVAL("shell\\open"));
		auto& openKey = appKey.GetChildKeys()[openKeyIndex];
		if (!(openKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return openKey.GetCreationResult();
		r = openKey.Load();
		if (!(r & CS_HLWA_R_SUCCESS)) return r;
		openKey.AddEntry({ CS_HLWA_STRVAL("Icon"), Registry::EntryType::STRING, nullptr, nullptr, &appPath1});
		
		auto commandKeyIndex = openKey.AddChildKey(CS_HLWA_STRVAL("command"));
		auto& commandKey = appKey.GetChildKeys()[commandKeyIndex];
		if (!(commandKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return commandKey.GetCreationResult();
		r = commandKey.Load();
		if (!(r & CS_HLWA_R_SUCCESS)) return r;
		commandKey.AddEntry({ CS_HLWA_DEFAULTENTRY, Registry::EntryType::STRING, nullptr, nullptr, &appPath2});
		
		appKey.Submit();
		defaultIconKey.Submit();
		openKey.Submit();
		commandKey.Submit();

		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R DestroyFileTypeKey(const FileRegistry& p_fileRegistry, const AppIDRegistry& p_appRegistry) {
		Registry::Key fileKey{ (CS_HLWA_STRVAL(".") + p_fileRegistry.extension).c_str(), Registry::RootKey::ClassesRoot };
		if (!(fileKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return fileKey.GetCreationResult();
		
		auto openKeyIndex = fileKey.AddChildKey(CS_HLWA_STRVAL("OpenWithProgids"));
		auto& openKey = fileKey.GetChildKeys()[openKeyIndex];
		if (!(openKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return openKey.GetCreationResult();
		auto r = openKey.Load();
		if (!(r & CS_HLWA_R_SUCCESS)) return r;
		openKey.RemoveEntry(p_appRegistry.appID + CS_HLWA_STRVAL(".") + p_fileRegistry.extension);

		if (openKey.GetEntries().size() == 0) {
			r = fileKey.EraseTree();
			if (!(r & CS_HLWA_R_SUCCESS)) return r;
		} else {
			r = openKey.Submit();
			if (!(r & CS_HLWA_R_SUCCESS)) return r;
		}

		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R DestroyAppKey(const FileRegistry& p_fileRegistry, const AppIDRegistry& p_appRegistry) {
		Registry::Key appKey{ p_appRegistry.appID + CS_HLWA_STRING(CS_HLWA_STRVAL(".")) + p_fileRegistry.extension, Registry::RootKey::ClassesRoot };
		if (!(appKey.GetCreationResult() & CS_HLWA_R_SUCCESS)) return appKey.GetCreationResult();

		appKey.EraseTree();

		return CS_HLWA_R_SUCCESS;
	}
#endif

	/*
#if defined(CS_HLWA_USEUTF8STRINGS)
	static std::wstring to_wstring(const std::string& str) {
		std::wstring wstr(str.size(), L'\0');
		std::mbstowcs(&wstr[0], str.c_str(), str.size());
		return wstr;
	}
#endif
	*/

#if defined(_WIN32) && (defined(CS_HLWA_E_UTILS) || defined(CS_HLWA_E_ALL))
	static HINSTANCE g_user32 = nullptr;

	void* GetUser32Library() {
		if (g_user32 == nullptr) g_user32 = LoadLibrary(CS_HLWA_STRVAL("user32.dll"));
		return g_user32;
	}
	CS_HLWA_R FreeUser32Library() {
		if (g_user32 == nullptr) return CS_HLWA_R_NOTFOUND;
		FreeLibrary(g_user32);
		g_user32 = nullptr;
		return CS_HLWA_R_SUCCESS;
	}
	void SetupCOM() {
		CoInitialize(nullptr);
	}
	void FreeCOM() {
		CoUninitialize();
	}
	/* TODO: Move this over to JumpList.
	CS_HLWA_R SetAppUserModelID(const AppIDRegistry& p_appRegistry) {
#if defined(_WIN32)
		if (p_appRegistry.appID != CS_HLWA_STRVAL(""))
#if defined(CS_HLWA_USEUTF8STRINGS)
			SetCurrentProcessExplicitAppUserModelID(to_wstring(p_appRegistry.appID).c_str());
#else
			SetCurrentProcessExplicitAppUserModelID(p_appRegistry.appID.c_str());
#endif
#endif
	}
	CS_HLWA_R FreeAppUserModelID() {
#if defined(_WIN32)
		//
#endif
	}
	*/
	CS_HLWA_R SetDPIV2() {\
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);\
	}
	void AttemptWindowDrag(void* p_handle) {
		ReleaseCapture();
		SendMessage((HWND)p_handle, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
	}
#endif

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
	void AttemptGlasswareDrag(Glassware::Glassware* p_glassware) {
#if defined(_WIN32)
		ReleaseCapture();
		SendMessage((HWND)p_glassware->GetHandle(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
#endif
	}
	CS_HLWA_R GetPositionConversion(Glassware::Glassware* p_glassware, Glassware::PositionState p_state, int p_offsetX, int p_offsetY, int& p_outX, int& p_outY) {
#if defined(_WIN32)
		int posX;
		int posY;
		uint32_t sizeX;
		uint32_t sizeY;

		if (p_glassware->GetHandle() == nullptr) return CS_HLWA_R_INVALID;

		auto display = DisplayMesh::DisplayMesh::GetDisplayFromGlassware(p_glassware);
		MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
		GetMonitorInfo((HMONITOR)display.m_handle, &monitorInfo);
		posX = monitorInfo.rcMonitor.left;
		posY = monitorInfo.rcMonitor.top;
		sizeX = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		sizeY = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

		switch (p_state) {
		case Glassware::PositionState::DEFAULT:
			p_outX = CW_USEDEFAULT;
			p_outY = CW_USEDEFAULT;
			break;
		case Glassware::PositionState::TOP_LEFT:
			p_outX = posX;
			p_outY = posY;
			break;
		case Glassware::PositionState::TOP_MIDDLE:
			p_outX = posX + (sizeX) / 2;
			p_outY = posY;
			break;
		case Glassware::PositionState::TOP_RIGHT:
			p_outX = posX + sizeX;
			p_outY = posY;
			break;
		case Glassware::PositionState::MIDDLE_LEFT:
			p_outX = posX;
			p_outY = posY + (sizeY) / 2;
			break;
		case Glassware::PositionState::MIDDLE_MIDDLE:
			p_outX = posX + (sizeX) / 2;
			p_outY = posY + (sizeY) / 2;
			break;
		case Glassware::PositionState::MIDDLE_RIGHT:
			p_outX = posX + sizeX;
			p_outY = posY + (sizeY) / 2;
			break;
		case Glassware::PositionState::BOTTOM_LEFT:
			p_outX = posX;
			p_outY = posY + sizeY;
			break;
		case Glassware::PositionState::BOTTOM_MIDDLE:
			p_outX = posX + (sizeX) / 2;
			p_outY = posY + sizeY;
			break;
		case Glassware::PositionState::BOTTOM_RIGHT:
			p_outX = posX + sizeX;
			p_outY = posY + sizeY;
			break;
		}

		if (p_outX != CW_USEDEFAULT) p_outX += p_offsetX;
		if (p_outY != CW_USEDEFAULT) p_outY += p_offsetY;
#endif
		return CS_HLWA_R_SUCCESS;
	}
#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
	CS_HLWA_R GetPositionConversion(const DisplayMesh::DisplayInfo& p_displayInfo, Glassware::PositionState p_state, int p_offsetX, int p_offsetY, int& p_outX, int& p_outY) {
#if defined(_WIN32)
		int posX;
		int posY;
		uint32_t sizeX;
		uint32_t sizeY;

		if (p_displayInfo.m_handle == nullptr) return CS_HLWA_R_INVALID;

		posX = p_displayInfo.m_monitorSize.m_posX;
		posY = p_displayInfo.m_monitorSize.m_posY;
		sizeX = p_displayInfo.m_monitorSize.m_sizeX;
		sizeY = p_displayInfo.m_monitorSize.m_sizeY;

		switch (p_state) {
		case Glassware::PositionState::DEFAULT:
			p_outX = CW_USEDEFAULT;
			p_outY = CW_USEDEFAULT;
			break;
		case Glassware::PositionState::TOP_LEFT:
			p_outX = posX;
			p_outY = posY;
			break;
		case Glassware::PositionState::TOP_MIDDLE:
			p_outX = posX + (sizeX) / 2;
			p_outY = posY;
			break;
		case Glassware::PositionState::TOP_RIGHT:
			p_outX = posX + sizeX;
			p_outY = posY;
			break;
		case Glassware::PositionState::MIDDLE_LEFT:
			p_outX = posX;
			p_outY = posY + (sizeY) / 2;
			break;
		case Glassware::PositionState::MIDDLE_MIDDLE:
			p_outX = posX + (sizeX) / 2;
			p_outY = posY + (sizeY) / 2;
			break;
		case Glassware::PositionState::MIDDLE_RIGHT:
			p_outX = posX + sizeX;
			p_outY = posY + (sizeY) / 2;
			break;
		case Glassware::PositionState::BOTTOM_LEFT:
			p_outX = posX;
			p_outY = posY + sizeY;
			break;
		case Glassware::PositionState::BOTTOM_MIDDLE:
			p_outX = posX + (sizeX) / 2;
			p_outY = posY + sizeY;
			break;
		case Glassware::PositionState::BOTTOM_RIGHT:
			p_outX = posX + sizeX;
			p_outY = posY + sizeY;
			break;
		}

		if (p_outX != CW_USEDEFAULT) p_outX += p_offsetX;
		if (p_outY != CW_USEDEFAULT) p_outY += p_offsetY;
#endif
		return CS_HLWA_R_SUCCESS;
	}
#endif
#endif
}
#endif