#include "raylib.h"
#include "../include/auth.h"
#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"
#include <string.h>
#include <stdio.h>

/* Sets up input field positions, button labels and initial mode.
 * startAsRegister=true opens in Register mode, false opens in Login mode. */
void InitAuthScreen(AuthScreen *as, bool startAsRegister)
{
    int cx = 860 / 2;
    int cardX = cx - 180; /* Card is 360px wide, centered */
    int cardY = 100;

    as->mode = startAsRegister ? AUTH_REGISTER : AUTH_LOGIN;

    /* Input fields */
    as->usernameField = (InputField){
        .rect = {cardX + 20, cardY + 110, 320, 44},
        .placeholder = "enter your username",
        .isPassword = false};

    as->passwordField = (InputField){
        .rect = {cardX + 20, cardY + 190, 320, 44},
        .placeholder = "enter your password",
        .isPassword = true}; /* Displays *** instead of typed text */

    /* Buttons — labels change depending on starting mode */
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

    /* Clear any leftover message from a previous visit */
    memset(as->message, 0, sizeof(as->message));
    as->messageTimer = 0;
    as->messageIsOk = false;
}

/* Handles all Auth Screen logic — field input, form submission, mode switching and navigation */
AppScreen UpdateAuthScreen(AuthScreen *as, char *loggedInUser)
{
    /* Activate the clicked field, deactivate the other */
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        as->usernameField.active = CheckCollisionPointRec(
            GetMousePosition(), as->usernameField.rect);
        as->passwordField.active = CheckCollisionPointRec(
            GetMousePosition(), as->passwordField.rect);
    }

    /* Tab key toggles focus between username and password fields */
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

    /* Pass keyboard input to whichever field is active */
    HandleInputField(&as->usernameField);
    HandleInputField(&as->passwordField);

    /* Enter key triggers form submission same as clicking the submit button */
    if (IsKeyPressed(KEY_ENTER))
        goto submit;

    if (IsButtonClicked(&as->submitBtn))
    {
    submit:;
        const char *username = as->usernameField.text;
        const char *password = as->passwordField.text;

        /* Both fields must be filled before submitting */
        if (strlen(username) == 0 || strlen(password) == 0)
        {
            strcpy(as->message, "All fields are required!");
            as->messageIsOk = false;
            as->messageTimer = 2.5f;
            return SCREEN_AUTH;
        }

        if (as->mode == AUTH_REGISTER)
        {
            /* Registration — check for duplicate username before saving */
            if (UserExists(username))
            {
                strcpy(as->message, "Username already taken!");
                as->messageIsOk = false;
                as->messageTimer = 2.5f;
                return SCREEN_AUTH;
            }

            /* Save new user to users.txt and log them in immediately */
            User newUser;
            strncpy(newUser.username, username, MAX_USERNAME - 1);
            strncpy(newUser.password, password, MAX_PASSWORD - 1);
            SaveUser(&newUser);

            strncpy(loggedInUser, username, MAX_USERNAME - 1);
            strcpy(as->message, "Account created! Welcome!");
            as->messageIsOk = true;
            as->messageTimer = 2.0f;
            return SCREEN_CHAT;
        }
        else
        {
            /* Login — validate credentials against users.txt */
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

    /* Switch button — toggle between Login and Register mode */
    if (IsButtonClicked(&as->switchBtn))
    {
        as->mode = (as->mode == AUTH_LOGIN) ? AUTH_REGISTER : AUTH_LOGIN;

        /* Update button labels to match the new mode */
        as->submitBtn.label = (as->mode == AUTH_REGISTER) ? "Register" : "Login";
        as->switchBtn.label = (as->mode == AUTH_REGISTER)
                                  ? "Already registered? Login"
                                  : "No account? Register here";

        /* Clear fields and message when switching modes */
        ClearInputField(&as->usernameField);
        ClearInputField(&as->passwordField);
        memset(as->message, 0, sizeof(as->message));
        as->messageTimer = 0;
        return SCREEN_AUTH;
    }

    /* Back button — return to Welcome Screen */
    if (IsButtonClicked(&as->backBtn))
    {
        ClearInputField(&as->usernameField);
        ClearInputField(&as->passwordField);
        return SCREEN_WELCOME;
    }

    return SCREEN_AUTH;
}

/* Draws the full Auth Screen — card, title, field labels, inputs, buttons and feedback message */
void DrawAuthScreen(AuthScreen *as)
{
    int cx = 860 / 2;
    int cardX = cx - 180;
    int cardY = 100;
    int cardW = 360;
    int cardH = 420;

    ClearBackground(COLOR_BG);

    /* Thin accent bar along the top edge */
    DrawRectangle(0, 0, 860, 4, COLOR_ACCENT);

    /* Card background */
    DrawPanel(cardX, cardY, cardW, cardH, COLOR_CARD, COLOR_BORDER);

    /* Title and subtitle change based on current mode */
    const char *title = (as->mode == AUTH_REGISTER) ? "Create Account" : "Welcome Back";
    const char *sub = (as->mode == AUTH_REGISTER) ? "Join ChatLink today" : "Login to your account";
    DrawCenteredText(title, cardY + 24, 26, COLOR_TEXT);
    DrawCenteredText(sub, cardY + 60, 16, COLOR_MUTED);

    /* Field labels */
    DrawText("USERNAME", cardX + 20, cardY + 86, 12, COLOR_MUTED);
    DrawText("PASSWORD", cardX + 20, cardY + 166, 12, COLOR_MUTED);

    /* Input fields, submit button, switch link and back button */
    DrawInputField(&as->usernameField);
    DrawInputField(&as->passwordField);
    DrawButton(&as->submitBtn, COLOR_ACCENT, WHITE);
    DrawButton(&as->switchBtn, COLOR_BG, COLOR_TEAL);
    DrawButton(&as->backBtn, COLOR_BG, COLOR_MUTED);

    /* Feedback message — green for success, red for error, fades after timer expires */
    if (as->messageTimer > 0)
    {
        as->messageTimer -= GetFrameTime();
        Color msgColor = as->messageIsOk ? COLOR_GREEN : COLOR_ACCENT;
        DrawCenteredText(as->message, cardY + cardH + 16, 16, msgColor);
    }
}