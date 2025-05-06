#include "../../include/HLWA/ext_utils.hpp"

#if defined(CS_HLWA_E_REGISTRY) || defined(CS_HLWA_E_ALL)
#if defined(_WIN32)
#include <Windows.h>

namespace CoganSoftware::HLWA::Utils {
	CS_HLWA_R IsAdmin() {
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
	}
}
#endif
#endif