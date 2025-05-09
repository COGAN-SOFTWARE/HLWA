#pragma once

#if defined(CS_HLWA_E_UTILS) || defined(CS_HLWA_E_ACCENTS) || defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_REGISTRY) || defined(CS_HLWA_E_ALL)

#include "types.hpp"

#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
namespace CoganSoftware::HLWA::DisplayMesh {
	struct DisplayInfo;
}
#endif
#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
namespace CoganSoftware::HLWA::Glassware {
	enum class PositionState;
	class Glassware;
}
#endif
namespace CoganSoftware::HLWA::Utils {
#if defined(CS_HLWA_E_UTILS) || defined(CS_HLWA_E_ALL)
	CS_HLWA_R IsAdmin();
	CS_HLWA_R ForceRelativePath(CS_HLWA_STRING p_path);
#endif
	
#if defined(_WIN32) && (defined(CS_HLWA_E_UTILS) || defined(CS_HLWA_E_ALL))
	struct AppIDRegistry {
	public:
		CS_HLWA_STRING appPath;
		CS_HLWA_STRING appID;
	};
	struct FileRegistry {
	public:
		CS_HLWA_STRING extension;
		CS_HLWA_STRING description;
		CS_HLWA_STRING contentType;
		CS_HLWA_STRING perceivedType;
		CS_HLWA_STRING iconPath;
	};
#endif

#if defined(_WIN32) && (defined(CS_HLWA_E_REGISTRY) || defined(CS_HLWA_E_ALL))
	CS_HLWA_R SubmitFileTypeKey(const FileRegistry& p_fileRegistry, const AppIDRegistry& p_appRegistry);
	CS_HLWA_R SubmitAppKey(const FileRegistry& p_fileRegistry, const AppIDRegistry& p_appRegistry);
	CS_HLWA_R DestroyFileTypeKey(const FileRegistry& p_fileRegistry, const AppIDRegistry& p_appRegistry);
	CS_HLWA_R DestroyAppKey(const FileRegistry& p_fileRegistry, const AppIDRegistry& p_appRegistry);
#endif
	
#if defined(_WIN32) && (defined(CS_HLWA_E_UTILS) || defined(CS_HLWA_E_ALL))
	void* GetUser32Library();
	CS_HLWA_R FreeUser32Library();
	void SetupCOM(); // Call this once per thread.
	void FreeCOM(); // Call this at the end of the current thread.
	CS_HLWA_R SetDPIV2();
	void AttemptWindowDrag(void* p_handle);
#endif

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
	void AttemptGlasswareDrag(Glassware::Glassware* p_glassware);
	CS_HLWA_R GetPositionConversion(Glassware::Glassware* p_glassware, Glassware::PositionState p_state, int p_offsetX, int p_offsetY, int& p_outX, int& p_outY);
#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
	CS_HLWA_R GetPositionConversion(const DisplayMesh::DisplayInfo& p_displayInfo, Glassware::PositionState p_state, int p_offsetX, int p_offsetY, int& p_outX, int& p_outY);
#endif
#endif
}
#endif