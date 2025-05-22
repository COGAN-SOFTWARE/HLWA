#pragma once

#if defined(CS_HLWA_E_JUMPLIST) || defined(CS_HLWA_E_ALL)

#include "ext_utils.hpp"

#include "types.hpp"

#if defined(_WIN32)
namespace CoganSoftware::HLWA::Jumplist {
	struct TaskArg {
	public:
		CS_HLWA_STRING name;
		CS_HLWA_STRING arg;
	};
	CS_HLWA_R Setup(const Utils::AppIDRegistry& p_appRegistry); // Call `SetupCOM()` from utils first.
	CS_HLWA_R Free();
	void AddRecentDoc(const CS_HLWA_STRING& p_path);
	CS_HLWA_R SetTasks(const std::vector<TaskArg>& p_args); // Call this once-per-startup and only if it has admin.
}
#endif
#endif