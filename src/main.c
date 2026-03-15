#include "raylib.h"
#include "../include/screen.h"
#include "../include/ui.h"
#include "../include/welcome.h"
#include "../include/auth.h"
#include "../include/chat.h"
#include "../include/search.h"
#include <string.h>
#include <stdio.h>

int main(void)
{
    InitWindow(860, 580, "ChatLink - One on One Chat");
    SetTargetFPS(60);

    WelcomeScreen welcomeScreen;
    AuthScreen authScreen;
    ChatScreen chatScreen;
    SearchScreen searchScreen;

    AppScreen currentScreen = SCREEN_WELCOME; /* Tracks the active screen */
    char loggedInUser[MAX_USERNAME] = {0};    /* Written by auth on login, cleared on logout */
    char peerToOpen[MAX_USERNAME] = {0};      /* Written by search screen, consumed by chat screen */

    /* Toast notification state */
    char toastMsg[128] = {0};
    float toastTimer = 0.0f; /* Visible while > 0, counts down each frame */
    bool toastIsOk = true;   /* true = green, false = red */

    InitWelcomeScreen(&welcomeScreen);

    /* Main loop — runs at 60 FPS until window is closed */
    while (!WindowShouldClose())
    {
        AppScreen nextScreen = currentScreen;

        /* UPDATE — call active screen's logic, handle any screen transition */
        switch (currentScreen)
        {
        case SCREEN_WELCOME:
        {
            AppScreen result = UpdateWelcomeScreen(&welcomeScreen);
            if (result != SCREEN_WELCOME)
            {
                /* Pass hovered button to auth so it opens in the correct mode */
                bool wantsRegister = welcomeScreen.registerBtn.hovered;
                InitAuthScreen(&authScreen, wantsRegister);
                nextScreen = SCREEN_AUTH;
            }
            break;
        }

        case SCREEN_AUTH:
        {
            AppScreen result = UpdateAuthScreen(&authScreen, loggedInUser);
            if (result == SCREEN_CHAT)
            {
                InitChatScreen(&chatScreen, loggedInUser);
                snprintf(toastMsg, sizeof(toastMsg), "Welcome, %s!", loggedInUser);
                toastTimer = 2.5f;
                toastIsOk = true;
            }
            nextScreen = result;
            break;
        }

        case SCREEN_CHAT:
        {
            /* If returning from search with a peer selected, open that chat first */
            if (strlen(peerToOpen) > 0)
            {
                LoadChatForPeer(&chatScreen, peerToOpen);
                memset(peerToOpen, 0, sizeof(peerToOpen));
            }

            AppScreen result = UpdateChatScreen(&chatScreen, loggedInUser);

            if (result == SCREEN_SEARCH)
            {
                InitSearchScreen(&searchScreen, loggedInUser);
                nextScreen = SCREEN_SEARCH;
            }
            else if (result == SCREEN_WELCOME)
            {
                /* Logout or account deletion — reset welcome and show goodbye toast */
                snprintf(toastMsg, sizeof(toastMsg), "Goodbye! See you soon.");
                toastTimer = 2.0f;
                toastIsOk = true;
                InitWelcomeScreen(&welcomeScreen);
                nextScreen = SCREEN_WELCOME;
            }
            else
            {
                nextScreen = result;
            }
            break;
        }

        case SCREEN_SEARCH:
        {
            AppScreen result = UpdateSearchScreen(&searchScreen, peerToOpen);
            if (result == SCREEN_CHAT)
            {
                /* Reload messages in case new ones arrived while on search screen */
                LoadMessages(chatScreen.allMessages, &chatScreen.allMessageCount);
                nextScreen = SCREEN_CHAT;
            }
            else
            {
                nextScreen = result;
            }
            break;
        }

        default:
            break;
        }

        currentScreen = nextScreen;

        /* DRAW — render active screen, toast always drawn on top */
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
        case SCREEN_SEARCH:
            DrawSearchScreen(&searchScreen);
            break;
        default:
            ClearBackground(COLOR_BG);
            break;
        }
        DrawToast(toastMsg, toastIsOk, &toastTimer);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}