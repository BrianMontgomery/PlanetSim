#pragma once
#include "PSIMPCH.h"

typedef enum class KeyCode : uint16_t
{
	// From glfw3.h
	Space = 32,
	Apostrophe = 39, /* ' */
	Comma = 44, /* , */
	Minus = 45, /* - */
	Period = 46, /* . */
	Slash = 47, /* / */

	D0 = 48, /* 0 */
	D1 = 49, /* 1 */
	D2 = 50, /* 2 */
	D3 = 51, /* 3 */
	D4 = 52, /* 4 */
	D5 = 53, /* 5 */
	D6 = 54, /* 6 */
	D7 = 55, /* 7 */
	D8 = 56, /* 8 */
	D9 = 57, /* 9 */

	Semicolon = 59, /* ; */
	Equal = 61, /* = */

	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,

	LeftBracket = 91,  /* [ */
	Backslash = 92,  /* \ */
	RightBracket = 93,  /* ] */
	GraveAccent = 96,  /* ` */

	World1 = 161, /* non-US #1 */
	World2 = 162, /* non-US #2 */

	/* Function keys */
	Escape = 256,
	Enter = 257,
	Tab = 258,
	Backspace = 259,
	Insert = 260,
	Delete = 261,
	Right = 262,
	Left = 263,
	Down = 264,
	Up = 265,
	PageUp = 266,
	PageDown = 267,
	Home = 268,
	End = 269,
	CapsLock = 280,
	ScrollLock = 281,
	NumLock = 282,
	PrintScreen = 283,
	Pause = 284,
	F1 = 290,
	F2 = 291,
	F3 = 292,
	F4 = 293,
	F5 = 294,
	F6 = 295,
	F7 = 296,
	F8 = 297,
	F9 = 298,
	F10 = 299,
	F11 = 300,
	F12 = 301,
	F13 = 302,
	F14 = 303,
	F15 = 304,
	F16 = 305,
	F17 = 306,
	F18 = 307,
	F19 = 308,
	F20 = 309,
	F21 = 310,
	F22 = 311,
	F23 = 312,
	F24 = 313,
	F25 = 314,

	/* Keypad */
	KP0 = 320,
	KP1 = 321,
	KP2 = 322,
	KP3 = 323,
	KP4 = 324,
	KP5 = 325,
	KP6 = 326,
	KP7 = 327,
	KP8 = 328,
	KP9 = 329,
	KPDecimal = 330,
	KPDivide = 331,
	KPMultiply = 332,
	KPSubtract = 333,
	KPAdd = 334,
	KPEnter = 335,
	KPEqual = 336,

	LeftShift = 340,
	LeftControl = 341,
	LeftAlt = 342,
	LeftSuper = 343,
	RightShift = 344,
	RightControl = 345,
	RightAlt = 346,
	RightSuper = 347,
	Menu = 348
} Key;

inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
{
	os << static_cast<int32_t>(keyCode);
	return os;
}

// From glfw3.h
#define PSIM_KEY_SPACE           ::Key::Space
#define PSIM_KEY_APOSTROPHE      ::Key::Apostrophe    /* ' */
#define PSIM_KEY_COMMA           ::Key::Comma         /* , */
#define PSIM_KEY_MINUS           ::Key::Minus         /* - */
#define PSIM_KEY_PERIOD          ::Key::Period        /* . */
#define PSIM_KEY_SLASH           ::Key::Slash         /* / */
#define PSIM_KEY_0               ::Key::D0
#define PSIM_KEY_1               ::Key::D1
#define PSIM_KEY_2               ::Key::D2
#define PSIM_KEY_3               ::Key::D3
#define PSIM_KEY_4               ::Key::D4
#define PSIM_KEY_5               ::Key::D5
#define PSIM_KEY_6               ::Key::D6
#define PSIM_KEY_7               ::Key::D7
#define PSIM_KEY_8               ::Key::D8
#define PSIM_KEY_9               ::Key::D9
#define PSIM_KEY_SEMICOLON       ::Key::Semicolon     /* ; */
#define PSIM_KEY_EQUAL           ::Key::Equal         /* = */
#define PSIM_KEY_A               ::Key::A
#define PSIM_KEY_B               ::Key::B
#define PSIM_KEY_C               ::Key::C
#define PSIM_KEY_D               ::Key::D
#define PSIM_KEY_E               ::Key::E
#define PSIM_KEY_F               ::Key::F
#define PSIM_KEY_G               ::Key::G
#define PSIM_KEY_H               ::Key::H
#define PSIM_KEY_I               ::Key::I
#define PSIM_KEY_J               ::Key::J
#define PSIM_KEY_K               ::Key::K
#define PSIM_KEY_L               ::Key::L
#define PSIM_KEY_M               ::Key::M
#define PSIM_KEY_N               ::Key::N
#define PSIM_KEY_O               ::Key::O
#define PSIM_KEY_P               ::Key::P
#define PSIM_KEY_Q               ::Key::Q
#define PSIM_KEY_R               ::Key::R
#define PSIM_KEY_S               ::Key::S
#define PSIM_KEY_T               ::Key::T
#define PSIM_KEY_U               ::Key::U
#define PSIM_KEY_V               ::Key::V
#define PSIM_KEY_W               ::Key::W
#define PSIM_KEY_X               ::Key::X
#define PSIM_KEY_Y               ::Key::Y
#define PSIM_KEY_Z               ::Key::Z
#define PSIM_KEY_LEFT_BRACKET    ::Key::LeftBracket   /* [ */
#define PSIM_KEY_BACKSLASH       ::Key::Backslash     /* \ */
#define PSIM_KEY_RIGHT_BRACKET   ::Key::RightBracket  /* ] */
#define PSIM_KEY_GRAVE_ACCENT    ::Key::GraveAccent   /* ` */
#define PSIM_KEY_WORLD_1         ::Key::World1        /* non-US #1 */
#define PSIM_KEY_WORLD_2         ::Key::World2        /* non-US #2 */

