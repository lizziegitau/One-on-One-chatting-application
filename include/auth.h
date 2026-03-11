#ifndef AUTH_H
#define AUTH_H

#include "../include/ui.h"
#include "../include/screen.h"

// ── Which mode is the auth screen in? ──
typedef enum
{
    AUTH_LOGIN,   // showing the login form
    AUTH_REGISTER // showing the register form
} AuthMode;

// ── Holds everything the Auth screen needs ──
typedef struct
{
    AuthMode mode; // login or register?

    InputField usernameField; // username text box
    InputField passwordField; // password text box

    Button submitBtn; // Login / Register button
    Button switchBtn; // "Switch to Register" / "Switch to Login"
    Button backBtn;   // Back to Welcome screen

    char message[128];  // error or success message to display
    bool messageIsOk;   // green if true, red if false
    float messageTimer; // how long to show the message

    // Store which button was clicked on welcome screen
    // so we know whether to start in login or register mode
    bool startAsRegister;
} AuthScreen;

// ── Function declarations ──
void InitAuthScreen(AuthScreen *as, bool startAsRegister);
AppScreen UpdateAuthScreen(AuthScreen *as, char *loggedInUser);
void DrawAuthScreen(AuthScreen *as);

#endif