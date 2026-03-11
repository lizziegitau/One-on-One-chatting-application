#ifndef WELCOME_H
#define WELCOME_H

#include "../include/ui.h"
#include "../include/screen.h"

typedef struct
{
    Button loginBtn;
    Button registerBtn;
} WelcomeScreen;

// ── Function declarations ──
void InitWelcomeScreen(WelcomeScreen *ws);
AppScreen UpdateWelcomeScreen(WelcomeScreen *ws);
void DrawWelcomeScreen(WelcomeScreen *ws);

#endif