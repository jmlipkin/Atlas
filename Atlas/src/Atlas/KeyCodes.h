#pragma once

#include "imgui/imgui.h"
// From glfw3.h
#define AT_KEY_SPACE              32
#define AT_KEY_APOSTROPHE         39  /* ' */
#define AT_KEY_COMMA              44  /* , */
#define AT_KEY_MINUS              45  /* - */
#define AT_KEY_PERIOD             46  /* . */
#define AT_KEY_SLASH              47  /* / */
#define AT_KEY_0                  48
#define AT_KEY_1                  49
#define AT_KEY_2                  50
#define AT_KEY_3                  51
#define AT_KEY_4                  52
#define AT_KEY_5                  53
#define AT_KEY_6                  54
#define AT_KEY_7                  55
#define AT_KEY_8                  56
#define AT_KEY_9                  57
#define AT_KEY_SEMICOLON          59  /* ; */
#define AT_KEY_EQUAL              61  /* = */
#define AT_KEY_A                  65
#define AT_KEY_B                  66
#define AT_KEY_C                  67
#define AT_KEY_D                  68
#define AT_KEY_E                  69
#define AT_KEY_F                  70
#define AT_KEY_G                  71
#define AT_KEY_H                  72
#define AT_KEY_I                  73
#define AT_KEY_J                  74
#define AT_KEY_K                  75
#define AT_KEY_L                  76
#define AT_KEY_M                  77
#define AT_KEY_N                  78
#define AT_KEY_O                  79
#define AT_KEY_P                  80
#define AT_KEY_Q                  81
#define AT_KEY_R                  82
#define AT_KEY_S                  83
#define AT_KEY_T                  84
#define AT_KEY_U                  85
#define AT_KEY_V                  86
#define AT_KEY_W                  87
#define AT_KEY_X                  88
#define AT_KEY_Y                  89
#define AT_KEY_Z                  90
#define AT_KEY_LEFT_BRACKET       91  /* [ */
#define AT_KEY_BACKSLASH          92  /* \ */
#define AT_KEY_RIGHT_BRACKET      93  /* ] */
#define AT_KEY_GRAVE_ACCENT       96  /* ` */
#define AT_KEY_WORLD_1            161 /* non-US #1 */
#define AT_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define AT_KEY_ESCAPE             256
#define AT_KEY_ENTER              257
#define AT_KEY_TAB                258
#define AT_KEY_BACKSPACE          259
#define AT_KEY_INSERT             260
#define AT_KEY_DELETE             261
#define AT_KEY_RIGHT              262
#define AT_KEY_LEFT               263
#define AT_KEY_DOWN               264
#define AT_KEY_UP                 265
#define AT_KEY_PAGE_UP            266
#define AT_KEY_PAGE_DOWN          267
#define AT_KEY_HOME               268
#define AT_KEY_END                269
#define AT_KEY_CAPS_LOCK          280
#define AT_KEY_SCROLL_LOCK        281
#define AT_KEY_NUM_LOCK           282
#define AT_KEY_PRINT_SCREEN       283
#define AT_KEY_PAUSE              284
#define AT_KEY_F1                 290
#define AT_KEY_F2                 291
#define AT_KEY_F3                 292
#define AT_KEY_F4                 293
#define AT_KEY_F5                 294
#define AT_KEY_F6                 295
#define AT_KEY_F7                 296
#define AT_KEY_F8                 297
#define AT_KEY_F9                 298
#define AT_KEY_F10                299
#define AT_KEY_F11                300
#define AT_KEY_F12                301
#define AT_KEY_F13                302
#define AT_KEY_F14                303
#define AT_KEY_F15                304
#define AT_KEY_F16                305
#define AT_KEY_F17                306
#define AT_KEY_F18                307
#define AT_KEY_F19                308
#define AT_KEY_F20                309
#define AT_KEY_F21                310
#define AT_KEY_F22                311
#define AT_KEY_F23                312
#define AT_KEY_F24                313
#define AT_KEY_F25                314
#define AT_KEY_KP_0               320
#define AT_KEY_KP_1               321
#define AT_KEY_KP_2               322
#define AT_KEY_KP_3               323
#define AT_KEY_KP_4               324
#define AT_KEY_KP_5               325
#define AT_KEY_KP_6               326
#define AT_KEY_KP_7               327
#define AT_KEY_KP_8               328
#define AT_KEY_KP_9               329
#define AT_KEY_KP_DECIMAL         330
#define AT_KEY_KP_DIVIDE          331
#define AT_KEY_KP_MULTIPLY        332
#define AT_KEY_KP_SUBTRACT        333
#define AT_KEY_KP_ADD             334
#define AT_KEY_KP_ENTER           335
#define AT_KEY_KP_EQUAL           336
#define AT_KEY_LEFT_SHIFT         340
#define AT_KEY_LEFT_CONTROL       341
#define AT_KEY_LEFT_ALT           342
#define AT_KEY_LEFT_SUPER         343
#define AT_KEY_RIGHT_SHIFT        344
#define AT_KEY_RIGHT_CONTROL      345
#define AT_KEY_RIGHT_ALT          346
#define AT_KEY_RIGHT_SUPER        347
#define AT_KEY_MENU               348



