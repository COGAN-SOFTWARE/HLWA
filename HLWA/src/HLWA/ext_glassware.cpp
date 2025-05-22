#include "../../include/HLWA/ext_glassware.hpp"

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)

#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
#include "../../include/HLWA/ext_displaymesh.hpp"
#endif
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
#include "../../include/HLWA/ext_logprocessor.hpp"
#endif
#include "../../include/HLWA/ext_utils.hpp"

#if defined(_WIN32)
#if defined(CS_HLWA_USEUTF8STRINGS)
#undef UNICODE
#endif
#include <windowsx.h>
#include <Windows.h>
#include <dwmapi.h>
#pragma comment (lib, "Dwmapi")

//#include <strsafe.h>
#endif

#include <cassert>

namespace CoganSoftware::HLWA::Glassware {
	IconSpecifier::Data::File::File(const CS_HLWA_STRING& p_path) : path{ p_path } {}
	IconSpecifier::Data::File::File(CS_HLWA_STRING&& p_path) : path{ std::move(p_path) } {}
	IconSpecifier::Data::File::File(const IconSpecifier::Data::File& p_other) : path{ p_other.path } {}
	IconSpecifier::Data::File::~File() {}

	IconSpecifier::Data::Buffer::Buffer(unsigned char* p_data, uint32_t p_dataLength, uint32_t p_width, uint32_t p_height) : data{ p_data }, dataLength{ p_dataLength }, width{ p_width }, height{ p_height } {}
	IconSpecifier::Data::Buffer::Buffer(const Buffer& p_other) : data{ p_other.data }, width{ p_other.width }, height{ p_other.height } {}
	IconSpecifier::Data::Buffer::~Buffer() {}

	IconSpecifier::IconSpecifier(const CS_HLWA_STRING& p_path) : type{ IconSpecifierType::FILE } {
		new (&data.file) Data::File(p_path);
	}
	IconSpecifier::IconSpecifier(unsigned char* p_data, uint32_t p_dataLength, uint32_t p_width, uint32_t p_height) : type{ IconSpecifierType::BUFFER } {
		new (&data.buffer) Data::Buffer(p_data, p_dataLength, p_width, p_height);
	}
	IconSpecifier::IconSpecifier(const IconSpecifier& p_other) : type{ p_other.type } {
		if (type == IconSpecifierType::FILE)
			new (&data.file) Data::File(p_other.data.file);
		else
			new (&data.buffer) Data::Buffer(p_other.data.buffer);
	}
	IconSpecifier::~IconSpecifier() {
		if (type == IconSpecifierType::FILE)
			data.file.~File();
		else
			data.buffer.~Buffer();
	}
	
	static bool g_destroyParentViaDestructor = true;
	bool Glassware::IsDependentsHovered() {
		auto isHovered = GetHoveredFlag();
		uint32_t i = 0;
		while (!isHovered && i < m_children.size()) {
			isHovered = m_children[i]->IsDependentsHovered();
			i++;
		}
		return isHovered;
	}

