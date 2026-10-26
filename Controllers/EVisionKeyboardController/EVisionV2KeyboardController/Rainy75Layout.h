/*---------------------------------------------------------*\
| Rainy75Layout.h                                           |
|                                                           |
|   Wired Rainy 75 ANSI layout and EVision LED addresses     |
|   Derived from the device keymap (command 0x07)             |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later                |
\*---------------------------------------------------------*/

#pragma once

#include "RGBControllerKeyNames.h"

#define RAINY75_LED_COUNT 81
#define RAINY75_MATRIX_WIDTH 16
#define RAINY75_MATRIX_HEIGHT 6

struct Rainy75Key
{
    const char* name;
    unsigned char index;
    unsigned char row;
    unsigned char column;
};

static const Rainy75Key rainy75_keys[RAINY75_LED_COUNT] =
{
    { KEY_EN_ESCAPE,  0, 0,  0 },
    { KEY_EN_F1,  1, 0,  2 },
    { KEY_EN_F2,  2, 0,  3 },
    { KEY_EN_F3,  3, 0,  4 },
    { KEY_EN_F4,  4, 0,  5 },
    { KEY_EN_F5,  5, 0,  6 },
    { KEY_EN_F6,  6, 0,  7 },
    { KEY_EN_F7,  7, 0,  8 },
    { KEY_EN_F8,  8, 0,  9 },
    { KEY_EN_F9,  9, 0, 10 },
    { KEY_EN_F10, 10, 0, 11 },
    { KEY_EN_F11, 11, 0, 12 },
    { KEY_EN_F12, 12, 0, 13 },
    { KEY_EN_DELETE, 13, 0, 14 },
    { KEY_EN_HOME, 14, 0, 15 },
    { KEY_EN_BACK_TICK, 16, 1,  0 },
    { KEY_EN_1, 17, 1,  1 },
    { KEY_EN_2, 18, 1,  2 },
    { KEY_EN_3, 19, 1,  3 },
    { KEY_EN_4, 20, 1,  4 },
    { KEY_EN_5, 21, 1,  5 },
    { KEY_EN_6, 22, 1,  6 },
    { KEY_EN_7, 23, 1,  7 },
    { KEY_EN_8, 24, 1,  8 },
    { KEY_EN_9, 25, 1,  9 },
    { KEY_EN_0, 26, 1, 10 },
    { KEY_EN_MINUS, 27, 1, 11 },
    { KEY_EN_EQUALS, 28, 1, 12 },
    { KEY_EN_BACKSPACE, 29, 1, 14 },
    { KEY_EN_END, 30, 1, 15 },
    { KEY_EN_TAB, 32, 2,  0 },
    { KEY_EN_Q, 33, 2,  2 },
    { KEY_EN_W, 34, 2,  3 },
    { KEY_EN_E, 35, 2,  4 },
    { KEY_EN_R, 36, 2,  5 },
    { KEY_EN_T, 37, 2,  6 },
    { KEY_EN_Y, 38, 2,  7 },
    { KEY_EN_U, 39, 2,  8 },
    { KEY_EN_I, 40, 2,  9 },
    { KEY_EN_O, 41, 2, 10 },
    { KEY_EN_P, 42, 2, 11 },
    { KEY_EN_LEFT_BRACKET, 43, 2, 12 },
    { KEY_EN_RIGHT_BRACKET, 44, 2, 13 },
    { KEY_EN_ANSI_BACK_SLASH, 45, 2, 14 },
    { KEY_EN_PAGE_UP, 62, 2, 15 },
    { KEY_EN_CAPS_LOCK, 48, 3,  0 },
    { KEY_EN_A, 49, 3,  2 },
    { KEY_EN_S, 50, 3,  3 },
    { KEY_EN_D, 51, 3,  4 },
    { KEY_EN_F, 52, 3,  5 },
    { KEY_EN_G, 53, 3,  6 },
    { KEY_EN_H, 54, 3,  7 },
    { KEY_EN_J, 55, 3,  8 },
    { KEY_EN_K, 56, 3,  9 },
    { KEY_EN_L, 57, 3, 10 },
    { KEY_EN_SEMICOLON, 58, 3, 11 },
    { KEY_EN_QUOTE, 59, 3, 12 },
    { KEY_EN_ANSI_ENTER, 61, 3, 14 },
    { KEY_EN_PAGE_DOWN, 63, 3, 15 },
    { KEY_EN_LEFT_SHIFT, 64, 4,  0 },
    { KEY_EN_Z, 66, 4,  2 },
    { KEY_EN_X, 67, 4,  3 },
    { KEY_EN_C, 68, 4,  4 },
    { KEY_EN_V, 69, 4,  5 },
    { KEY_EN_B, 70, 4,  6 },
    { KEY_EN_N, 71, 4,  7 },
    { KEY_EN_M, 72, 4,  8 },
    { KEY_EN_COMMA, 73, 4,  9 },
    { KEY_EN_PERIOD, 74, 4, 10 },
    { KEY_EN_FORWARD_SLASH, 75, 4, 11 },
    { KEY_EN_RIGHT_SHIFT, 77, 4, 13 },
    { KEY_EN_UP_ARROW, 78, 4, 14 },
    { KEY_EN_LEFT_CONTROL, 80, 5,  0 },
    { KEY_EN_LEFT_WINDOWS, 81, 5,  1 },
    { KEY_EN_LEFT_ALT, 82, 5,  2 },
    { KEY_EN_SPACE, 88, 5,  6 },
    { KEY_EN_RIGHT_ALT, 91, 5, 10 },
    { KEY_EN_RIGHT_FUNCTION, 90, 5, 11 },
    { KEY_EN_LEFT_ARROW, 93, 5, 13 },
    { KEY_EN_DOWN_ARROW, 94, 5, 14 },
    { KEY_EN_RIGHT_ARROW, 95, 5, 15 },
};
