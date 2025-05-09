#include "../../include/HLWA/ext_taskbar.hpp"

#if defined(CS_HLWA_E_TASKBAR) || defined(CS_HLWA_E_ALL)

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
#include "../../include/HLWA/ext_glassware.hpp"
#endif

#if defined(_WIN32)
#include <Windows.h>
#include <shobjidl.h>
#endif

namespace CoganSoftware::HLWA::Taskbar {
#if defined(CS_HLWA_USEUTF8STRINGS)
	static std::wstring to_wstring(const std::string& str) {
		std::wstring wstr(str.size(), L'\0');
		std::mbstowcs(&wstr[0], str.c_str(), str.size());
		return wstr;
	}
#endif

#if defined(_WIN32)
	static ITaskbarList3* g_taskbar = nullptr;

	CS_HLWA_R Initialize() {
		if (g_taskbar == nullptr) {
			HRESULT r = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_taskbar));
			if (r != ERROR_SUCCESS) return CS_HLWA_R_SUCCESS;
			return CS_HLWA_R_INVALID;
		}
		return CS_HLWA_R_ALREADYEXISTS;
	}
	CS_HLWA_R Free() {
		if (g_taskbar == nullptr) return CS_HLWA_R_NOTFOUND;
		g_taskbar->Release();
		return CS_HLWA_R_SUCCESS;
	}
	void Alert(void* p_handle) {
		FlashWindow((HWND)p_handle, true);
	}
	CS_HLWA_R EnableProgress(void* p_handle) {
		if (g_taskbar) {
			g_taskbar->SetProgressState((HWND)p_handle, TBPF_NORMAL);
			return CS_HLWA_R_SUCCESS;
		}
		return CS_HLWA_R_INVALID;
	}
	CS_HLWA_R DisableProgress(void* p_handle) {
		if (g_taskbar) {
			g_taskbar->SetProgressState((HWND)p_handle, TBPF_NOPROGRESS);
			return CS_HLWA_R_SUCCESS;
		}
		return CS_HLWA_R_INVALID;
	}
	CS_HLWA_R SetProgressValue(void* p_handle, size_t p_value, size_t p_total) {
		if (g_taskbar) {
			g_taskbar->SetProgressValue((HWND)p_handle, p_value, p_total);
			return CS_HLWA_R_SUCCESS;
		}
		return CS_HLWA_R_INVALID;
	}
	CS_HLWA_R SpinProgress(void* p_handle) {
		if (g_taskbar) {
			g_taskbar->SetProgressState((HWND)p_handle, TBPF_INDETERMINATE);
			return CS_HLWA_R_SUCCESS;
		}
		return CS_HLWA_R_INVALID;
	}
	CS_HLWA_R PauseProgress(void* p_handle) {
		if (g_taskbar) {
			g_taskbar->SetProgressState((HWND)p_handle, TBPF_PAUSED);
			return CS_HLWA_R_SUCCESS;
		}
		return CS_HLWA_R_INVALID;
	}
	CS_HLWA_R ErrorProgress(void* p_handle) {
		if (g_taskbar) {
			g_taskbar->SetProgressState((HWND)p_handle, TBPF_ERROR);
			return CS_HLWA_R_SUCCESS;
		}
		return CS_HLWA_R_INVALID;
	}
	CS_HLWA_R SetTooltip(void* p_handle, const CS_HLWA_STRING& p_value) {
		if (g_taskbar) {
#if defined(CS_HLWA_USEUTF8STRINGS)
			g_taskbar->SetThumbnailTooltip((HWND)p_handle, to_wstring(p_value).c_str());
#else
			g_taskbar->SetThumbnailTooltip((HWND)p_handle, p_value.c_str());
#endif
			return CS_HLWA_R_SUCCESS;
		}
		return CS_HLWA_R_INVALID;
	}
#endif

#if defined(CS_HLWA_E_TASKBAR) || defined(CS_HLWA_E_ALL)
	void Alert(Glassware::Glassware* p_glassware) {
#if defined(_WIN32)
		Alert(p_glassware->GetHandle());
#endif
	}
	CS_HLWA_R EnableProgress(Glassware::Glassware* p_glassware) {
#if defined(_WIN32)
		EnableProgress(p_glassware->GetHandle());
#endif
	}
	CS_HLWA_R DisableProgress(Glassware::Glassware* p_glassware) {
#if defined(_WIN32)
		DisableProgress(p_glassware->GetHandle());
#endif
	}
	CS_HLWA_R SetProgressValue(Glassware::Glassware* p_glassware, size_t p_value, size_t p_total) {
#if defined(_WIN32)
		SetProgressValue(p_glassware->GetHandle(), p_value, p_total);
#endif
	}
	CS_HLWA_R SpinProgress(Glassware::Glassware* p_glassware) {
#if defined(_WIN32)
		SpinProgress(p_glassware->GetHandle());
#endif
	}
	CS_HLWA_R PauseProgress(Glassware::Glassware* p_glassware) {
#if defined(_WIN32)
		PauseProgress(p_glassware->GetHandle());
#endif
	}
	CS_HLWA_R ErrorProgress(Glassware::Glassware* p_glassware) {
#if defined(_WIN32)
		ErrorProgress(p_glassware->GetHandle());
#endif
	}
	CS_HLWA_R SetTooltip(Glassware::Glassware* p_glassware, const CS_HLWA_STRING& p_value) {
#if defined(_WIN32)
		SetTooltip(p_glassware->GetHandle(), p_value);
#endif
	}
#endif
}
#endif