	void Glassware::TickContextWindow() {
		if (!(m_config.context.flags & CS_HLWA_GW_CSF_CONTEXTMENU)) return;
		auto* p = m_config.parent;
		bool isHovered = IsDependentsHovered();
		if (!isHovered) delete this;
		if (p != nullptr) p->TickContextWindow();
	}

#if defined(_WIN32)
	static Glassware* g_tempInitWindow = nullptr;
	char* w2u(const WCHAR* src) {
		char* tar;
		int size;

		size = WideCharToMultiByte(CP_UTF8, 0, src, -1, nullptr, 0, nullptr, nullptr);
		if (!size) {
			return nullptr; // failed?!?
		}

		tar = (char*)malloc(size);

		if (!WideCharToMultiByte(CP_UTF8, 0, src, -1, tar, size, nullptr, nullptr)) {
			free(tar);
			return nullptr; // failed?!?
		}

		return tar;
	}
	intptr_t __stdcall Glassware::WindowProc(void* p_hwnd, uint32_t p_uMsg, uintptr_t p_wParam, intptr_t p_lParam) {
		auto hwnd = (HWND)p_hwnd;
		Glassware* window = reinterpret_cast<Glassware*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (window == nullptr) window = g_tempInitWindow;
		switch (p_uMsg) {
		case WM_CREATE: {
			if (p_lParam) {
				auto params = reinterpret_cast<LPCREATESTRUCT>(p_lParam);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(params->lpCreateParams));
			}
			return false;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			return false;
		}
		case WM_NCCALCSIZE: {
			// Remove the window's standard sizing border
			if (window == nullptr) break;
			if (p_wParam == true && p_lParam != NULL) {
				window->ForceTriggerResizeFlag();
			}
			if (p_wParam == true && p_lParam != NULL && window->m_config.context.flags & CS_HLWA_GW_CSF_DISABLETITLEBAR) {
				NCCALCSIZE_PARAMS* pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(p_lParam);
				pParams->rgrc[0] = pParams->rgrc[1];
				return 0; // Disable titlebar.
			} else if (p_wParam == false && window->SupportsComposition() && window->m_config.context.flags & CS_HLWA_GW_CSF_TRANSPARENT) {
				return 0; // Enable transparency (dx12 only).
			}
			break;
		}
		case WM_NCHITTEST: {
			uint32_t border_width = 8;
			RECT rect;
			GetWindowRect(hwnd, &rect);
			uint32_t x = LOWORD(p_lParam);
			uint32_t y = HIWORD(p_lParam);

			bool is_left = x >= rect.left && x < rect.left + border_width;
			bool is_top = y >= rect.top && y < rect.top + border_width;
			bool is_right = x < rect.right && x >= rect.right - border_width;
			bool is_bottom = y < rect.bottom && y >= rect.bottom - border_width;

			if (is_left) {
				if (is_top) return HTTOPLEFT;
				else if (is_bottom) return HTBOTTOMLEFT;
				return HTLEFT;
			} else if (is_right) {
				if (is_top) return HTTOPRIGHT;
				else if (is_bottom) return HTBOTTOMRIGHT;
				return HTRIGHT;
			}

			if (is_top) return HTTOP;
			else if (is_bottom) return HTBOTTOM;

			//return HTCAPTION;
			break;
		}
		case WM_SYSCOMMAND: {
			if ((p_wParam & 0xFFF0) == SC_MOVE || (p_wParam & 0xFFF0) == 0xf012) {
				if (window->GetMode() != ModeState::WINDOWED) window->Restore();
			}
			break;
		}
		case WM_PAINT: {
			//window->OnUpdate();
			//return false;
			break;
		}
		case WM_NCACTIVATE:
		case WM_NCPAINT: {
			// So apparently if this is uncommented... re-enabling the titlebar looks like gay as fuck win7 aero.
			//if (window->m_config.context.flags & CS_HLWA_GW_CSF_DISABLETITLEBAR) return true; // prevent title bar from being drawn after restoring
			break;
		}
		case WM_ERASEBKGND: {
			return true;
		}
		case WM_DPICHANGED: {
			const float dpi = HIWORD(p_wParam);

			window->m_dpi = dpi;

			break;
		}
		case WM_GETMINMAXINFO: {
			MINMAXINFO* pMinMax = (MINMAXINFO*)p_lParam;
			HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo = { sizeof(monitorInfo) };

			if (GetMonitorInfo(hMonitor, &monitorInfo)) {
				RECT workArea = monitorInfo.rcWork;
				pMinMax->ptMaxSize.x = workArea.right - workArea.left;
				pMinMax->ptMaxSize.y = workArea.bottom - workArea.top;
				pMinMax->ptMaxPosition.x = workArea.left;
				pMinMax->ptMaxPosition.y = workArea.top;
				pMinMax->ptMaxTrackSize.x = pMinMax->ptMaxSize.x;
				pMinMax->ptMaxTrackSize.y = pMinMax->ptMaxSize.y;
			}
			return 0;
		}
		case WM_SIZE: {
			RECT rect;
			if (p_wParam == SIZE_MAXIMIZED && window->m_config.context.flags & CS_HLWA_GW_CSF_DISABLETITLEBAR) {
				window->m_flags |= CS_HLWA_GW_F_WASMAXIMIZE;
				
				LONG style = GetWindowLong(hwnd, GWL_STYLE);
				style &= ~WS_OVERLAPPEDWINDOW;
				SetWindowLong(hwnd, GWL_STYLE, style);

				MONITORINFO mi = { sizeof(mi) };
#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
				GetMonitorInfo((HMONITOR)DisplayMesh::DisplayMesh::GetDisplayFromGlassware(window).m_handle, &mi);
#else
				GetMonitorInfo(MonitorFromWindow(WINDOWS.m_handle), &mi);
#endif
				SetWindowPos(hwnd, HWND_TOPMOST,
							 mi.rcMonitor.left,
							 mi.rcMonitor.top,
							 mi.rcMonitor.right - mi.rcMonitor.left,
							 mi.rcMonitor.bottom - mi.rcMonitor.top,
							 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);

				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
					 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

				InvalidateRect(hwnd, NULL, TRUE);
			} else if (p_wParam == SIZE_RESTORED && window->m_flags & CS_HLWA_GW_F_WASMAXIMIZE && window->m_config.context.flags & CS_HLWA_GW_CSF_DISABLETITLEBAR) {
				window->m_flags &= ~CS_HLWA_GW_F_WASMAXIMIZE;
				
				SetWindowLong(hwnd, GWL_STYLE, GetWindowLongA(hwnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW); // TODO: I think this needs to be using the cached variables? Fuck me idk...

				RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
			}
			if (p_wParam == SIZE_RESTORED && window->m_config.context.flags & CS_HLWA_GW_CSF_DISABLETITLEBAR) {
				SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);

				MARGINS margins = { 0, 0, 0, 0 };
				DwmExtendFrameIntoClientArea(hwnd, &margins);
			}
			if (GetWindowRect(hwnd, &rect)) {
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
				LogProcessor::GlasswareResizedLog msg{};
				msg.glassware = window;
				msg.prevX = window->m_sizeX;
				msg.prevY = window->m_sizeY;
				msg.newX = rect.right - rect.left;
				msg.newY = rect.bottom - rect.top;
#endif
				window->m_sizeX = rect.right - rect.left;
				window->m_sizeY = rect.bottom - rect.top;
				RECT clientRect;
				if (GetClientRect(hwnd, &clientRect)) {
					window->m_cliX = clientRect.right - clientRect.left;
					window->m_cliY = clientRect.bottom - clientRect.top;
				}
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
				LogProcessor::CallLogProcessor(&msg);
#endif
			}
			//break;
			return 0;
		}
		case WM_MOVE: {
			RECT rect;
			if (GetWindowRect(hwnd, &rect)) {
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
				LogProcessor::GlasswareMovedLog msg{};
				msg.glassware = window;
				msg.prevX = window->m_posX;
				msg.prevY = window->m_posY;
				msg.newX = rect.left;
				msg.newY = rect.top;
#endif
				window->m_posX = rect.left; // Before these were the previous coords? Should be fixed now...
				window->m_posY = rect.top;
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
				LogProcessor::CallLogProcessor(&msg);
#endif
			}
			//break;
			return 0;
		}
		case WM_SETFOCUS: {
			if (window) {
				if (!window->GetFocusedFlag()) {
					window->SetFocusedFlag(true);
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
					LogProcessor::GlasswareFocusedLog msg{};
					msg.glassware = window;
					msg.focused = true;
					LogProcessor::CallLogProcessor(&msg);
#endif
				}
			}
			break;
		}
		case WM_KILLFOCUS: {
			if (window) {
				if (window->GetFocusedFlag()) {
					window->SetFocusedFlag(false);
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
					LogProcessor::GlasswareFocusedLog msg{};
					msg.glassware = window;
					msg.focused = false;
					LogProcessor::CallLogProcessor(&msg);
#endif
				}
			}
			break;
		}
		case WM_MOUSEMOVE: {
			if (window) {
				const int x = GET_X_LPARAM(p_lParam);
				const int y = GET_Y_LPARAM(p_lParam);

				if (!window->GetHoveredFlag()) {
					TRACKMOUSEEVENT tme = {};
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hwnd;
					TrackMouseEvent(&tme);

					window->SetHoveredFlag(true);
					
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
					LogProcessor::GlasswareHoveredLog msg{};
					msg.glassware = window;
					msg.hovered = true;
					LogProcessor::CallLogProcessor(&msg);
#endif

					if (window->m_config.context.flags & CS_HLWA_GW_CSF_CONTEXTMENU) {
						g_destroyParentViaDestructor = false;
						window->TickContextWindow();
						g_destroyParentViaDestructor = true;
					}
				}

				const int dx = x - window->INPUTSTORAGE.m_mouseX;
				const int dy = y - window->INPUTSTORAGE.m_mouseY;

				window->InputMouseMove(x, y, GetKeyMods());
				window->InputMouseDelta(dx, dy, GetKeyMods());

				window->INPUTSTORAGE.m_mouseX = x;
				window->INPUTSTORAGE.m_mouseY = y;
			}
			break;
		}
		case WM_MOUSELEAVE: {
			if (window) {
				if (window->GetHoveredFlag()) {
					window->SetHoveredFlag(false);
					
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
					LogProcessor::GlasswareHoveredLog msg{};
					msg.glassware = window;
					msg.hovered = false;
					LogProcessor::CallLogProcessor(&msg);
#endif

					if (window->m_config.context.flags & CS_HLWA_GW_CSF_CONTEXTMENU) {
						g_destroyParentViaDestructor = false;
						window->TickContextWindow();
						g_destroyParentViaDestructor = true;
					}
				}
			}
			break;
		}
		case WM_MOUSEHOVER: {
			break;
		}
		case WM_CHAR:
		case WM_SYSCHAR: {
			if (!(p_wParam >= 0xd800 && p_wParam <= 0xdbff)) {
				uint32_t codepoint = 0;

				if (p_wParam > 0xdc00 && p_wParam <= 0xdfff) {
					if (window->WINDOWS.m_highSurrogate) {
						codepoint += (window->WINDOWS.m_highSurrogate - 0xd800) << 10;
						codepoint += (WCHAR)p_wParam - 0xdc00;
						codepoint += 0x10000;
					}
				} else codepoint = (WCHAR)p_wParam;

				window->WINDOWS.m_highSurrogate = 0;

				window->InputChar(codepoint, GetKeyMods());
			} else window->WINDOWS.m_highSurrogate = (WCHAR)p_wParam;

			if (p_uMsg == WM_SYSCHAR) break;

			return 0;

		}
		case WM_UNICHAR: {
			if (p_wParam == UNICODE_NOCHAR) {
				return true;
			}

			window->InputChar((uint32_t)p_wParam, GetKeyMods());

			return 0;
		}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP: {
			int scancode = 0;
			int keycode = 0;
			
			const int action = (HIWORD(p_lParam) & KF_UP) ? CS_HLWA_GW_A_RELEASE : CS_HLWA_GW_A_PRESS;
			const int mods = GetKeyMods();

			scancode = (HIWORD(p_lParam) & (KF_EXTENDED | 0xff));
			if (!scancode) scancode = KeycodeToScancode((uint32_t)p_wParam);
			switch (scancode) {
			case 0x54: // Alt + Print Screen has different scancode.
				scancode = 0x137;
				break;
			case 0x146: // Alt + Pause has different scancode.
				scancode = 0x45;
				break;
			case 0x136: // CJK IME sets the extended bit for right Shift.
				scancode = 0x36;
				break;
			default:
				break;
			}

			keycode = ScancodeToKeycode(scancode);

			if (p_wParam == VK_CONTROL) {
				if (HIWORD(p_lParam) & KF_EXTENDED) keycode = CS_HLWA_GW_K_RCONTROL;
				else {
					MSG next;
					const DWORD time = GetMessageTime();

					if (PeekMessage(&next, NULL, 0, 0, PM_NOREMOVE)) {
						if (next.message == WM_KEYDOWN
							|| next.message == WM_SYSKEYDOWN
							|| next.message == WM_KEYUP
							|| next.message == WM_SYSKEYUP ) {
							if (next.wParam == VK_MENU
								&& (HIWORD(next.lParam) & KF_EXTENDED)
								&& next.time == time) {
								break;
							}
						}
					}
				}
			} else if (p_wParam == VK_PROCESSKEY) break;

			if (action == CS_HLWA_GW_A_RELEASE && p_wParam == VK_SHIFT) { // Release both Shift keys on Shift up event, as both are pressed the first release does not emit any event.
				window->InputKey(scancode, CS_HLWA_GW_K_LSHIFT, action, mods);
			} else if (p_wParam == VK_SNAPSHOT) { // Key down is not reported for Print Screen key
				window->InputKey(scancode, keycode, CS_HLWA_GW_A_PRESS, mods);
				window->InputKey(scancode, keycode, CS_HLWA_GW_A_RELEASE, mods);
			} else window->InputKey(scancode, keycode, action, mods);

			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP: {
			int i = 0;
			int button = 0;
			int action = 0;

			if (p_uMsg == WM_LBUTTONDOWN || p_uMsg == WM_LBUTTONUP) button = CS_HLWA_GW_MBTN_1;
			else if (p_uMsg == WM_RBUTTONDOWN || p_uMsg == WM_RBUTTONUP) button = CS_HLWA_GW_MBTN_2;
			else if (p_uMsg == WM_MBUTTONDOWN || p_uMsg == WM_MBUTTONUP) button = CS_HLWA_GW_MBTN_3;
			else if (GET_XBUTTON_WPARAM(p_wParam) == XBUTTON1) button = CS_HLWA_GW_MBTN_4;
			else button = CS_HLWA_GW_MBTN_5;

			if (p_uMsg == WM_LBUTTONDOWN
				|| p_uMsg == WM_RBUTTONDOWN
				|| p_uMsg == WM_MBUTTONDOWN
				|| p_uMsg == WM_XBUTTONDOWN) action = CS_HLWA_GW_A_PRESS;
			else action = CS_HLWA_GW_A_RELEASE;

			for (i = 0; i <= CS_HLWA_GW_MBTN_LAST; i++) if (window->INPUTSTORAGE.m_mouseButtons[i] == CS_HLWA_GW_A_PRESS) break;

			if (i > CS_HLWA_GW_MBTN_LAST) SetCapture(hwnd);

			window->InputMouseButton(button, action, GetKeyMods());

			for (i = 0; i <= CS_HLWA_GW_MBTN_LAST; i++) if (window->INPUTSTORAGE.m_mouseButtons[i] == CS_HLWA_GW_A_PRESS) break;

			if (i > CS_HLWA_GW_MBTN_LAST) ReleaseCapture();

			if (p_uMsg == WM_XBUTTONDOWN || p_uMsg == WM_XBUTTONUP) return true;

			return 0;
		}
		case WM_MOUSEWHEEL:
			window->InputMouseScroll(0.0, (SHORT)HIWORD(p_wParam) / (double)WHEEL_DELTA, GetKeyMods());
			return 0;
		case WM_MOUSEHWHEEL:
			window->InputMouseScroll(-((SHORT)HIWORD(p_wParam) / (double)WHEEL_DELTA), 0.0, GetKeyMods());
			return 0;
		case WM_DROPFILES: {
			if (!window->m_config.context.flags & CS_HLWA_GW_CSF_ENABLEFILEDROP) break;
			HDROP drop = (HDROP)p_wParam;
			POINT pt;
			int i;
			
			const int count = DragQueryFile(drop, 0xffffffff, nullptr, 0);
			char** paths = (char**)malloc(count * sizeof(char*));

			DragQueryPoint(drop, &pt);

			const int dx = pt.x - window->INPUTSTORAGE.m_mouseX;
			const int dy = pt.y - window->INPUTSTORAGE.m_mouseY;

			window->InputMouseMove(pt.x, pt.y, GetKeyMods());
			window->InputMouseDelta(dx, dy, GetKeyMods());

			window->INPUTSTORAGE.m_mouseX = pt.x;
			window->INPUTSTORAGE.m_mouseY = pt.y;

			for (i = 0; i < count; i++) {
				const uint32_t len = DragQueryFile(drop, i, nullptr, 0);
#if defined(CS_HLWA_USEUTF8STRINGS)
				char* buffer = (char*)malloc((len + 1) * sizeof(char));
#else
				wchar_t* buffer = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
#endif

				DragQueryFile(drop, i, buffer, len + 1);
#if defined(CS_HLWA_USEUTF8STRINGS)
				paths[i] = buffer;
#else
				paths[i] = w2u(buffer);
#endif

				free(buffer);
			}

			assert(count >= 0);

			const char** const_paths = (const char**)malloc(count * sizeof(const char*));
			for (uint32_t i = 0; i < count; i++) const_paths[i] = paths[i];
			
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
			LogProcessor::GlasswareFileDropLog msg{};
			msg.glassware = window;
			msg.posX = window->INPUTSTORAGE.m_mouseX;
			msg.posY = window->INPUTSTORAGE.m_mouseY;
			msg.paths = const_paths;
			msg.pathCount = count;
			LogProcessor::CallLogProcessor(&msg);
#endif

			for (i = 0; i < count; i++) free(paths[i]);
			free(paths);
			free(const_paths);

			DragFinish(drop);

			return 0;
		}
		default:
			break;
		}
		
		return DefWindowProc(hwnd, p_uMsg, p_wParam, p_lParam);
	}
#endif
	void Glassware::InputChar(uint32_t p_codepoint, CS_HLWA_GW_M p_mods) {
		assert(p_mods == (p_mods & CS_HLWA_GW_M_MOD_MASK));

		if (p_codepoint < 32 || (p_codepoint > 126 && p_codepoint < 160)) return;
		if (INPUTSTORAGE.m_lockKeyMods) p_mods &= ~(CS_HLWA_GW_M_CAPSLOCK | CS_HLWA_GW_M_NUMLOCK);
		IC.m_charCall(this, p_codepoint, p_mods);
	}
	void Glassware::InputKey(int p_scancode, CS_HLWA_GW_K p_keycode, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods) {
		assert(p_keycode >= 0 || p_keycode == -1);
		assert(p_keycode <= CS_HLWA_GW_K_LAST);
		assert(p_action == CS_HLWA_GW_A_PRESS | p_action == CS_HLWA_GW_A_RELEASE);
		assert(p_mods == (p_mods & CS_HLWA_GW_M_MOD_MASK));

		if (p_keycode >= 0 && p_keycode <= CS_HLWA_GW_K_LAST) {
			bool repeated = false;

			if (p_action == CS_HLWA_GW_A_RELEASE && INPUTSTORAGE.m_keys[p_keycode] == CS_HLWA_GW_A_RELEASE) return;
			if (p_action == CS_HLWA_GW_A_PRESS && INPUTSTORAGE.m_keys[p_keycode] == CS_HLWA_GW_A_PRESS) repeated = true;
			if (p_action == CS_HLWA_GW_A_RELEASE && INPUTSTORAGE.m_stickyKeys) INPUTSTORAGE.m_keys[p_keycode] = I_CS_HLWA_GW_A_STICK;
			else INPUTSTORAGE.m_keys[p_keycode] = (char)p_action;

			if (repeated) p_action = CS_HLWA_GW_A_ROLLOVER;
		}
		
		if (INPUTSTORAGE.m_lockKeyMods) p_mods &= ~(CS_HLWA_GW_M_CAPSLOCK | CS_HLWA_GW_M_NUMLOCK);

		IC.m_keyCall(this, p_scancode, p_keycode, p_action, p_mods);
	}
	void Glassware::InputMouseButton(CS_HLWA_GW_MBTN p_button, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods) {
		assert(p_button >= 0);
		assert(p_action == CS_HLWA_GW_A_PRESS | p_action == CS_HLWA_GW_A_RELEASE);
		assert(p_mods == (p_mods & CS_HLWA_GW_M_MOD_MASK));

		if (p_button < 0) return;
		if (INPUTSTORAGE.m_lockKeyMods) p_mods &= ~(CS_HLWA_GW_M_CAPSLOCK | CS_HLWA_GW_M_NUMLOCK);
		if (p_button <= CS_HLWA_GW_MBTN_LAST) {
			if (p_action == CS_HLWA_GW_A_RELEASE && INPUTSTORAGE.m_stickyMouse) INPUTSTORAGE.m_mouseButtons[p_button] = I_CS_HLWA_GW_A_STICK;
			else INPUTSTORAGE.m_mouseButtons[p_button] = (char)p_button;
		}

		IC.m_mouseBtnCall(this, p_button, p_action, p_mods);
	}
	void Glassware::InputMouseScroll(double p_xoffset, double p_yoffset, CS_HLWA_GW_M p_mods) {
		assert(p_xoffset > -FLT_MAX);
		assert(p_xoffset < FLT_MAX);
		assert(p_yoffset > -FLT_MAX);
		assert(p_yoffset < FLT_MAX);

		IC.m_mouseScrCall(this, p_xoffset, p_yoffset, p_mods);
	}
	void Glassware::InputMouseMove(double p_posx, double p_posy, CS_HLWA_GW_M p_mods) {
		assert(p_posx > -FLT_MAX);
		assert(p_posy < FLT_MAX);

		IC.m_mouseMoveCall(this, p_posx, p_posy, p_mods);
	}
	void Glassware::InputMouseDelta(double p_dx, double p_dy, CS_HLWA_GW_M p_mods) {
		assert(p_dx > -FLT_MAX);
		assert(p_dy < FLT_MAX);

		IC.m_mouseDelCall(this, p_dx, p_dy, p_mods);
	}

#if defined(_WIN32)
	Glassware::Glassware(void* p_hwnd) {
		//
	}
#endif
	Glassware::Glassware(const GlasswareCreateInfoConfig& p_config) : m_config{ p_config } {
		g_windows.push_back(this);
		if (m_config.parent != nullptr) m_config.parent->m_children.push_back(this);

#if defined(_WIN32)
		BOOL sc;
		DwmIsCompositionEnabled(&sc);
		m_flags |= CS_HLWA_GW_F_SUPPORTSCOMPOSITION;

		int posX = 0;
		int posY = 0;
		Utils::GetPositionConversion(
#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
			DisplayMesh::DisplayMesh::GetDisplayFromCursor(),
#else
			this
#endif
			m_config.position,
			m_config.offsetX,
			m_config.offsetY,
			posX,
			posY
		);

		g_tempInitWindow = this;
		
		WNDCLASSEX wc = { 0 };
		if (!GetClassInfoEx(GetModuleHandle(nullptr), m_config.className.c_str(), &wc)) {
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.lpszClassName = m_config.className.c_str();
			wc.hInstance = GetModuleHandle(nullptr);
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
			wc.lpfnWndProc = reinterpret_cast<WNDPROC>(WindowProc);

			bool hasCustomIcon = false;
			HICON customIcon;
			if (m_config.iconSpecifier.type == IconSpecifierType::FILE) {
				if (m_config.iconSpecifier.data.file.path != CS_HLWA_DEFAULTENTRY) {
					customIcon = (HICON)LoadImage(
						nullptr,
						m_config.iconSpecifier.data.file.path.c_str(),
						IMAGE_ICON,
						0,
						0,
						LR_LOADFROMFILE
					);
					hasCustomIcon = true;
				}
			} else {
				if (m_config.iconSpecifier.data.buffer.data != nullptr
					&& m_config.iconSpecifier.data.buffer.dataLength > 0
					&& m_config.iconSpecifier.data.buffer.width > 0
					&& m_config.iconSpecifier.data.buffer.height > 0) {
					customIcon = CreateIconFromResourceEx(
						m_config.iconSpecifier.data.buffer.data,
						(DWORD)m_config.iconSpecifier.data.buffer.dataLength,
						true,
						0x0030000,
						m_config.iconSpecifier.data.buffer.width,
						m_config.iconSpecifier.data.buffer.height,
						LR_DEFAULTCOLOR
					);
					hasCustomIcon = true;
				}
			}

			wc.hIcon = hasCustomIcon ? customIcon : LoadIcon(nullptr, IDI_APPLICATION);
			RegisterClassEx(&wc);
		}

		WINDOWS.g_windowClasses[m_config.className]++;
		
		WINDOWS.m_handle = CreateWindowEx(
			0, //WS_EX_NOREDIRECTIONBITMAP
			m_config.className.c_str(),
			m_config.title.c_str(),
			WS_OVERLAPPEDWINDOW & ~WS_OVERLAPPED,
			posX,
			posY,
			m_config.sizeX,
			m_config.sizeY,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this
		);
		SetProp((HWND)WINDOWS.m_handle, CS_HLWA_STRVAL("GLASSWARE"), this);

		if (m_config.context.flags & CS_HLWA_GW_CSF_CONTEXTMENU) {
			Hide();
			// TODO: Maybe just move this into the create window function?
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
		}
		Show();

		if (m_config.context.flags & CS_HLWA_GW_CSF_ALWAYSONTOP) {
			SetWindowPos((HWND)WINDOWS.m_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}
		
		// Apparently this cannot be in the CreateWindowEx call because it'll fuck up the titlebar... why?
		SetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE, GetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE) | WS_EX_LAYERED);// | WS_EX_NOREDIRECTIONBITMAP);
		SetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		
		WINDOWS.m_windowedStyle = GetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE);
		WINDOWS.m_fullscreenStyle = GetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE) | WS_POPUP;
		WINDOWS.m_fullscreenBorderlessStyle = (GetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW) | WS_POPUP;
		WINDOWS.m_windowedExStyle = GetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE);
		WINDOWS.m_fullscreenExStyle = GetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE) | WS_EX_APPWINDOW;
		WINDOWS.m_fullscreenBorderlessExStyle = GetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE);
		
		if (SupportsComposition() && m_config.context.flags & CS_HLWA_GW_CSF_TRANSPARENT) {
			HRGN region = CreateRectRgn(0, 0, -1, -1);
			DWM_BLURBEHIND bb = { 0 };								// No this does not enable blur behind; this is for transparent; acrylic is located in the accents.
			bb.dwFlags = DWM_BB_ENABLE;// | DWM_BB_BLURREGION;
			bb.hRgnBlur = region;
			bb.fEnable = true;
			DwmEnableBlurBehindWindow((HWND)WINDOWS.m_handle, &bb);	// Enable transparency (vk only).
			DeleteObject(region);
		} else if (!SupportsComposition()) {
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
			LogProcessor::OutputLog msg{};
			msg.outputType = LogProcessor::OutputType::ERR;
			msg.value = CS_HLWA_STRVAL("System does not support composition.");
			LogProcessor::CallLogProcessor(&msg);
#endif
		}
		
		auto original_proc = (WNDPROC)GetWindowLongPtr((HWND)WINDOWS.m_handle, GWLP_WNDPROC);
		(WNDPROC)SetWindowLongPtr((HWND)WINDOWS.m_handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));
		SetWindowPos((HWND)WINDOWS.m_handle, NULL, 0, 0, m_config.sizeX, m_config.sizeY, SWP_FRAMECHANGED | SWP_NOMOVE);

		m_dpi = GetDpiForWindow((HWND)WINDOWS.m_handle);
		ResetResizeFlag();
