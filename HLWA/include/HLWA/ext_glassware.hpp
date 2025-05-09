#pragma once

#if defined(CS_HLWA_E_GLASSWARE) || defined(CS_HLWA_E_ALL)

#include "types.hpp"

#define CS_HLWA_GW_CSF								int
#define CS_HLWA_GW_CSF_DISABLETITLEBAR				0x01
#define CS_HLWA_GW_CSF_TRANSPARENT					0x02
#define CS_HLWA_GW_CSF_CONTEXTMENU					0x04
#define CS_HLWA_GW_CSF_ENABLEFILEDROP				0x08
// TODO: Add ALWAYSONTOP.

#define CS_HLWA_GW_F								int
#define CS_HLWA_GW_F_CLOSE							0x01
#define CS_HLWA_GW_F_DESTRUCTING					0x02
#define CS_HLWA_GW_F_MANUALLYCREATED				0x04
#define CS_HLWA_GW_F_SUPPORTSCOMPOSITION			0x08
#define CS_HLWA_GW_F_RESIZEFLAG						0x10
#define CS_HLWA_GW_F_HOVEREDFLAG					0x20
#define CS_HLWA_GW_F_FOCUSEDFLAG					0x40
#define CS_HLWA_GW_F_WASMAXIMIZE					0x80

#define CS_HLWA_GW_A								int
#define CS_HLWA_GW_A_RELEASE						0
#define CS_HLWA_GW_A_PRESS							1
#define CS_HLWA_GW_A_ROLLOVER						2

#define I_CS_HLWA_GW_A_STICK						3

#define CS_HLWA_GW_M								int
#define CS_HLWA_GW_M_SHIFT							0x0001
#define CS_HLWA_GW_M_CONTROL						0x0002
#define CS_HLWA_GW_M_ALT							0x0004
#define CS_HLWA_GW_M_SUPER							0x0008
#define CS_HLWA_GW_M_CAPSLOCK						0x0010
#define CS_HLWA_GW_M_NUMLOCK						0x0020

#define CS_HLWA_GW_M_MOD_MASK						(CS_HLWA_GW_M_SHIFT | CS_HLWA_GW_M_CONTROL | CS_HLWA_GW_M_ALT | CS_HLWA_GW_M_SUPER | CS_HLWA_GW_M_CAPSLOCK | CS_HLWA_GW_M_NUMLOCK)

#define CS_HLWA_GW_K								int
#define CS_HLWA_GW_K_SPACE							32
#define CS_HLWA_GW_K_APOSTROPHE						39
#define CS_HLWA_GW_K_COMMA							44
#define CS_HLWA_GW_K_MINUS							45
#define CS_HLWA_GW_K_PERIOD							46
#define CS_HLWA_GW_K_SLASH							47
#define CS_HLWA_GW_K_0								48
#define CS_HLWA_GW_K_1								49
#define CS_HLWA_GW_K_2								50
#define CS_HLWA_GW_K_3								51
#define CS_HLWA_GW_K_4								52
#define CS_HLWA_GW_K_5								53
#define CS_HLWA_GW_K_6								54
#define CS_HLWA_GW_K_7								55
#define CS_HLWA_GW_K_8								56
#define CS_HLWA_GW_K_9								57
#define CS_HLWA_GW_K_SEMICOLON						59
#define CS_HLWA_GW_K_EQUAL							61
#define CS_HLWA_GW_K_A								65
#define CS_HLWA_GW_K_B								66
#define CS_HLWA_GW_K_C								67
#define CS_HLWA_GW_K_D								68
#define CS_HLWA_GW_K_E								69
#define CS_HLWA_GW_K_F								70
#define CS_HLWA_GW_K_G								71
#define CS_HLWA_GW_K_H								72
#define CS_HLWA_GW_K_I								73
#define CS_HLWA_GW_K_J								74
#define CS_HLWA_GW_K_K								75
#define CS_HLWA_GW_K_L								76
#define CS_HLWA_GW_K_M								77
#define CS_HLWA_GW_K_N								78
#define CS_HLWA_GW_K_O								79
#define CS_HLWA_GW_K_P								80
#define CS_HLWA_GW_K_Q								81
#define CS_HLWA_GW_K_R								82
#define CS_HLWA_GW_K_S								83
#define CS_HLWA_GW_K_T								84
#define CS_HLWA_GW_K_U								85
#define CS_HLWA_GW_K_V								86
#define CS_HLWA_GW_K_W								87
#define CS_HLWA_GW_K_X								88
#define CS_HLWA_GW_K_Y								89
#define CS_HLWA_GW_K_Z								90
#define CS_HLWA_GW_K_LBRACKET						91
#define CS_HLWA_GW_K_BACKSLASH						92
#define CS_HLWA_GW_K_RBRACKET						93
#define CS_HLWA_GW_K_BACKTICK						96
#define CS_HLWA_GW_K_WORLD1							161
#define CS_HLWA_GW_K_WORLD2							162

