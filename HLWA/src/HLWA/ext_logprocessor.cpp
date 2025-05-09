#include "../../include/HLWA/ext_logprocessor.hpp"

#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)

namespace CoganSoftware::HLWA::LogProcessor {
	static LogProcessorCallbackType g_logProcessorCallback = nullptr;
	void SetLogProcessor(LogProcessorCallbackType p_callback) {
		g_logProcessorCallback = p_callback;
	}
	LogProcessorCallbackType GetLogProcessor() {
		return g_logProcessorCallback;
	}
	CS_HLWA_R CallLogProcessor(BaseLog* p_log) {
		if (p_log == nullptr) return CS_HLWA_R_NOTFOUND;
		if (g_logProcessorCallback == nullptr) return CS_HLWA_R_NOTFOUND;
		g_logProcessorCallback(p_log);
		return CS_HLWA_R_SUCCESS;
	}
}
#endif