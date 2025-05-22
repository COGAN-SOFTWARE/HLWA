#include "../../include/HLWA/ext_jumplist.hpp"

#if defined(CS_HLWA_E_JUMPLIST) || defined(CS_HLWA_E_ALL)

#if defined(_WIN32)
#if defined(CS_HLWA_USEUTF8STRINGS)
#undef UNICODE
#endif
#include <Windows.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <propvarutil.h>				// InitPropVariantFromString
#include <propkey.h>					// PKEY_Title
#pragma comment(lib, "propsys.lib")		// Needed for InitPropVariantFromString

namespace CoganSoftware::HLWA::Jumplist {
#if defined(CS_HLWA_USEUTF8STRINGS)
	static std::wstring to_wstring(const std::string& str) {
		std::wstring wstr(str.size(), L'\0');
		std::mbstowcs(&wstr[0], str.c_str(), str.size());
		return wstr;
	}
#endif

	static inline ICustomDestinationList* g_destList = nullptr;

	CS_HLWA_R Setup(const Utils::AppIDRegistry& p_appRegistry) {
		if (p_appRegistry.appID == "") return CS_HLWA_R_INVALID;
#if defined(CS_HLWA_USEUTF8STRINGS)
		SetCurrentProcessExplicitAppUserModelID(to_wstring(p_appRegistry.appID).c_str());
#else
		SetCurrentProcessExplicitAppUserModelID(p_appRegistry.appID.c_str());
#endif
		CoCreateInstance(CLSID_DestinationList, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_destList));
#if defined(CS_HLWA_USEUTF8STRINGS)
		g_destList->SetAppID(to_wstring(p_appRegistry.appID).c_str());
#else
		g_destList->SetAppID(appRegistry.appID.c_str());
#endif
		return CS_HLWA_R_SUCCESS;
	}
	CS_HLWA_R Free() {
		if (g_destList == nullptr) return CS_HLWA_R_NOTFOUND;
		g_destList->Release();
		g_destList = nullptr;
		return CS_HLWA_R_SUCCESS;
	}
	void AddRecentDoc(const CS_HLWA_STRING& p_path) {
#if defined(CS_HLWA_USEUTF8STRINGS)
		SHAddToRecentDocs(SHARD_PATHA, p_path.c_str());
#else
		SHAddToRecentDocs(SHARD_PATHW, p_path.c_str());
#endif
	}
	CS_HLWA_R SetTasks(const std::vector<TaskArg>& p_args) {
		HRESULT r;
		uint32_t slots;
		IObjectArray* removedObjects;
		r = g_destList->BeginList(&slots, IID_PPV_ARGS(&removedObjects));
		if (FAILED(r)) return CS_HLWA_R_INVALID;

		CS_HLWA_R result = CS_HLWA_R_SUCCESS;

		IObjectCollection* collection;
		r = CoCreateInstance(CLSID_EnumerableObjectCollection, nullptr, CLSCTX_INPROC, IID_PPV_ARGS(&collection));

		for (auto& arg : p_args) {
			IShellLink* finalLink = nullptr;
			IShellLink* link = nullptr;
			IPropertyStore* store = nullptr;
			
			r = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link));
			if (FAILED(r)) {
				result = CS_HLWA_R_INVALID;
				goto next;
			}
#if defined(CS_HLWA_USEUTF8STRINGS)
			char appPath[MAX_PATH];
#else
			wchar_t appPath[MAX_PATH];
#endif
			r = GetModuleFileName(nullptr, appPath, ARRAYSIZE(appPath));
			if (FAILED(r)) {
				result = CS_HLWA_R_INVALID;
				goto next;
			}
			r = link->SetPath(appPath);
			if (FAILED(r)) {
				result = CS_HLWA_R_INVALID;
				goto next;
			}
			r = link->SetArguments(arg.arg.c_str());
			if (FAILED(r)) {
				result = CS_HLWA_R_INVALID;
				goto next;
			}
			r = link->QueryInterface(IID_PPV_ARGS(&store));
			if (FAILED(r)) {
				result = CS_HLWA_R_INVALID;
				goto next;
			}
			PROPVARIANT propVar;
#if defined(CS_HLWA_USEUTF8STRINGS)
			r = InitPropVariantFromString(to_wstring(arg.name).c_str(), &propVar);
#else
			r = InitPropVariantFromString(arg.name.c_str(), &propVar);
#endif
			if (FAILED(r)) {
				result = CS_HLWA_R_INVALID;
				PropVariantClear(&propVar);
				goto next;
			}
			r = store->SetValue(PKEY_Title, propVar);
			if (FAILED(r)) {
				result = CS_HLWA_R_INVALID;
				PropVariantClear(&propVar);
				goto next;
			}
			r = store->Commit();
			if (FAILED(r)) {
				result = CS_HLWA_R_INVALID;
				PropVariantClear(&propVar);
				goto next;
			}
			r = link->QueryInterface(IID_PPV_ARGS(&finalLink));
			PropVariantClear(&propVar);
			//store->Release();
			//link->Release();

			r = collection->AddObject(finalLink);
			
			if (finalLink) finalLink->Release();
			if (link) link->Release();
			if (store) store->Release();
		}

		IObjectArray* arr;
		r = collection->QueryInterface(IID_PPV_ARGS(&arr));

		g_destList->AddUserTasks(arr);
next:
		g_destList->CommitList();

		collection->Release();
		removedObjects->Release();

		return result;
	}
}
#endif
#endif