#define CS_HLWA_GW_K_ESCAPE							256
#define CS_HLWA_GW_K_ENTER							257
#define CS_HLWA_GW_K_TAB							258
#define CS_HLWA_GW_K_BACKSPACE						259
#define CS_HLWA_GW_K_INSERT							260
#define CS_HLWA_GW_K_DELETE							261
#define CS_HLWA_GW_K_RIGHT							262
#define CS_HLWA_GW_K_LEFT							263
#define CS_HLWA_GW_K_DOWN							264
#define CS_HLWA_GW_K_UP								265
#define CS_HLWA_GW_K_PAGEUP							266
#define CS_HLWA_GW_K_PAGEDOWN						267
#define CS_HLWA_GW_K_HOME							268
#define CS_HLWA_GW_K_END							269
#define CS_HLWA_GW_K_CAPSLOCK						280
#define CS_HLWA_GW_K_SCROLLLOCK						281
#define CS_HLWA_GW_K_NUMLOCK						282
#define CS_HLWA_GW_K_PRINTSCREEN					283
#define CS_HLWA_GW_K_PAUSE							284
#define CS_HLWA_GW_K_F1								290
#define CS_HLWA_GW_K_F2								291
#define CS_HLWA_GW_K_F3								292
#define CS_HLWA_GW_K_F4								293
#define CS_HLWA_GW_K_F5								294
#define CS_HLWA_GW_K_F6								295
#define CS_HLWA_GW_K_F7								296
#define CS_HLWA_GW_K_F8								297
#define CS_HLWA_GW_K_F9								298
#define CS_HLWA_GW_K_F10							299
#define CS_HLWA_GW_K_F11							300
#define CS_HLWA_GW_K_F12							301
#define CS_HLWA_GW_K_F13							302
#define CS_HLWA_GW_K_F14							303
#define CS_HLWA_GW_K_F15							304
#define CS_HLWA_GW_K_F16							305
#define CS_HLWA_GW_K_F17							306
#define CS_HLWA_GW_K_F18							307
#define CS_HLWA_GW_K_F19							308
#define CS_HLWA_GW_K_F20							309
#define CS_HLWA_GW_K_F21							310
#define CS_HLWA_GW_K_F22							311
#define CS_HLWA_GW_K_F23							312
#define CS_HLWA_GW_K_F24							313
#define CS_HLWA_GW_K_F25							314
#define CS_HLWA_GW_K_KP_0							320
#define CS_HLWA_GW_K_KP_1							321
#define CS_HLWA_GW_K_KP_2							322
#define CS_HLWA_GW_K_KP_3							323
#define CS_HLWA_GW_K_KP_4							324
#define CS_HLWA_GW_K_KP_5							325
#define CS_HLWA_GW_K_KP_6							326
#define CS_HLWA_GW_K_KP_7							327
#define CS_HLWA_GW_K_KP_8							328
#define CS_HLWA_GW_K_KP_9							329
#define CS_HLWA_GW_K_KP_DECIMAL						330
#define CS_HLWA_GW_K_KP_DIVIDE						331
#define CS_HLWA_GW_K_KP_MULTIPLY					332
#define CS_HLWA_GW_K_KP_SUBTRACT					333
#define CS_HLWA_GW_K_KP_ADD							334
#define CS_HLWA_GW_K_KP_ENTER						335
#define CS_HLWA_GW_K_KP_EQUAL						336
#define CS_HLWA_GW_K_LSHIFT							340
#define CS_HLWA_GW_K_LCONTROL						341
#define CS_HLWA_GW_K_LALT							342
#define CS_HLWA_GW_K_LSUPER							343
#define CS_HLWA_GW_K_RSHIFT							344
#define CS_HLWA_GW_K_RCONTROL						345
#define CS_HLWA_GW_K_RALT							346
#define CS_HLWA_GW_K_RSUPER							347
#define CS_HLWA_GW_K_MENU							348

