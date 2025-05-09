#pragma once

#if defined(CS_HLWA_E_ACCENTS) || defined(CS_HLWA_E_ALL)
#if defined(_WIN32)

#include "types.hpp"

#define CS_HLWA_A_AT				int
#define CS_HLWA_A_AT_ACRYLIC		1
#define CS_HLWA_A_AT_ROUND			2
#define CS_HLWA_A_AT_DARKMODE		3

#if defined(CS_HLWA_E_TASKBAR) || defined(CS_HLWA_E_ALL)
namespace CoganSoftware::HLWA::Glassware {
	class Glassware;
}
#endif
namespace CoganSoftware::HLWA::Accents {
	struct BaseAccent {
		CS_HLWA_A_AT type = 0;
		bool enable = false;
	};
	enum struct AccentState {
		GRADIENT = 1,
		TRANSPARENT_GRADIENT = 2,
		AEROBLURBEHIND = 3,
		ACRYLICBLURBEHIND = 4,
		HOSTBACKDROP = 5
	};
	struct AcrylicAccent : public BaseAccent {
		AcrylicAccent() {
			type = CS_HLWA_A_AT_ACRYLIC;
		}
		AccentState accentState;
		uint32_t gradientColor = 0x99000000; // Uses ARGB.
	};
	enum struct RoundState {
		DEFAULT = 0,
		MEDIUM = 2,
		SMALL = 3
	};
	struct RoundAccent : public BaseAccent {
		RoundAccent() {
			type = CS_HLWA_A_AT_ROUND;
		}
		RoundState roundState;
	};
	struct DarkModeAccent : public BaseAccent {
		DarkModeAccent() {
			type = CS_HLWA_A_AT_DARKMODE;
		}
	};

	CS_HLWA_R SetAccent(void* p_handle, BaseAccent* p_accent);
	CS_HLWA_R SetAccent(Glassware::Glassware* p_glassware, BaseAccent* p_accent);
}
#endif
#endif