#endif

		SetMode(m_config.mode);
	}
	Glassware::~Glassware() {
		m_flags |= CS_HLWA_GW_F_DESTRUCTING;
		if (m_config.parent != nullptr) {
			auto it = std::find(m_config.parent->m_children.begin(), m_config.parent->m_children.end(), this);
			if (it != m_config.parent->m_children.end()) m_config.parent->m_children.erase(it);
			m_config.parent = nullptr;
		}
		auto tempChildren = std::move(m_children);
		for (auto* child : m_children) {
			delete child;
		}
		auto it = std::find(g_windows.begin(), g_windows.end(), this);
		if (it != g_windows.end()) g_windows.erase(it);
#if defined(_WIN32)
		//PostMessage(WINDOWS.handle, WM_CLOSE, 0, 0);
		if ((HWND)WINDOWS.m_handle) {
			DestroyWindow((HWND)WINDOWS.m_handle);
			WINDOWS.m_handle = nullptr;
		}
		if (!(m_flags & CS_HLWA_GW_F_MANUALLYCREATED)) {
			WINDOWS.g_windowClasses[m_config.className.c_str()]--;
			if (WINDOWS.g_windowClasses[m_config.className.c_str()] == 0) {
				WINDOWS.g_windowClasses.erase(m_config.className.c_str());
				UnregisterClass(m_config.className.c_str(), GetModuleHandle(nullptr));
			}
		}
#endif
	}
	
	CS_HLWA_R Glassware::Poll() {
#if defined(_WIN32)
		MSG msg = { 0 };
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (g_windows.size() == 0) return false;

		for (int i = g_windows.size() - 1; i >= 0; i--) {
			if (g_windows[i]->m_flags & CS_HLWA_GW_F_CLOSE) delete g_windows[i];
			if (i >= g_windows.size()) i = g_windows.size() - 1;
		}

		HWND hndl = GetActiveWindow();
		if (hndl) {
			Glassware* win = (Glassware*)GetProp(hndl, CS_HLWA_STRVAL("GLASSWARE"));
			if (win) {
				int i;
				const int keys[4][2] = {
					{ VK_LSHIFT, CS_HLWA_GW_K_LSHIFT },
					{ VK_RSHIFT, CS_HLWA_GW_K_RSHIFT },
					{ VK_LWIN, CS_HLWA_GW_K_LSUPER },
					{ VK_RWIN, CS_HLWA_GW_K_RSUPER }
				};

				for (i = 0; i < 4; i++) {
					const int vk = keys[i][0];
					const int keycode = keys[i][1];
					const int scancode = KeycodeToScancode(keycode);

					if (GetKeyState(vk) & 0x8000) continue;
					if (win->INPUTSTORAGE.m_keys[keycode] != CS_HLWA_GW_A_PRESS) continue;

					win->InputKey(scancode, keycode, CS_HLWA_GW_A_RELEASE, GetKeyMods());
				}
			}
		}
#endif
		return CS_HLWA_R_SUCCESS;
	}
	void Glassware::PreUpdateWindows() {
		for (int i = g_windows.size() - 1; i >= 0; i--) {
			auto* win = g_windows[i];
			if (win != nullptr) {
				win->OnPreUpdate();
			}
		}
	}
	void Glassware::PostUpdateWindows() {
		for (int i = g_windows.size() - 1; i >= 0; i--) {
			auto* win = g_windows[i];
			if (win != nullptr) {
				win->OnPostUpdate();
			}
		}
	}

	const Glassware* Glassware::GetParent() const {
		return m_config.parent;
	};
	const std::vector<Glassware*> Glassware::GetChildren() {
		return m_children;
	};

	void Glassware::SetUserPointer(void* p_userPtr) {
		m_userPtr = p_userPtr;
	};
	const void* Glassware::GetUserPointer() const {
		return m_userPtr;
	};
		
	void Glassware::SetTitle(const CS_HLWA_STRING& p_title) {
		m_config.title = p_title;
		
#if defined(_WIN32)
		SetWindowText((HWND)WINDOWS.m_handle, p_title.c_str());
#endif
	}
	void Glassware::Hide() {
#if defined(_WIN32)
		ShowWindow((HWND)WINDOWS.m_handle, SW_HIDE);
#endif
	}
	void Glassware::Show() {
#if defined(_WIN32)
		ShowWindow((HWND)WINDOWS.m_handle, SW_SHOW);
#endif
	}
	void Glassware::Minimize() {
#if defined(_WIN32)
		ShowWindow((HWND)WINDOWS.m_handle, SW_MINIMIZE);
#endif
	}
	void Glassware::Restore() {
#if defined(_WIN32)
		switch (GetMode()) {
		case ModeState::WINDOWED:
			break;
		case ModeState::FULLSCREEN:
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE, WINDOWS.m_windowedStyle);
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE, WINDOWS.m_windowedExStyle);
			ChangeDisplaySettings(nullptr, 0);
			break;
		case ModeState::FULLSCREEN_BORDERLESS:
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE, WINDOWS.m_windowedStyle);
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE, WINDOWS.m_windowedExStyle);
			SetWindowPos(
				(HWND)WINDOWS.m_handle,
				HWND_TOP,
				WINDOWS.m_unfullscreenRectLeft,
				WINDOWS.m_unfullscreenRectTop,
				WINDOWS.m_unfullscreenRectRight - WINDOWS.m_unfullscreenRectLeft,
				WINDOWS.m_unfullscreenRectBottom - WINDOWS.m_unfullscreenRectTop,
				SWP_FRAMECHANGED
			);
			break;
		}

		ShowWindow((HWND)WINDOWS.m_handle, SW_RESTORE);
