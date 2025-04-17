#pragma once

#define LIGHT_BLUE  "\033[1;36m"
#define BLUE        "\x1B[34m"
#define RED         "\033[31m"
#define PURPLE      "\033[35m"
#define YELLOW      "\033[33m"
#define GREEN       "\x1b[32m"
#define CLEAR       "\033[0m"

#define PURPLE_TEXT(text)     PURPLE text CLEAR
#define BLUE_TEXT(text)       BLUE text CLEAR
#define LIGHT_BLUE_TEXT(text) LIGHT_BLUE text CLEAR
#define GREEN_TEXT(text)      GREEN text CLEAR
#define RED_TEXT(text)        RED text CLEAR
#define YELLOW_TEXT(text)     YELLOW text CLEAR