////////////////////////////////////////////////////////////////

static ImGuiKey keyCodeToImGuiKey(int keycode) {
    switch (keycode) {
        case AT_KEY_SPACE:
            return ImGuiKey_Space;
        case AT_KEY_APOSTROPHE:
            return ImGuiKey_Apostrophe;
        case AT_KEY_COMMA:
            return ImGuiKey_Comma;
        case AT_KEY_MINUS:
            return ImGuiKey_Minus;
        case AT_KEY_PERIOD:
            return ImGuiKey_Period;
        case AT_KEY_SLASH:
            return ImGuiKey_Slash;
        case AT_KEY_0:
            return ImGuiKey_0;
        case AT_KEY_1:
            return ImGuiKey_1;
        case AT_KEY_2:
            return ImGuiKey_2;
        case AT_KEY_3:
            return ImGuiKey_3;
        case AT_KEY_4:
            return ImGuiKey_4;
        case AT_KEY_5:
            return ImGuiKey_5;
        case AT_KEY_6:
            return ImGuiKey_6;
        case AT_KEY_7:
            return ImGuiKey_7;
        case AT_KEY_8:
            return ImGuiKey_8;
        case AT_KEY_9:
            return ImGuiKey_9;
        case AT_KEY_SEMICOLON:
            return ImGuiKey_Semicolon;
        case AT_KEY_EQUAL:
            return ImGuiKey_Equal;
        case AT_KEY_A:
            return ImGuiKey_A;
        case AT_KEY_B:
            return ImGuiKey_B;
        case AT_KEY_C:
            return ImGuiKey_C;
        case AT_KEY_D:
            return ImGuiKey_D;
        case AT_KEY_E:
            return ImGuiKey_E;
        case AT_KEY_F:
            return ImGuiKey_F;
        case AT_KEY_G:
            return ImGuiKey_G;
        case AT_KEY_H:
            return ImGuiKey_H;
        case AT_KEY_I:
            return ImGuiKey_I;
        case AT_KEY_J:
            return ImGuiKey_J;
        case AT_KEY_K:
            return ImGuiKey_K;
        case AT_KEY_L:
            return ImGuiKey_L;
        case AT_KEY_M:
            return ImGuiKey_M;
        case AT_KEY_N:
            return ImGuiKey_N;
        case AT_KEY_O:
            return ImGuiKey_O;
        case AT_KEY_P:
            return ImGuiKey_P;
        case AT_KEY_Q:
            return ImGuiKey_Q;
        case AT_KEY_R:
            return ImGuiKey_R;
        case AT_KEY_S:
            return ImGuiKey_S;
        case AT_KEY_T:
            return ImGuiKey_T;
        case AT_KEY_U:
            return ImGuiKey_U;
        case AT_KEY_V:
            return ImGuiKey_V;
        case AT_KEY_W:
            return ImGuiKey_W;
        case AT_KEY_X:
            return ImGuiKey_X;
        case AT_KEY_Y:
            return ImGuiKey_Y;
        case AT_KEY_Z:
            return ImGuiKey_Z;
        case AT_KEY_LEFT_BRACKET:
            return ImGuiKey_LeftBracket;
        case AT_KEY_BACKSLASH:
            return ImGuiKey_Backslash;
        case AT_KEY_RIGHT_BRACKET:
            return ImGuiKey_RightBracket;
        case AT_KEY_GRAVE_ACCENT:
            return ImGuiKey_GraveAccent;
        case AT_KEY_WORLD_1:
            return ImGuiKey_None;
        case AT_KEY_WORLD_2:
            return ImGuiKey_None;
        case AT_KEY_ESCAPE:
            return ImGuiKey_Escape;
        case AT_KEY_ENTER:
            return ImGuiKey_Enter;
        case AT_KEY_TAB:
            return ImGuiKey_Tab;
        case AT_KEY_BACKSPACE:
            return ImGuiKey_Backspace;
        case AT_KEY_INSERT:
            return ImGuiKey_Insert;
        case AT_KEY_DELETE:
            return ImGuiKey_Delete;
        case AT_KEY_RIGHT:
            return ImGuiKey_RightArrow;
        case AT_KEY_LEFT:
            return ImGuiKey_LeftArrow;
        case AT_KEY_DOWN:
            return ImGuiKey_DownArrow;
        case AT_KEY_UP:
            return ImGuiKey_UpArrow;
        case AT_KEY_PAGE_UP:
            return ImGuiKey_PageUp;
        case AT_KEY_PAGE_DOWN:
            return ImGuiKey_PageDown;
        case AT_KEY_HOME:
            return ImGuiKey_Home;
        case AT_KEY_END:
            return ImGuiKey_End;
        case AT_KEY_CAPS_LOCK:
            return ImGuiKey_CapsLock;
        case AT_KEY_SCROLL_LOCK:
            return ImGuiKey_ScrollLock;
        case AT_KEY_NUM_LOCK:
            return ImGuiKey_NumLock;
        case AT_KEY_PRINT_SCREEN:
            return ImGuiKey_PrintScreen;
        case AT_KEY_PAUSE:
            return ImGuiKey_Pause;
        case AT_KEY_F1:
            return ImGuiKey_F1;
        case AT_KEY_F2:
            return ImGuiKey_F2;
        case AT_KEY_F3:
            return ImGuiKey_F3;
        case AT_KEY_F4:
            return ImGuiKey_F4;
        case AT_KEY_F5:
            return ImGuiKey_F5;
        case AT_KEY_F6:
            return ImGuiKey_F6;
        case AT_KEY_F7:
            return ImGuiKey_F7;
        case AT_KEY_F8:
            return ImGuiKey_F8;
        case AT_KEY_F9:
            return ImGuiKey_F9;
        case AT_KEY_F10:
            return ImGuiKey_F10;
        case AT_KEY_F11:
            return ImGuiKey_F11;
        case AT_KEY_F12:
            return ImGuiKey_F12;
        case AT_KEY_F13:
            return ImGuiKey_F13;
        case AT_KEY_F14:
            return ImGuiKey_F14;
        case AT_KEY_F15:
            return ImGuiKey_F15;
        case AT_KEY_F16:
            return ImGuiKey_F16;
        case AT_KEY_F17:
            return ImGuiKey_F17;
        case AT_KEY_F18:
            return ImGuiKey_F18;
        case AT_KEY_F19:
            return ImGuiKey_F19;
        case AT_KEY_F20:
            return ImGuiKey_F20;
        case AT_KEY_F21:
            return ImGuiKey_F21;
        case AT_KEY_F22:
            return ImGuiKey_F22;
        case AT_KEY_F23:
            return ImGuiKey_F23;
        case AT_KEY_F24:
            return ImGuiKey_F24;
        case AT_KEY_F25:
            return ImGuiKey_None;
        case AT_KEY_KP_0:
            return ImGuiKey_Keypad0;
        case AT_KEY_KP_1:
            return ImGuiKey_Keypad1;
        case AT_KEY_KP_2:
            return ImGuiKey_Keypad2;
        case AT_KEY_KP_3:
            return ImGuiKey_Keypad3;
        case AT_KEY_KP_4:
            return ImGuiKey_Keypad4;
        case AT_KEY_KP_5:
            return ImGuiKey_Keypad5;
        case AT_KEY_KP_6:
            return ImGuiKey_Keypad6;
        case AT_KEY_KP_7:
            return ImGuiKey_Keypad7;
        case AT_KEY_KP_8:
            return ImGuiKey_Keypad8;
        case AT_KEY_KP_9:
            return ImGuiKey_Keypad9;
        case AT_KEY_KP_DECIMAL:
            return ImGuiKey_KeypadDecimal;
        case AT_KEY_KP_DIVIDE:
            return ImGuiKey_KeypadDivide;
        case AT_KEY_KP_MULTIPLY:
            return ImGuiKey_KeypadMultiply;
        case AT_KEY_KP_SUBTRACT:
            return ImGuiKey_KeypadSubtract;
        case AT_KEY_KP_ADD:
            return ImGuiKey_KeypadAdd;
        case AT_KEY_KP_ENTER:
            return ImGuiKey_KeypadEnter;
        case AT_KEY_KP_EQUAL:
            return ImGuiKey_KeypadEqual;
        case AT_KEY_LEFT_SHIFT:
            return ImGuiKey_LeftShift;
        case AT_KEY_LEFT_CONTROL:
            return ImGuiKey_LeftCtrl;
        case AT_KEY_LEFT_ALT:
            return ImGuiKey_LeftAlt;
        case AT_KEY_LEFT_SUPER:
            return ImGuiKey_LeftSuper;
        case AT_KEY_RIGHT_SHIFT:
            return ImGuiKey_RightShift;
        case AT_KEY_RIGHT_CONTROL:
            return ImGuiKey_RightCtrl;
        case AT_KEY_RIGHT_ALT:
            return ImGuiKey_RightAlt;
        case AT_KEY_RIGHT_SUPER:
            return ImGuiKey_RightSuper;
        case AT_KEY_MENU:
            return ImGuiKey_Menu;
        default: {
            AT_CORE_WARN("Unknown keycode: {0}", keycode);
            return ImGuiKey_None;
        }
    }
}