#endif
	}
	void Glassware::Maximize() {
#if defined(_WIN32)
		ShowWindow((HWND)WINDOWS.m_handle, SW_MAXIMIZE);
#endif
	}
	CS_HLWA_R Glassware::SetMode(ModeState p_mode) {
		Restore();
#if defined(_WIN32)
		switch (p_mode) {
		case ModeState::WINDOWED:
			break;
		case ModeState::FULLSCREEN: {
			RECT rect;
			GetClientRect((HWND)WINDOWS.m_handle, &rect);
			WINDOWS.m_unfullscreenRectLeft = rect.left;
			WINDOWS.m_unfullscreenRectTop = rect.top;
			WINDOWS.m_unfullscreenRectRight = rect.right;
			WINDOWS.m_unfullscreenRectBottom = rect.bottom;
			
#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
			auto display = DisplayMesh::DisplayMesh::GetDisplayFromGlassware(this);
			HMONITOR monitorHandle = (HMONITOR)display.m_handle;
#else
			HMONITOR monitorHandle = MonitorFromWindow((HWND)WINDOWS.m_handle);
#endif
			MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
			GetMonitorInfo(monitorHandle, &monitorInfo);
			DEVMODE dm;
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
			dm.dmPelsWidth = display.m_monitorSize.m_sizeX;
			dm.dmPelsHeight = display.m_monitorSize.m_sizeY;
			dm.dmBitsPerPel = display.m_bitsPerPixel;
			dm.dmDisplayFrequency = display.m_frequency;	
#else
			MONITORINFOEX mi;
			mi.cbSize = sizeof(MONITORINFOEX);
			if (GetMonitorInfo(monitorHandle, &mi)) {
				DEVMODE dm2;
				dm2.dmSize = sizeof(dm2);
				if (EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm2)) {
					dm.dmPelsWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
					dm.dmPelsHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
					dm.dmBitsPerPel = dm2.dmBitsPerPel;
					dm.dmDisplayFrequency = dm2.dmDisplayFrequency;
				}
			}
#endif
			dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
			
			SetWindowLongPtr((HWND)WINDOWS.m_handle, GWL_STYLE, WINDOWS.m_fullscreenStyle);
			SetWindowLongPtr((HWND)WINDOWS.m_handle, GWL_EXSTYLE, WINDOWS.m_fullscreenExStyle);
			
			SetWindowPos((HWND)WINDOWS.m_handle, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, dm.dmPelsWidth, dm.dmPelsHeight, SWP_SHOWWINDOW);
			ChangeDisplaySettings(&dm, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
			
			if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
				Restore();
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
				LogProcessor::OutputLog msg{};
				msg.outputType = LogProcessor::OutputType::ERR;
				msg.value = CS_HLWA_STRVAL("Error occurred during attempt to change display settings; restoring.");
				LogProcessor::CallLogProcessor(&msg);
#endif
				return false;
			}
			
			Maximize();

			break;
		}
		case ModeState::FULLSCREEN_BORDERLESS: {
			RECT rect;
			GetClientRect((HWND)WINDOWS.m_handle, &rect);
			WINDOWS.m_unfullscreenRectLeft = rect.left;
			WINDOWS.m_unfullscreenRectTop = rect.top;
			WINDOWS.m_unfullscreenRectRight = rect.right;
			WINDOWS.m_unfullscreenRectBottom = rect.bottom;
			
#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
			HMONITOR monitorHandle = (HMONITOR)DisplayMesh::DisplayMesh::GetDisplayFromGlassware(this).m_handle;
#else
			HMONITOR monitorHandle = MonitorFromWindow((HWND)WINDOWS.m_handle);
#endif
			MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
			
			if (!GetMonitorInfo(monitorHandle, &monitorInfo)) {
				Restore();
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
				LogProcessor::OutputLog msg{};
				msg.outputType = LogProcessor::OutputType::ERR;
				msg.value = CS_HLWA_STRVAL("Error occurred during attempt to change display settings; restoring.");
				LogProcessor::CallLogProcessor(&msg);
#endif
				return false;
			}
			
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE, WINDOWS.m_fullscreenBorderlessStyle);
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE, WINDOWS.m_fullscreenBorderlessExStyle);

			SetWindowPos(
				(HWND)WINDOWS.m_handle,
				HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
			);

			Maximize();

			break;
		}
		}