#define CS_HLWA_GW_K_LAST							CS_HLWA_GW_K_MENU

#define CS_HLWA_GW_MBTN								int
#define CS_HLWA_GW_MBTN_1							0
#define CS_HLWA_GW_MBTN_2							1
#define CS_HLWA_GW_MBTN_3							2
#define CS_HLWA_GW_MBTN_4							3
#define CS_HLWA_GW_MBTN_5							4
#define CS_HLWA_GW_MBTN_6							5
#define CS_HLWA_GW_MBTN_7							6
#define CS_HLWA_GW_MBTN_8							7

#define CS_HLWA_GW_MBTN_LAST						CS_HLWA_GW_MBTN_8

#define CS_HLWA_GW_GAMEPAD							int
#define CS_HLWA_GW_GAMEPAD_1						0
#define CS_HLWA_GW_GAMEPAD_2						1
#define CS_HLWA_GW_GAMEPAD_3						2
#define CS_HLWA_GW_GAMEPAD_4						3
#define CS_HLWA_GW_GAMEPAD_5						4
#define CS_HLWA_GW_GAMEPAD_6						5
#define CS_HLWA_GW_GAMEPAD_7						6
#define CS_HLWA_GW_GAMEPAD_8						7
#define CS_HLWA_GW_GAMEPAD_9						8
#define CS_HLWA_GW_GAMEPAD_10						9
#define CS_HLWA_GW_GAMEPAD_11						10
#define CS_HLWA_GW_GAMEPAD_12						11
#define CS_HLWA_GW_GAMEPAD_13						12
#define CS_HLWA_GW_GAMEPAD_14						13
#define CS_HLWA_GW_GAMEPAD_15						14
#define CS_HLWA_GW_GAMEPAD_16						15
#define CS_HLWA_GW_GAMEPAD_LAST						CS_HLWA_GW_GAMEPAD_16

#include <unordered_map>

namespace CoganSoftware::HLWA::Glassware {
	class Glassware;
	
	enum struct IconSpecifierType { FILE, BUFFER };
	struct IconSpecifier {
		union Data {
			struct File {
				File(const CS_HLWA_STRING& p_path);
				File(CS_HLWA_STRING&& p_path);
				File(const File& p_other);
				~File();

				CS_HLWA_STRING path;

				File& operator=(const File& p_other) {
					path = p_other.path;
					return *this;
				}
			} file;
			struct Buffer {
				Buffer(unsigned char* p_data, uint32_t p_dataLength, uint32_t p_width, uint32_t p_height);
				Buffer(const Buffer& p_other);
				~Buffer();

				unsigned char* data;
				uint32_t dataLength;
				uint32_t width;
				uint32_t height;

				Buffer& operator=(const Buffer& p_other) {
					data = p_other.data;
					dataLength = p_other.dataLength;
					width = p_other.width;
					height = p_other.height;
					return *this;
				}
			} buffer{ nullptr, 0, 0, 0 };

			Data() {};
			~Data() {};
		} data;
		IconSpecifierType type;

