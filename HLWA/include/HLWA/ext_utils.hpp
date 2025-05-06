#pragma once

#if defined(CS_HLWA_E_UTILS) || defined(CS_HLWA_E_ALL)

#include "types.hpp"

#if defined(_WIN32)
namespace CoganSoftware::HLWA::Utils {
	static CS_HLWA_R IsAdmin();
}
#endif
#endif