#include "raylib.h"
#include "../include/auth.h"
#include "../include/ui.h"
#include "../include/fileio.h"
#include <string.h>
#include <stdio.h>

// ══════════════════════════════════════════
//   InitAuthScreen
//   Sets up positions of all UI elements.
//   startAsRegister = true if user clicked
//   "Register" on the welcome screen.
// ══════════════════════════════════════════
void InitAuthScreen(AuthScreen *as, bool startAsRegister)
{
    int cx = 860 / 2; // horizontal center

    // Card is 360px wide, centered
    int cardX = cx - 180;
    int cardY = 100;

    as->mode = startAsRegister ? AUTH_REGISTER : AUTH_LOGIN;

    // ── Input fields ──
    as->usernameField = (InputField){
        .rect = {cardX + 20, cardY + 110, 320, 44},
        .placeholder = "enter your username",
        .isPassword = false};

    as->passwordField = (InputField){
        .rect = {cardX + 20, cardY + 190, 320, 44},
        .placeholder = "enter your password",
        .isPassword = true // shows *** instead of text
    };

    // ── Buttons ──
    as->submitBtn = (Button){
        .rect = {cardX + 20, cardY + 260, 320, 48},
        .label = startAsRegister ? "Register" : "Login"};

    as->switchBtn = (Button){
        .rect = {cardX + 60, cardY + 326, 240, 36},
        .label = startAsRegister
                     ? "Already registered? Login"
                     : "No account? Register here"};

    as->backBtn = (Button){
        .rect = {cardX + 120, cardY + 374, 120, 32},
        .label = "< Back"};

    // Clear message
    memset(as->message, 0, sizeof(as->message));
    as->messageTimer = 0;
    as->messageIsOk = false;
}

// ══════════════════════════════════════════
//   UpdateAuthScreen
//   Handles all logic — typing, clicking,
//   validating, saving.
//   loggedInUser is filled when login succeeds.
// ══════════════════════════════════════════
AppScreen UpdateAuthScreen(AuthScreen *as, char *loggedInUser)
{
    /* int cardX = 860 / 2 - 180;
    int cardY = 100; */

    // ── Activate input field on click ──
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        as->usernameField.active = CheckCollisionPointRec(
            GetMousePosition(), as->usernameField.rect);

        as->passwordField.active = CheckCollisionPointRec(
            GetMousePosition(), as->passwordField.rect);
    }

    // ── Tab key switches between fields ──
    if (IsKeyPressed(KEY_TAB))
    {
        if (as->usernameField.active)
        {
            as->usernameField.active = false;
            as->passwordField.active = true;
        }
        else
        {
            as->passwordField.active = false;
            as->usernameField.active = true;
        }
    }

    // ── Handle keyboard input for active field ──
    HandleInputField(&as->usernameField);
    HandleInputField(&as->passwordField);

    // ── Enter key submits the form ──
    if (IsKeyPressed(KEY_ENTER))
        goto submit;

    // ── Submit button clicked ──
    if (IsButtonClicked(&as->submitBtn))
    {
    submit:;

        const char *username = as->usernameField.text;
        const char *password = as->passwordField.text;

        // Validate — fields must not be empty
        if (strlen(username) == 0 || strlen(password) == 0)
        {
            strcpy(as->message, "All fields are required!");
            as->messageIsOk = false;
            as->messageTimer = 2.5f;
            return SCREEN_AUTH;
        }

        if (as->mode == AUTH_REGISTER)
        {
            // ── Registration logic ──
            if (UserExists(username))
            {
                strcpy(as->message, "Username already taken!");
                as->messageIsOk = false;
                as->messageTimer = 2.5f;
                return SCREEN_AUTH;
            }

            // Save new user to file
            User newUser;
            strncpy(newUser.username, username, MAX_USERNAME - 1);
            strncpy(newUser.password, password, MAX_PASSWORD - 1);
            SaveUser(&newUser);

            // Log them in immediately after registering
            strncpy(loggedInUser, username, MAX_USERNAME - 1);
            strcpy(as->message, "Account created! Welcome!");
            as->messageIsOk = true;
            as->messageTimer = 2.0f;
            return SCREEN_CHAT;
        }
        else
        {
            // ── Login logic ──
            if (ValidateLogin(username, password))
            {
                strncpy(loggedInUser, username, MAX_USERNAME - 1);
                strcpy(as->message, "Login successful!");
                as->messageIsOk = true;
                as->messageTimer = 2.0f;
                return SCREEN_CHAT;
            }
            else
            {
                strcpy(as->message, "Invalid username or password!");
                as->messageIsOk = false;
                as->messageTimer = 2.5f;
                return SCREEN_AUTH;
            }
        }
    }

    // ── Switch between login / register ──
    if (IsButtonClicked(&as->switchBtn))
    {
        as->mode = (as->mode == AUTH_LOGIN) ? AUTH_REGISTER : AUTH_LOGIN;

        // Update button labels
        as->submitBtn.label = (as->mode == AUTH_REGISTER)
                                  ? "Register"
                                  : "Login";
        as->switchBtn.label = (as->mode == AUTH_REGISTER)
                                  ? "Already registered? Login"
                                  : "No account? Register here";

        // Clear the fields when switching
        ClearInputField(&as->usernameField);
        ClearInputField(&as->passwordField);
        memset(as->message, 0, sizeof(as->message));
        as->messageTimer = 0;
        return SCREEN_AUTH;
    }

    // ── Back button ──
    if (IsButtonClicked(&as->backBtn))
    {
        ClearInputField(&as->usernameField);
        ClearInputField(&as->passwordField);
        return SCREEN_WELCOME;
    }

    return SCREEN_AUTH;
}