/* Function keys */
#define PSIM_KEY_ESCAPE          ::Key::Escape
#define PSIM_KEY_ENTER           ::Key::Enter
#define PSIM_KEY_TAB             ::Key::Tab
#define PSIM_KEY_BACKSPACE       ::Key::Backspace
#define PSIM_KEY_INSERT          ::Key::Insert
#define PSIM_KEY_DELETE          ::Key::Delete
#define PSIM_KEY_RIGHT           ::Key::Right
#define PSIM_KEY_LEFT            ::Key::Left
#define PSIM_KEY_DOWN            ::Key::Down
#define PSIM_KEY_UP              ::Key::Up
#define PSIM_KEY_PAGE_UP         ::Key::PageUp
#define PSIM_KEY_PAGE_DOWN       ::Key::PageDown
#define PSIM_KEY_HOME            ::Key::Home
#define PSIM_KEY_END             ::Key::End
#define PSIM_KEY_CAPS_LOCK       ::Key::CapsLock
#define PSIM_KEY_SCROLL_LOCK     ::Key::ScrollLock
#define PSIM_KEY_NUM_LOCK        ::Key::NumLock
#define PSIM_KEY_PRINT_SCREEN    ::Key::PrintScreen
#define PSIM_KEY_PAUSE           ::Key::Pause
#define PSIM_KEY_F1              ::Key::F1
#define PSIM_KEY_F2              ::Key::F2
#define PSIM_KEY_F3              ::Key::F3
#define PSIM_KEY_F4              ::Key::F4
#define PSIM_KEY_F5              ::Key::F5
#define PSIM_KEY_F6              ::Key::F6
#define PSIM_KEY_F7              ::Key::F7
#define PSIM_KEY_F8              ::Key::F8
#define PSIM_KEY_F9              ::Key::F9
#define PSIM_KEY_F10             ::Key::F10
#define PSIM_KEY_F11             ::Key::F11
#define PSIM_KEY_F12             ::Key::F12
#define PSIM_KEY_F13             ::Key::F13
#define PSIM_KEY_F14             ::Key::F14
#define PSIM_KEY_F15             ::Key::F15
#define PSIM_KEY_F16             ::Key::F16
#define PSIM_KEY_F17             ::Key::F17
#define PSIM_KEY_F18             ::Key::F18
#define PSIM_KEY_F19             ::Key::F19
#define PSIM_KEY_F20             ::Key::F20
#define PSIM_KEY_F21             ::Key::F21
#define PSIM_KEY_F22             ::Key::F22
#define PSIM_KEY_F23             ::Key::F23
#define PSIM_KEY_F24             ::Key::F24
#define PSIM_KEY_F25             ::Key::F25

/* Keypad */
#define PSIM_KEY_KP_0            ::Key::KP0
#define PSIM_KEY_KP_1            ::Key::KP1
#define PSIM_KEY_KP_2            ::Key::KP2
#define PSIM_KEY_KP_3            ::Key::KP3
#define PSIM_KEY_KP_4            ::Key::KP4
#define PSIM_KEY_KP_5            ::Key::KP5
#define PSIM_KEY_KP_6            ::Key::KP6
#define PSIM_KEY_KP_7            ::Key::KP7
#define PSIM_KEY_KP_8            ::Key::KP8
#define PSIM_KEY_KP_9            ::Key::KP9
#define PSIM_KEY_KP_DECIMAL      ::Key::KPDecimal
#define PSIM_KEY_KP_DIVIDE       ::Key::KPDivide
#define PSIM_KEY_KP_MULTIPLY     ::Key::KPMultiply
#define PSIM_KEY_KP_SUBTRACT     ::Key::KPSubtract
#define PSIM_KEY_KP_ADD          ::Key::KPAdd
#define PSIM_KEY_KP_ENTER        ::Key::KPEnter
#define PSIM_KEY_KP_EQUAL        ::Key::KPEqual

#define PSIM_KEY_LEFT_SHIFT      ::Key::LeftShift
#define PSIM_KEY_LEFT_CONTROL    ::Key::LeftControl
#define PSIM_KEY_LEFT_ALT        ::Key::LeftAlt
#define PSIM_KEY_LEFT_SUPER      ::Key::LeftSuper
#define PSIM_KEY_RIGHT_SHIFT     ::Key::RightShift
#define PSIM_KEY_RIGHT_CONTROL   ::Key::RightControl
#define PSIM_KEY_RIGHT_ALT       ::Key::RightAlt
#define PSIM_KEY_RIGHT_SUPER     ::Key::RightSuper
#define PSIM_KEY_MENU            ::Key::Menu