#endif
		return CS_HLWA_R_SUCCESS;
	}
	void Glassware::Close() {
		m_flags |= CS_HLWA_GW_F_CLOSE;
	}
	void* Glassware::GetHandle() {
		return WINDOWS.m_handle;
	}

	bool Glassware::Minimized() const {
#if defined(_WIN32)
		return IsIconic((HWND)WINDOWS.m_handle);
#endif
	}
	bool Glassware::Maximized() const {
#if defined(_WIN32)
		return IsZoomed((HWND)WINDOWS.m_handle);
#endif
	}
	ModeState Glassware::GetMode() const {
#if defined(_WIN32)
		RECT windowRect;
		if (!GetWindowRect((HWND)WINDOWS.m_handle, &windowRect)) {
			return ModeState::WINDOWED;
		}

		HMONITOR monitor = MonitorFromWindow((HWND)WINDOWS.m_handle, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
		if (!GetMonitorInfo(monitor, &monitorInfo)) {
			return ModeState::WINDOWED;
		}
		RECT monitorRect = monitorInfo.rcMonitor;

		LONG style = GetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE);

		if ((style & WS_POPUP) != 0 &&
			windowRect.right - windowRect.left == monitorRect.right - monitorRect.left &&
			windowRect.bottom - windowRect.top == monitorRect.bottom - monitorRect.top) {
			return ModeState::FULLSCREEN;
		}

		if ((style & WS_OVERLAPPEDWINDOW) == 0 && 
			windowRect.left == monitorRect.left &&
			windowRect.top == monitorRect.top &&
			windowRect.right == monitorRect.right &&
			windowRect.bottom == monitorRect.bottom) {
			return ModeState::FULLSCREEN_BORDERLESS;
		}

		return ModeState::WINDOWED;
#endif
	}

	int Glassware::GetPositionX() const {
		return m_posX;
	}
	int Glassware::GetPositionY() const {
		return m_posY;
	}
	void Glassware::GetPositions(int& p_outX, int& p_outY) const {
		p_outX = m_posX;
		p_outY = m_posY;
	}
	uint32_t Glassware::GetSizeX() const {
		return m_sizeX;
	}
	uint32_t Glassware::GetSizeY() const {
		return m_sizeY;
	}
	void Glassware::GetSizes(uint32_t& p_outX, uint32_t& p_outY) const {
		p_outX = m_sizeX;
		p_outY = m_sizeY;
	}
	uint32_t Glassware::GetClientSizeX() const {
		return m_cliX;
	}
	uint32_t Glassware::GetClientSizeY() const {
		return m_cliY;
	}
	void Glassware::GetClientSizes(uint32_t& p_outX, uint32_t& p_outY) const {
		p_outX = m_cliX;
		p_outY = m_cliY;
	}
	void Glassware::SetPositions(PositionState p_state, int p_offsetX, int p_offsetY) {
		int posX = 0;
		int posY = 0;
#if defined(CS_HLWA_E_DISPLAYMESH) || defined(CS_HLWA_E_ALL)
		Utils::GetPositionConversion(DisplayMesh::DisplayMesh::GetDisplayFromGlassware(this), p_state, p_offsetX, p_offsetY, posX, posY);
#else
		Utils::GetPositionConversion(this, p_state, p_offsetX, p_offsetY, posX, posY);
#endif
#if defined(_WIN32)
		SetWindowPos(
			(HWND)WINDOWS.m_handle,
			HWND_TOP,
			posX,
			posY,
			0,
			0,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE
		);
#endif
	}
	void Glassware::SetSizes(uint32_t p_sizeX, uint32_t p_sizeY) {
#if defined(_WIN32)
		SetWindowPos(
			(HWND)WINDOWS.m_handle,
			HWND_TOP,
			0,
			0,
			p_sizeX,
			p_sizeY,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE
		);
#endif
	}

	void Glassware::SetCharCallback(void(*p_callback)(Glassware* p_glassware, uint32_t p_codepoint, CS_HLWA_GW_M p_mods)) {
		IC.m_charCall = p_callback;
	}
	void Glassware::SetKeyCallback(void(*p_callback)(Glassware* p_glassware, int p_scancode, CS_HLWA_GW_K p_keycode, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods)) {
		IC.m_keyCall = p_callback;
	}
	void Glassware::SetMouseButtonCallback(void(*p_callback)(Glassware* p_glassware, CS_HLWA_GW_MBTN p_button, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods)) {
		IC.m_mouseBtnCall = p_callback;
	}
	void Glassware::SetMouseScrollCallback(void(*p_callback)(Glassware* p_glassware, double p_xoffset, double p_yoffset, CS_HLWA_GW_M p_mods)) {
		IC.m_mouseScrCall = p_callback;
	}
	void Glassware::SetMouseMoveCallback(void(*p_callback)(Glassware* p_glassware, double p_posx, double p_posy, CS_HLWA_GW_M p_mods)) {
		IC.m_mouseMoveCall = p_callback;
	}
	void Glassware::SetMouseDeltaCallback(void(*p_callback)(Glassware* p_glassware, double p_dx, double p_dy, CS_HLWA_GW_M p_mods)) {
		IC.m_mouseDelCall = p_callback;
	}
	bool Glassware::GetMousePosition(double& p_outX, double& p_outY) {
		p_outX = INPUTSTORAGE.m_mouseX;
		p_outY = INPUTSTORAGE.m_mouseY;
		return true;
	}
	void Glassware::GetGamepadState(int p_gid, GamepadState* p_state) {
		assert(p_gid >= CS_HLWA_GW_GAMEPAD_1);
		assert(p_gid <= CS_HLWA_GW_GAMEPAD_LAST);
		assert(p_state != nullptr);

		p_state->Clear();

		if (p_gid <= CS_HLWA_GW_GAMEPAD_1 || p_gid >= CS_HLWA_GW_GAMEPAD_LAST) return;

		// TODO: Add implementation for GetGamepadState.
	}

	void Glassware::SetInputMode(int p_major, int p_minor) {
		//
	}
	const Glassware::InputCallbacks& Glassware::GetInputCallbacks() const {
		return IC;
	}
	CS_HLWA_GW_M Glassware::GetKeyMods() {
		CS_HLWA_GW_M mods = 0;
		
#if defined(_WIN32)
		if (GetKeyState(VK_SHIFT) & 0x8000)
			mods |= CS_HLWA_GW_M_SHIFT;
		if (GetKeyState(VK_CONTROL) & 0x8000)
			mods |= CS_HLWA_GW_M_CONTROL;
		if (GetKeyState(VK_MENU) & 0x8000)
			mods |= CS_HLWA_GW_M_ALT;
		if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
			mods |= CS_HLWA_GW_M_SUPER;
		if (GetKeyState(VK_CAPITAL) & 1)
			mods |= CS_HLWA_GW_M_CAPSLOCK;
		if (GetKeyState(VK_NUMLOCK) & 1)
			mods |= CS_HLWA_GW_M_NUMLOCK;
#endif

		return mods;
	}
	CS_HLWA_GW_K Glassware::ScancodeToKeycode(int p_scancode) {
#if defined(_WIN32)
		return MapVirtualKey(p_scancode, MAPVK_VSC_TO_VK);//Ex += GetKeyboardLayout(0));
#endif
	}
	int Glassware::KeycodeToScancode(CS_HLWA_GW_K p_keycode) {
#if defined(_WIN32)
		return MapVirtualKey(p_keycode, MAPVK_VK_TO_VSC);//Ex += GetKeyboardLayout(0));
#endif
	}
	std::string Glassware::GetKeycodeDisplayName(CS_HLWA_GW_K p_keycode) {
#if defined(_WIN32)
		LONG scancode = MapVirtualKey(p_keycode, MAPVK_VK_TO_VSC);
		char name[256];
		GetKeyNameTextA(scancode << 16, name, 256);
		return std::string(name);
#endif
	}

	void Glassware::SetCursor() {
		//
	}
	void Glassware::BindCursorToWindow() {
		//
	}
	void Glassware::UnbindCursorFromWindow() {
		//
	}
	void Glassware::BindCursorToAbsoluteArea() {
		//
	}
	void Glassware::UnbindCursorFromAbsoluteArea() {
		//
	}
	void Glassware::BindCursorToRelativeArea() {
		//
	}
	void Glassware::UnbindCursorFromRelativeArea() {
		//
	}

	uint32_t Glassware::GetDPI() {
		return m_dpi;
	}
	float Glassware::GetDPIScaleFactor() {
		return m_dpi / 96.0f;
	}
	float Glassware::GetDPIScaleFactor(uint32_t p_dpi) {
		return p_dpi / 96.0f;
	}

	CS_HLWA_R Glassware::SetContextFlag(CS_HLWA_GW_CSF p_flag, bool p_value) {
		CS_HLWA_GW_CSF tempFlags = m_config.context.flags;
		tempFlags = (tempFlags & ~p_flag) | (-int(p_value) & p_flag);

		if (tempFlags == m_config.context.flags) {
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
			LogProcessor::OutputLog msg{};
			msg.outputType = LogProcessor::OutputType::WARN;
			msg.value = CS_HLWA_STRVAL("Attempting to set context flag with the same value; ignoring.");
			LogProcessor::CallLogProcessor(&msg);
#endif
			return CS_HLWA_R_SUCCESS;
		}
		m_config.context.flags = tempFlags;

		switch (p_flag) {
		case(CS_HLWA_GW_CSF_DISABLETITLEBAR): {
			m_config.context.flags = (m_config.context.flags & ~CS_HLWA_GW_CSF_DISABLETITLEBAR) | (-int(p_value) & CS_HLWA_GW_CSF_DISABLETITLEBAR);
			SetWindowPos((HWND)WINDOWS.m_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_DRAWFRAME);
			break;
		}
		case(CS_HLWA_GW_CSF_TRANSPARENT): {
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
			LogProcessor::OutputLog msg{};
			msg.outputType = LogProcessor::OutputType::ERR;
			msg.value = CS_HLWA_STRVAL("Cannot change transparent context flag after creation; incompatible with DX12.");
			LogProcessor::CallLogProcessor(&msg);
#endif
			return CS_HLWA_R_INVALID;
		}
		case(CS_HLWA_GW_CSF_CONTEXTMENU): {
			m_config.context.flags = (m_config.context.flags & ~CS_HLWA_GW_CSF_CONTEXTMENU) | (-int(p_value) & CS_HLWA_GW_CSF_CONTEXTMENU);
			if (p_value) UpdateWindowStyles(0, 0, WS_EX_TOOLWINDOW, 0);
			else UpdateWindowStyles(0, 0, 0, WS_EX_TOOLWINDOW);
			break;
		}
		case(CS_HLWA_GW_CSF_ENABLEFILEDROP): {
			m_config.context.flags = (m_config.context.flags & ~CS_HLWA_GW_CSF_ENABLEFILEDROP) | (-int(p_value) & CS_HLWA_GW_CSF_ENABLEFILEDROP);
			break;
		}
		case(CS_HLWA_GW_CSF_ALWAYSONTOP): {
			m_config.context.flags = (m_config.context.flags & ~CS_HLWA_GW_CSF_ALWAYSONTOP) | (-int(p_value) & CS_HLWA_GW_CSF_ALWAYSONTOP);
			if (p_value) SetWindowPos((HWND)WINDOWS.m_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			else SetWindowPos((HWND)WINDOWS.m_handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			break;
		}
		default:
#if defined(CS_HLWA_E_LOGPROCESSOR) || defined(CS_HLWA_E_ALL)
			LogProcessor::OutputLog msg{};
			msg.outputType = LogProcessor::OutputType::WARN;
			msg.value = CS_HLWA_STRVAL("Invalid p_flag parameter in `SetContextFlag` call.");
			LogProcessor::CallLogProcessor(&msg);
#endif
			break;
		}

		return CS_HLWA_R_SUCCESS;
	}
	bool Glassware::GetContextFlag(CS_HLWA_GW_CSF p_flag) const {
		return m_config.context.flags & p_flag;
	}
	void Glassware::SetPositionSnapping(uint32_t p_value) {
		m_config.context.positionSnapping = p_value;
	}
	uint32_t Glassware::GetPositionSnapping() const {
		return m_config.context.positionSnapping;
	}
	void Glassware::SetSizeSnapping(uint32_t p_value) {
		m_config.context.sizeSnapping = p_value;
	}
	uint32_t Glassware::GetSizeSnapping() const {
		return m_config.context.sizeSnapping;
	}

	bool Glassware::SupportsComposition() const {
		return m_flags & CS_HLWA_GW_F_SUPPORTSCOMPOSITION;
	}

	void Glassware::ForceTriggerResizeFlag() {
		m_flags |= CS_HLWA_GW_F_RESIZEFLAG;
	}
	void Glassware::ResetResizeFlag() {
		m_flags &= ~CS_HLWA_GW_F_RESIZEFLAG;
	}
	bool Glassware::GetResizeFlag() const {
		return m_flags & CS_HLWA_GW_F_RESIZEFLAG;
	}

	void Glassware::SetHoveredFlag(bool p_value) {
		m_flags = (m_flags & ~CS_HLWA_GW_F_HOVEREDFLAG) | (-int(p_value) & CS_HLWA_GW_F_HOVEREDFLAG);
	}
	bool Glassware::GetHoveredFlag() const {
		return m_flags & CS_HLWA_GW_F_HOVEREDFLAG;
	}
	void Glassware::SetFocusedFlag(bool p_value) {
		m_flags = (m_flags & ~CS_HLWA_GW_F_FOCUSEDFLAG) | (-int(p_value) & CS_HLWA_GW_F_FOCUSEDFLAG);
	}
	bool Glassware::GetFocusedFlag() const {
		return m_flags & CS_HLWA_GW_F_FOCUSEDFLAG;
	}

#if defined(_WIN32)
	void Glassware::UpdateWindowStyles(int p_add, int p_remove, int p_exadd, int p_exremove) {
		WINDOWS.m_windowedStyle = (WINDOWS.m_windowedStyle & ~p_remove) | p_add; 
		WINDOWS.m_fullscreenStyle = (WINDOWS.m_fullscreenStyle & ~p_remove) | p_add;
		WINDOWS.m_fullscreenBorderlessStyle = (WINDOWS.m_fullscreenBorderlessStyle & ~p_remove) | p_add;
		WINDOWS.m_windowedExStyle = (WINDOWS.m_windowedExStyle & ~p_exremove) | p_exadd;
		WINDOWS.m_fullscreenExStyle = (WINDOWS.m_fullscreenExStyle & ~p_exremove) | p_exadd;
		WINDOWS.m_fullscreenBorderlessExStyle = (WINDOWS.m_fullscreenBorderlessExStyle & ~p_exremove) | p_exadd;

		auto mode = GetMode();
		switch (mode) {
		case(ModeState::WINDOWED):
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE, WINDOWS.m_windowedStyle);
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE, WINDOWS.m_windowedExStyle);
			break;
		case(ModeState::FULLSCREEN):
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE, WINDOWS.m_fullscreenStyle);
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE, WINDOWS.m_fullscreenExStyle);
			break;
		case(ModeState::FULLSCREEN_BORDERLESS):
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_STYLE, WINDOWS.m_fullscreenBorderlessStyle);
			SetWindowLong((HWND)WINDOWS.m_handle, GWL_EXSTYLE, WINDOWS.m_fullscreenBorderlessExStyle);
			break;
		}
	}
#endif
}
#endif