// ══════════════════════════════════════════
//   DrawAuthScreen
//   Draws the full login/register form.
// ══════════════════════════════════════════
void DrawAuthScreen(AuthScreen *as)
{
    int cx = 860 / 2;
    int cardX = cx - 180;
    int cardY = 100;
    int cardW = 360;
    int cardH = 420;

    // ── Background ──
    ClearBackground(COLOR_BG);

    // ── Subtle top gradient strip ──
    DrawRectangle(0, 0, 860, 4, COLOR_ACCENT);

    // ── Card background ──
    DrawPanel(cardX, cardY, cardW, cardH, COLOR_CARD, COLOR_BORDER);

    // ── Card title ──
    const char *title = (as->mode == AUTH_REGISTER)
                            ? "Create Account"
                            : "Welcome Back";
    DrawCenteredText(title, cardY + 24, 26, COLOR_TEXT);

    // ── Subtitle ──
    const char *sub = (as->mode == AUTH_REGISTER)
                          ? "Join ChatLink today"
                          : "Login to your account";
    DrawCenteredText(sub, cardY + 60, 16, COLOR_MUTED);

    // ── Field labels ──
    DrawText("USERNAME",
             cardX + 20, cardY + 86, 12, COLOR_MUTED);
    DrawText("PASSWORD",
             cardX + 20, cardY + 166, 12, COLOR_MUTED);

    // ── Input fields ──
    DrawInputField(&as->usernameField);
    DrawInputField(&as->passwordField);

    // ── Submit button ──
    DrawButton(&as->submitBtn, COLOR_ACCENT, WHITE);

    // ── Switch mode link ──
    DrawButton(&as->switchBtn, COLOR_BG, COLOR_TEAL);

    // ── Back button ──
    DrawButton(&as->backBtn, COLOR_BG, COLOR_MUTED);

    // ── Error / success message ──
    if (as->messageTimer > 0)
    {
        as->messageTimer -= GetFrameTime();

        Color msgColor = as->messageIsOk ? COLOR_GREEN : COLOR_ACCENT;
        DrawCenteredText(as->message, cardY + cardH + 16, 16, msgColor);
    }
}