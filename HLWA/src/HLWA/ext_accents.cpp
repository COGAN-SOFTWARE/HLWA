#include "../../include/HLWA/ext_accents.hpp"

#if defined(CS_HLWA_E_ACCENTS) || defined(CS_HLWA_E_ALL)

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
#include "../../include/HLWA/ext_glassware.hpp"
#endif

#include "../../include/HLWA/ext_utils.hpp"

#if defined(_WIN32)
#include <Windows.h>
#include <dwmapi.h>
#pragma comment (lib, "Dwmapi")

namespace CoganSoftware::HLWA::Accents {
	struct ACCENTPOLICY {
		UINT accentState;
		UINT accentFlags;
		COLORREF gradientColor;
		LONG animationId;
	};
	enum struct WINDOWCOMPOSITIONATTRIB {
		WCA_ACCENT_POLICY = 0x13
	};
	struct WINDOWCOMPOSITIONATTRIBDATA {
		WINDOWCOMPOSITIONATTRIB attribute;
		LPVOID pvData;
		UINT cbData;
	};
	CS_HLWA_R SetAccent(void* p_handle, BaseAccent* p_accent) {
		auto user32 = (HMODULE)Utils::GetUser32Library();
		if (user32 == nullptr) return CS_HLWA_R_INVALID;

		typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
		const pSetWindowCompositionAttribute SWCA = (pSetWindowCompositionAttribute)GetProcAddress(user32, "SetWindowCompositionAttribute");
		
		switch (p_accent->type) {
		case(CS_HLWA_A_AT_ACRYLIC): {
			auto* accent = (AcrylicAccent*)p_accent;
			if (!SWCA) return CS_HLWA_R_INVALID;
			ACCENTPOLICY policy{
				.accentState = accent->enable ? (uint32_t)accent->accentState : 0,
				.accentFlags = 2,
				.gradientColor = accent->gradientColor,
				.animationId = 0
			};
			WINDOWCOMPOSITIONATTRIBDATA data{
				.attribute = WINDOWCOMPOSITIONATTRIB::WCA_ACCENT_POLICY,
				.pvData = &policy,
				.cbData = sizeof(ACCENTPOLICY)
			};
			SWCA((HWND)p_handle, &data);
			break;
		}
		case(CS_HLWA_A_AT_ROUND): {
			auto* accent = (RoundAccent*)p_accent;
			DWM_WINDOW_CORNER_PREFERENCE roundState;
			if (accent->enable == false) roundState = (DWM_WINDOW_CORNER_PREFERENCE)1;
			else if (accent->roundState == RoundState::DEFAULT) roundState = (DWM_WINDOW_CORNER_PREFERENCE)0;
			else if (accent->roundState == RoundState::SMALL) roundState = (DWM_WINDOW_CORNER_PREFERENCE)3;
			else if (accent->roundState == RoundState::MEDIUM) roundState = (DWM_WINDOW_CORNER_PREFERENCE)2;
			else roundState = (DWM_WINDOW_CORNER_PREFERENCE)1;
			DwmSetWindowAttribute((HWND)p_handle, DWMWINDOWATTRIBUTE::DWMWA_WINDOW_CORNER_PREFERENCE, &roundState, sizeof(roundState));
			break;
		}
		case(CS_HLWA_A_AT_DARKMODE): {
			auto* accent = (DarkModeAccent*)p_accent;
			if (accent->enable) {
				DwmSetWindowAttribute((HWND)p_handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &accent->enable, sizeof(accent->enable));
			}
			break;
		}
		default:
			return CS_HLWA_R_INVALIDARGS;
		}

		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R SetAccent(Glassware::Glassware* p_glassware, BaseAccent* p_accent) {
		return SetAccent(p_glassware->GetHandle(), p_accent);
	}
}
#endif
#endif