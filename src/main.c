#include "raylib.h"
#include <stdio.h>
#include "../include/screen.h"
#include "../include/ui.h"
#include "../include/welcome.h"
#include "../include/auth.h"
#include "../include/chat.h"

int main(void)
{
    InitWindow(860, 580, "ChatLink - One on One Chat");
    SetTargetFPS(60);

    WelcomeScreen welcomeScreen;
    AuthScreen authScreen;
    ChatScreen chatScreen;

    AppScreen currentScreen = SCREEN_WELCOME;
    char loggedInUser[50] = {0};

    char toastMsg[128] = {0};
    float toastTimer = 0.0f;
    bool toastIsOk = true;

    InitWelcomeScreen(&welcomeScreen);

    while (!WindowShouldClose())
    {
        // ── UPDATE ──
        AppScreen nextScreen = currentScreen;

        switch (currentScreen)
        {
        case SCREEN_WELCOME:
        {
            AppScreen result = UpdateWelcomeScreen(&welcomeScreen);
            if (result != SCREEN_WELCOME)
            {
                bool wantsRegister =
                    welcomeScreen.registerBtn.hovered;
                InitAuthScreen(&authScreen, wantsRegister);
                nextScreen = SCREEN_AUTH;
            }
            break;
        }

        case SCREEN_AUTH:
        {
            AppScreen result =
                UpdateAuthScreen(&authScreen, loggedInUser);
            if (result == SCREEN_CHAT)
            {
                // Initialize chat with logged in user
                InitChatScreen(&chatScreen, loggedInUser);
                snprintf(toastMsg, sizeof(toastMsg),
                         "Welcome, %s!", loggedInUser);
                toastTimer = 2.5f;
                toastIsOk = true;
            }
            nextScreen = result;
            break;
        }

        case SCREEN_CHAT:
        {
            AppScreen result =
                UpdateChatScreen(&chatScreen, loggedInUser);

            if (result == SCREEN_WELCOME)
            {
                // Logged out or deregistered
                snprintf(toastMsg, sizeof(toastMsg),
                         "Goodbye! See you soon.");
                toastTimer = 2.0f;
                toastIsOk = true;
                InitWelcomeScreen(&welcomeScreen);
            }
            nextScreen = result;
            break;
        }

        default:
            break;
        }

        currentScreen = nextScreen;

        // ── DRAW ──
        BeginDrawing();

        switch (currentScreen)
        {
        case SCREEN_WELCOME:
            DrawWelcomeScreen(&welcomeScreen);
            break;

        case SCREEN_AUTH:
            DrawAuthScreen(&authScreen);
            break;

        case SCREEN_CHAT:
            DrawChatScreen(&chatScreen);
            break;

        default:
            ClearBackground(COLOR_BG);
            DrawCenteredText("Coming soon...",
                             580 / 2, 20, COLOR_MUTED);
            break;
        }

        DrawToast(toastMsg, toastIsOk, &toastTimer);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}