		IconSpecifier(const CS_HLWA_STRING& p_path);
		IconSpecifier(unsigned char* p_data, uint32_t p_dataLength, uint32_t p_width, uint32_t p_height);
		IconSpecifier(const IconSpecifier& p_other);
		~IconSpecifier();

		IconSpecifier& operator=(const IconSpecifier& p_other) {
			if (this != &p_other) {
				this->~IconSpecifier(); // destroy current content
				type = p_other.type;
				if (type == IconSpecifierType::FILE)
					new (&data.file) Data::File(p_other.data.file);
				else
					new (&data.buffer) Data::Buffer(p_other.data.buffer);
			}
			return *this;
		}
	};
	struct ContextState {
		CS_HLWA_GW_CSF flags;
		uint32_t positionSnapping = 0;
		uint32_t sizeSnapping = 0;
	};
	enum struct ModeState { WINDOWED, FULLSCREEN, FULLSCREEN_BORDERLESS };
	enum struct PositionState {
		DEFAULT,
		TOP_LEFT, TOP_MIDDLE, TOP_RIGHT,
		MIDDLE_LEFT, MIDDLE_MIDDLE, MIDDLE_RIGHT,
		BOTTOM_LEFT, BOTTOM_MIDDLE, BOTTOM_RIGHT
	};

	struct GlasswareCreateInfoConfig {
	public:
		CS_HLWA_STRING title = CS_HLWA_STRVAL("");
		CS_HLWA_STRING className = CS_HLWA_STRVAL("");
		IconSpecifier iconSpecifier = IconSpecifier{ nullptr, 0, 0, 0 };
		ContextState context{};
		ModeState mode = ModeState::WINDOWED;
		PositionState position = PositionState::DEFAULT;
		int offsetX = 0;
		int offsetY = 0;
		uint32_t sizeX = 1000;
		uint32_t sizeY = 800;
		Glassware* parent = nullptr;
		void* userPtr = nullptr;
	};

	struct GamepadState {
	public:
		void Clear() {
			for (uint32_t i = 0; i < 15; i++) {
				buttons[i] = 0;
			}
			for (uint32_t i = 0; i < 6; i++) {
				axes[i] = 0.0f;
			}
		};

		unsigned char buttons[15];
		float axes[6];
	};

	class Glassware {
	public:
		bool IsDependentsHovered();
	protected:
		void TickContextWindow();
#if defined(_WIN32)
		static intptr_t __stdcall WindowProc(void* p_hwnd, uint32_t p_uMsg, uintptr_t p_wParam, intptr_t p_lParam);
#endif
		void InputChar(uint32_t p_codepoint, CS_HLWA_GW_M p_mods);
		void InputKey(int p_scancode, CS_HLWA_GW_K p_keycode, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods);
		void InputMouseButton(CS_HLWA_GW_MBTN p_button, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods);
		void InputMouseScroll(double p_xoffset, double p_yoffset, CS_HLWA_GW_M p_mods);
		void InputMouseMove(double p_posx, double p_posy, CS_HLWA_GW_M p_mods);
		void InputMouseDelta(double p_dx, double p_dy, CS_HLWA_GW_M p_mods);
		struct InputCallbacks {
			void(*m_charCall)(Glassware* p_glassware, uint32_t p_codepoint, CS_HLWA_GW_M p_mods) = DEFCharCallback;
			void(*m_keyCall)(Glassware* p_glassware, int p_scancode, CS_HLWA_GW_K p_keycode, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods) = DEFKeyCallback;
			void(*m_mouseBtnCall)(Glassware* p_glassware, CS_HLWA_GW_MBTN p_button, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods) = DEFMouseButtonCallback;
			void(*m_mouseScrCall)(Glassware* p_glassware, double p_xoffset, double p_yoffset, CS_HLWA_GW_M p_mods) = DEFMouseScrollCallback;
			void(*m_mouseMoveCall)(Glassware* p_glassware, double p_posx, double p_posy, CS_HLWA_GW_M p_mods) = DEFMouseMoveCallback;
			void(*m_mouseDelCall)(Glassware* p_glassware, double p_dx, double p_dy, CS_HLWA_GW_M p_mods) = DEFMouseDeltaCallback;
		} IC;
	public:
		// Default Callbacks
		static inline void DEFCharCallback(Glassware* p_glassware, uint32_t p_codepoint, CS_HLWA_GW_M p_mods) {};
		static inline void DEFKeyCallback(Glassware* p_glassware, int p_scancode, CS_HLWA_GW_K p_keycode, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods) {};
		static inline void DEFMouseButtonCallback(Glassware* p_glassware, CS_HLWA_GW_MBTN p_button, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods) {};
		static inline void DEFMouseScrollCallback(Glassware* p_glassware, double p_xoffset, double p_yoffset, CS_HLWA_GW_M p_mods) {};
		static inline void DEFMouseMoveCallback(Glassware* p_glassware, double p_posx, double p_posy, CS_HLWA_GW_M p_mods) {};
		static inline void DEFMouseDeltaCallback(Glassware* p_glassware, double p_dx, double p_dy, CS_HLWA_GW_M p_mods) {};

#if defined(_WIN32)
		Glassware(void* p_hwnd); // TODO: Add Glassware (HWND).
#endif
		Glassware(const GlasswareCreateInfoConfig& p_config);
		virtual ~Glassware();
		
