#pragma once

#if defined(CS_HLWA_E_TASKBAR) || defined(CS_HLWA_E_ALL)

#include "types.hpp"

#if defined(CS_HLWA_E_TASKBAR) || defined(CS_HLWA_E_ALL)
namespace CoganSoftware::HLWA::Glassware {
	class Glassware;
}
#endif
namespace CoganSoftware::HLWA::Taskbar {
#if defined(_WIN32)
	CS_HLWA_R Initialize();
	CS_HLWA_R Free();
	void Alert(void* p_handle);
	CS_HLWA_R EnableProgress(void* p_handle);
	CS_HLWA_R DisableProgress(void* p_handle);
	CS_HLWA_R SetProgressValue(void* p_handle, size_t p_value, size_t p_total);
	CS_HLWA_R SpinProgress(void* p_handle);
	CS_HLWA_R PauseProgress(void* p_handle);
	CS_HLWA_R ErrorProgress(void* p_handle);
	CS_HLWA_R SetTooltip(void* p_handle, const CS_HLWA_STRING& p_value);
#endif
#if defined(CS_HLWA_E_TASKBAR) || defined(CS_HLWA_E_ALL)
	void Alert(Glassware::Glassware* p_glassware);
	CS_HLWA_R EnableProgress(Glassware::Glassware* p_glassware);
	CS_HLWA_R DisableProgress(Glassware::Glassware* p_glassware);
	CS_HLWA_R SetProgressValue(Glassware::Glassware* p_glassware, size_t p_value, size_t p_total);
	CS_HLWA_R SpinProgress(Glassware::Glassware* p_glassware);
	CS_HLWA_R PauseProgress(Glassware::Glassware* p_glassware);
	CS_HLWA_R ErrorProgress(Glassware::Glassware* p_glassware);
	CS_HLWA_R SetTooltip(Glassware::Glassware* p_glassware, const CS_HLWA_STRING& p_value);
#endif
}
#endif