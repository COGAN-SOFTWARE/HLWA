#define CS_HLWA_E_REGISTRY
#define CS_HLWA_E_UTILS
#include <HLWA/core.hpp>
using namespace CoganSoftware::HLWA;

int main() {
	if (!(Utils::IsAdmin() & CS_HLWA_R_SUCCESS)) return 1;

	Registry::Key myKey = Registry::Key{ CS_HLWA_STRVAL("MyExample") };		// This creates/opens to an existing key. The validation can be checked by calling `GetCreationResult()`.

	myKey.Load();											// It is recommended to call `Load()` first before handling the key; this loads all child keys and entries. If you want the child keys to also be fully loaded and so on and so forth, you can call `DeepLoad()`.

	CS_HLWA_STRING str = CS_HLWA_STRVAL("Example Default Entry!!!");
	Registry::Entry defaultEntry = Registry::Entry{
		CS_HLWA_DEFAULTENTRY,								// This is a `std::wstring` input, it's the name of the entry; here we provide the default entry name string which exposed is simply just L"". We write this as a macro for future proofing constants or if we support additional platforms that have a similar registry-like system.
		Registry::EntryType::STRING,
		nullptr,											// For entries, we add each data type as a ptr that can be nullptr if not the desired EntryType; this is present in `SetType()` and `GetData()`
		nullptr,
		&str
	};

	myKey.AddEntry(defaultEntry);

	myKey.Submit();											// This is the process to submit the changed information, this will optimally determine if it needs to delete all entries if entry count has changed at any point or just change the current ones. If you want to also submit all child key information (do not do unless you know you've loaded all of them), call `DeepSubmit()`.

	return 0;
}