		// Static Handling
		static CS_HLWA_R Poll();
		static void PreUpdateWindows();
		static void PostUpdateWindows();

		// Virtuals
		virtual void OnUpdate() = 0;
		virtual void OnPostUpdate() = 0;

		// Hierarchical
		const Glassware* GetParent() const;
		const std::vector<Glassware*> GetChildren();

		// UserPointer
		void SetUserPointer(void* p_userPtr);
		const void* GetUserPointer() const;
		
		// Window Handling
		void SetTitle(const CS_HLWA_STRING& p_title);
		void Hide();
		void Show();
		void Minimize();
		void Restore();
		void Maximize();
		CS_HLWA_R SetMode(ModeState p_mode);
		void Close();
#if defined(_WIN32)
		void* GetHandle();
#endif

		// Window State
		bool Minimized() const;
		bool Maximized() const;
		ModeState GetMode() const;

		// Transform Handling
		int GetPositionX() const;
		int GetPositionY() const;
		void GetPositions(int& p_outX, int& p_outY) const;
		uint32_t GetSizeX() const;
		uint32_t GetSizeY() const;
		void GetSizes(uint32_t& p_outX, uint32_t& p_outY) const;
		uint32_t GetClientSizeX() const;
		uint32_t GetClientSizeY() const;
		void GetClientSizes(uint32_t& p_outX, uint32_t& p_outY) const;
		void SetPositions(PositionState p_state, int p_offsetX, int p_offsetY);
		void SetSizes(uint32_t p_sizeX, uint32_t p_sizeY);

		// Input Callback
		void SetCharCallback(void(*p_callback)(Glassware* p_glassware, uint32_t p_codepoint, CS_HLWA_GW_M p_mods));
		void SetKeyCallback(void(*p_callback)(Glassware* p_glassware, int p_scancode, CS_HLWA_GW_K p_keycode, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods));
		void SetMouseButtonCallback(void(*p_callback)(Glassware* p_glassware, CS_HLWA_GW_MBTN p_button, CS_HLWA_GW_A p_action, CS_HLWA_GW_M p_mods));
		void SetMouseScrollCallback(void(*p_callback)(Glassware* p_glassware, double p_xoffset, double p_yoffset, CS_HLWA_GW_M p_mods));
		void SetMouseMoveCallback(void(*p_callback)(Glassware* p_glassware, double p_posx, double p_posy, CS_HLWA_GW_M p_mods));
		void SetMouseDeltaCallback(void(*p_callback)(Glassware* p_glassware, double p_dx, double p_dy, CS_HLWA_GW_M p_mods));
		bool GetMousePosition(double& p_outX, double& p_outY);
		void GetGamepadState(int p_gid, GamepadState* p_state); // TODO: Add GetGamepadState.

