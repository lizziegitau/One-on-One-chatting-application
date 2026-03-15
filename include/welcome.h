#ifndef WELCOME_H
#define WELCOME_H

#include "../include/ui.h"
#include "../include/screen.h"

/* Welcome Struct */
typedef struct
{
    Button loginBtn;    /* Login Button */
    Button registerBtn; /* Register Button */
} WelcomeScreen;

/* Function Declarations */
void InitWelcomeScreen(WelcomeScreen *ws);
AppScreen UpdateWelcomeScreen(WelcomeScreen *ws);
void DrawWelcomeScreen(WelcomeScreen *ws);

#endif