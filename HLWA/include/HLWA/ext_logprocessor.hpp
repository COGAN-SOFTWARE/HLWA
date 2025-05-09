#pragma once

#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)

#include "types.hpp"

#define CS_HLWA_LP_LG					int
#define CS_HLWA_LP_LG_OUTPUT			1
#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
#define CS_HLWA_LP_LG_GWRESIZED			2
#define CS_HLWA_LP_LG_GWMOVED			3
#define CS_HLWA_LP_LG_GWHOVERED			4
#define CS_HLWA_LP_LG_GWFOCUSED			5
#define CS_HLWA_LP_LG_GWFILEDROPPED		6
#endif

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
namespace CoganSoftware::HLWA::Glassware {
	class Glassware;
}
#endif
namespace CoganSoftware::HLWA::LogProcessor {
	struct BaseLog {
	public:
		CS_HLWA_LP_LG type = 0;
	};
	enum struct OutputType {
		LOG,
		WARN,
		ERR
	};
	struct OutputLog : public BaseLog {
	public:
		OutputLog() {
			type = CS_HLWA_LP_LG_OUTPUT;
		}
		OutputType outputType;
		CS_HLWA_STRING value;
	};
#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)
	struct BaseGlasswareLog : public BaseLog {
	public:
		Glassware::Glassware* glassware = nullptr;
	};
	struct GlasswareResizedLog : public BaseGlasswareLog {
	public:
		GlasswareResizedLog() {
			type = CS_HLWA_LP_LG_GWRESIZED;
		}
		uint32_t prevX;
		uint32_t prevY;
		uint32_t newX;
		uint32_t newY;
	};
	struct GlasswareMovedLog : public BaseGlasswareLog {
	public:
		GlasswareMovedLog() {
			type = CS_HLWA_LP_LG_GWMOVED;
		}
		int prevX;
		int prevY;
		int newX;
		int newY;
	};
	struct GlasswareHoveredLog : public BaseGlasswareLog {
	public:
		GlasswareHoveredLog() {
			type = CS_HLWA_LP_LG_GWHOVERED;
		}
		bool hovered;
	};
	struct GlasswareFocusedLog : public BaseGlasswareLog {
	public:
		GlasswareFocusedLog() {
			type = CS_HLWA_LP_LG_GWFOCUSED;
		}
		bool focused;
	};
	struct GlasswareFileDropLog : public BaseGlasswareLog {
	public:
		GlasswareFileDropLog() {
			type = CS_HLWA_LP_LG_GWFILEDROPPED;
		}
		double posX;
		double posY;
		const char** paths;
		uint32_t pathCount;
	};
#endif
	
	typedef void(*LogProcessorCallbackType)(BaseLog* p_log);

	void SetLogProcessor(LogProcessorCallbackType p_callback);
	LogProcessorCallbackType GetLogProcessor();
	CS_HLWA_R CallLogProcessor(BaseLog* p_log);
}
#endif