		// Input Utility
		void SetInputMode(int p_major, int p_minor); // TODO: Add SetInputMode.
		const InputCallbacks& GetInputCallbacks() const;
		static CS_HLWA_GW_M GetKeyMods();
		static CS_HLWA_GW_K ScancodeToKeycode(int p_scancode);
		static int KeycodeToScancode(int p_keycode);
		static std::string GetKeycodeDisplayName(int p_keycode);

		// Cursor
		void SetCursor(); // TODO: Add SetCursor.
		void BindCursorToWindow(); // TODO: Add BindCursorToWindow.
		void UnbindCursorFromWindow(); // TODO: Add UnbindCursorFromWindow.
		void BindCursorToAbsoluteArea(); // TODO: Add BindCursorToAbsoluteArea.
		void UnbindCursorFromAbsoluteArea(); // TODO: Add UnbindCursorFromAbsoluteArea.
		void BindCursorToRelativeArea(); // TODO: Add BindCursorToRelativeArea.
		void UnbindCursorFromRelativeArea(); // TODO: Add UnbindCursorFromRelativeArea.

		// DPI Handling
		uint32_t GetDPI();
		float GetDPIScaleFactor();
		static float GetDPIScaleFactor(uint32_t p_dpi);

		// Context Handling
		void SetContextFlag(CS_HLWA_GW_CSF p_flag, bool p_value); // TODO: Finish this...
		bool GetContextFlag(CS_HLWA_GW_CSF p_flag) const;
		void SetPositionSnapping(uint32_t p_value);
		uint32_t GetPositionSnapping() const;
		void SetSizeSnapping(uint32_t p_value);
		uint32_t GetSizeSnapping() const;

		// Composition Handling
		bool SupportsComposition() const;

		// Resize Flag Handling
		void ForceTriggerResizeFlag();
		void ResetResizeFlag();
		bool GetResizeFlag() const;

		// Hovered & Focus Flags Handling
		void SetHoveredFlag(bool p_value);
		bool GetHoveredFlag() const;
		void SetFocusedFlag(bool p_value);
		bool GetFocusedFlag() const;
	private:
#if defined(_WIN32)
		struct WINDOWST {
			void* m_handle;
			intptr_t m_windowedStyle;
			intptr_t m_fullscreenStyle;
			intptr_t m_fullscreenBorderlessStyle;
			intptr_t m_windowedExStyle;
			intptr_t m_fullscreenExStyle;
			intptr_t m_fullscreenBorderlessExStyle;
			int m_unfullscreenRectLeft;
			int m_unfullscreenRectTop;
			int m_unfullscreenRectRight;
			int m_unfullscreenRectBottom;
			wchar_t m_highSurrogate = 0;

			static inline std::unordered_map<CS_HLWA_STRING, uint32_t> g_windowClasses{};
		} WINDOWS;
#endif
		struct INPUTSTORAGET {
			bool m_stickyKeys = false;
			bool m_stickyMouse = false;
			bool m_lockKeyMods = false;
			char m_keys[CS_HLWA_GW_K_LAST + 1];
			char m_mouseButtons[CS_HLWA_GW_MBTN_LAST + 1];
			double m_mouseX = 0;
			double m_mouseY = 0;
		} INPUTSTORAGE;

		GlasswareCreateInfoConfig m_config;
		std::vector<Glassware*> m_children;
		void* m_userPtr;
		
		CS_HLWA_GW_F m_flags = 0;

		int m_posX;
		int m_posY;
		uint32_t m_sizeX;
		uint32_t m_sizeY;
		uint32_t m_cliX;
		uint32_t m_cliY;
		uint32_t m_dpi = 72;

		static inline std::vector<Glassware*> g_windows{};
	};
}
#endif