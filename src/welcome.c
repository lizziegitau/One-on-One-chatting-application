#include "raylib.h"
#include "../include/welcome.h"
#include "../include/ui.h"
#include "../include/screen.h"
#include <math.h> // for sinf() used in the floating animation

void InitWelcomeScreen(WelcomeScreen *ws)
{
    int cx = 860 / 2; // center x of the screen

    // Login button — centered, 200px wide, 48px tall
    ws->loginBtn = (Button){
        .rect = {cx - 105, 340, 200, 48},
        .label = "Login"};

    // Register button — next to login button
    ws->registerBtn = (Button){
        .rect = {cx + 105, 340, 200, 48},
        .label = "Register"};
}

AppScreen UpdateWelcomeScreen(WelcomeScreen *ws)
{
    if (IsButtonClicked(&ws->loginBtn))
        return SCREEN_AUTH;
    if (IsButtonClicked(&ws->registerBtn))
        return SCREEN_AUTH;

    // Nothing clicked — stay on this screen
    return SCREEN_WELCOME;
}

void DrawWelcomeScreen(WelcomeScreen *ws)
{
    int sw = 860; // screen width
    /* int sh = 580; // screen height */

    // ── Background ──
    ClearBackground(COLOR_BG);

    // ── Animated floating offset ──
    // sinf() returns a value between -1 and +1
    // GetTime() increases every second
    // Together they create a smooth up-down float
    float floatY = sinf((float)GetTime() * 1.5f) * 6.0f;

    // ── Logo circle ──
    // A red-to-orange gradient circle as the app icon
    DrawCircle(sw / 2, 160 + (int)floatY, 52, COLOR_ACCENT2);
    DrawCircle(sw / 2, 160 + (int)floatY, 46, COLOR_ACCENT);

    // Chat bubble icon drawn as simple rectangles
    DrawRectangleRounded(
        (Rectangle){sw / 2 - 18, 148 + floatY, 36, 24},
        0.4f, 6, WHITE);
    // Little tail on the bubble
    DrawTriangle(
        (Vector2){sw / 2 - 6, 172 + floatY},
        (Vector2){sw / 2 - 18, 180 + floatY},
        (Vector2){sw / 2 + 2, 172 + floatY},
        WHITE);

    // ── App title ──
    DrawCenteredText("ChatLink", 230, 52, COLOR_ACCENT);

    // ── Subtitle ──
    DrawCenteredText("One-on-One Chat Application", 292, 20, COLOR_TEXT);

    // ── Small description ──
    DrawCenteredText("Register, login, search users and send messages",
                     322, 16, COLOR_MUTED);

    // ── Buttons ──
    // Login button — solid red
    DrawButton(&ws->loginBtn, COLOR_ACCENT, WHITE);

    // Register button — outlined style (dark bg, accent border)
    DrawButton(&ws->registerBtn, COLOR_CARD, COLOR_TEXT);

    // ── Feature icons row at the bottom ──
    // Four small labels showing app features
    const char *features[] = {"Secure", "Fast", "Search", "Stored"};
    const char *icons[] = {"[*]", "[>]", "[?]", "[S]"};
    int startX = sw / 2 - 160;

    for (int i = 0; i < 4; i++)
    {
        int fx = startX + i * 86;

        // Icon label
        DrawText(icons[i], fx, 440, 18, COLOR_ACCENT);

        // Feature name
        DrawText(features[i], fx, 464, 13, COLOR_MUTED);
    }

    // ── Bottom credit line ──
    DrawCenteredText("Network & Distributed Programming",
                     540, 13, COLOR_MUTED);
}