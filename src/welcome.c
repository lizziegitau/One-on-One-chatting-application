#include "raylib.h"
#include "../include/welcome.h"
#include "../include/ui.h"
#include "../include/screen.h"
#include <math.h> /* sinf() — used for the floating logo animation */

/* Sets up button positions and labels for the Welcome Screen */
void InitWelcomeScreen(WelcomeScreen *ws)
{
    int cx = 860 / 2; /* Horizontal center of the screen */

    ws->loginBtn = (Button){
        .rect = {cx - 105, 340, 200, 48},
        .label = "Login"};

    ws->registerBtn = (Button){
        .rect = {cx + 105, 340, 200, 48},
        .label = "Register"};
}

/* Checks for button clicks and returns the next screen.
 * Both buttons lead to SCREEN_AUTH — main.c checks which was hovered
 * to determine whether to open Auth in Login or Register mode. */
AppScreen UpdateWelcomeScreen(WelcomeScreen *ws)
{
    if (IsButtonClicked(&ws->loginBtn))
        return SCREEN_AUTH;
    if (IsButtonClicked(&ws->registerBtn))
        return SCREEN_AUTH;

    return SCREEN_WELCOME; /* No click — stay on this screen */
}

/* Draws the full Welcome Screen every frame */
void DrawWelcomeScreen(WelcomeScreen *ws)
{
    int sw = 860;

    ClearBackground(COLOR_BG);

    /* Floating animation — sinf() oscillates between -1 and +1 to create
     * a smooth up-down movement applied to the logo position each frame */
    float floatY = sinf((float)GetTime() * 1.5f) * 6.0f;

    /* Logo — two overlapping circles (orange outer, red inner) with a
     * white chat bubble icon drawn on top using a rectangle and triangle */
    DrawCircle(sw / 2, 160 + (int)floatY, 52, COLOR_ACCENT2);
    DrawCircle(sw / 2, 160 + (int)floatY, 46, COLOR_ACCENT);
    DrawRectangleRounded(
        (Rectangle){sw / 2 - 18, 148 + floatY, 36, 24}, 0.4f, 6, WHITE);
    DrawTriangle(/* Bubble tail */
                 (Vector2){sw / 2 - 6, 172 + floatY},
                 (Vector2){sw / 2 - 18, 180 + floatY},
                 (Vector2){sw / 2 + 2, 172 + floatY},
                 WHITE);

    /* App title, subtitle and description */
    DrawCenteredText("ChatLink", 230, 52, COLOR_ACCENT);
    DrawCenteredText("One-on-One Chat Application", 292, 20, COLOR_TEXT);
    DrawCenteredText("Register, login, search users and send messages", 322, 16, COLOR_MUTED);

    /* Login (solid red) and Register (dark with text) buttons */
    DrawButton(&ws->loginBtn, COLOR_ACCENT, WHITE);
    DrawButton(&ws->registerBtn, COLOR_CARD, COLOR_TEXT);

    /* Feature labels row — four icons with captions */
    const char *features[] = {"Secure", "Fast", "Search", "Stored"};
    const char *icons[] = {"[*]", "[>]", "[?]", "[S]"};
    int startX = sw / 2 - 160;

    for (int i = 0; i < 4; i++)
    {
        int fx = startX + i * 86;
        DrawText(icons[i], fx, 440, 18, COLOR_ACCENT);
        DrawText(features[i], fx, 464, 13, COLOR_MUTED);
    }

    /* Course credit line at the bottom */
    DrawCenteredText("Network & Distributed Programming", 540, 13, COLOR_MUTED);
}