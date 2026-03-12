#include "raylib.h"
#include "../include/search.h"
#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"
#include <string.h>
#include <stdio.h>

#define SCREEN_W 860
#define SCREEN_H 580

// ══════════════════════════════════════════
//   InitSearchScreen
//   Sets up positions of all UI elements.
//   Called once when switching to this screen.
// ══════════════════════════════════════════
void InitSearchScreen(SearchScreen *ss, const char *currentUser)
{
    int cx = SCREEN_W / 2;

    strncpy(ss->currentUser, currentUser, MAX_USERNAME - 1);
    ss->result = SEARCH_IDLE;
    ss->openChat = false;
    memset(ss->selectedPeer, 0, sizeof(ss->selectedPeer));

    // ── Search input field ──
    ss->searchField = (InputField){
        .rect = {cx - 200, 220, 320, 46},
        .placeholder = "Enter username to search...",
        .isPassword = false};

    // ── Search button — sits right of input ──
    ss->searchBtn = (Button){
        .rect = {cx + 130, 220, 110, 46},
        .label = "Search"};

    // ── Back button — top left ──
    ss->backBtn = (Button){
        .rect = {20, 20, 110, 38},
        .label = "< Back"};

    // ── Open Chat button — shown in result card ──
    ss->chatBtn = (Button){
        .rect = {cx - 80, 370, 160, 44},
        .label = "Open Chat"};
}

// ══════════════════════════════════════════
//   UpdateSearchScreen
//   Handles search logic and button clicks.
//   peerToOpen is filled when user clicks
//   "Open Chat" so main.c knows who to load.
// ══════════════════════════════════════════
AppScreen UpdateSearchScreen(SearchScreen *ss, char *peerToOpen)
{
    // ── Activate input on click ──
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        ss->searchField.active = CheckCollisionPointRec(
            GetMousePosition(), ss->searchField.rect);
    }

    // ── Handle keyboard input ──
    HandleInputField(&ss->searchField);

    // ── Perform search on button click or Enter ──
    bool doSearch = IsButtonClicked(&ss->searchBtn) ||
                    (IsKeyPressed(KEY_ENTER) &&
                     ss->searchField.active);

    if (doSearch && ss->searchField.textLength > 0)
    {
        const char *query = ss->searchField.text;

        // Can't search for yourself
        if (strcmp(query, ss->currentUser) == 0)
        {
            ss->result = SEARCH_NOT_FOUND;
        }
        else
        {
            // Load all users and look for a match
            User users[MAX_USERS];
            int count = 0;
            LoadUsers(users, &count);

            bool found = false;
            for (int i = 0; i < count; i++)
            {
                if (strcmp(users[i].username, query) == 0)
                {
                    ss->foundUser = users[i];
                    ss->result = SEARCH_FOUND;
                    found = true;
                    break;
                }
            }

            if (!found)
                ss->result = SEARCH_NOT_FOUND;
        }
    }

    // ── Open Chat button ──
    if (ss->result == SEARCH_FOUND &&
        IsButtonClicked(&ss->chatBtn))
    {
        // Tell main.c which peer to open
        strncpy(peerToOpen, ss->foundUser.username,
                MAX_USERNAME - 1);
        return SCREEN_CHAT;
    }

    // ── Back button ──
    if (IsButtonClicked(&ss->backBtn))
    {
        memset(peerToOpen, 0, MAX_USERNAME);
        return SCREEN_CHAT;
    }

    return SCREEN_SEARCH;
}

// ══════════════════════════════════════════
//   DrawSearchScreen
//   Draws the full search screen every frame.
// ══════════════════════════════════════════
void DrawSearchScreen(SearchScreen *ss)
{
    int cx = SCREEN_W / 2;

    // ── Background ──
    ClearBackground(COLOR_BG);

    // ── Top accent bar ──
    DrawRectangle(0, 0, SCREEN_W, 4, COLOR_ACCENT);

    // ── Back button ──
    DrawButton(&ss->backBtn, COLOR_CARD, COLOR_MUTED);

    // ── Page title ──
    DrawCenteredText("Search Users", 80, 32, COLOR_TEXT);
    DrawCenteredText("Find a registered user to start chatting",
                     122, 16, COLOR_MUTED);

    // ── Divider line ──
    DrawRectangle(cx - 200, 160, 440, 1, COLOR_BORDER);

    // ── Search label ──
    DrawText("USERNAME", cx - 200, 196, 12, COLOR_MUTED);

    // ── Search input + button ──
    DrawInputField(&ss->searchField);
    DrawButton(&ss->searchBtn, COLOR_ACCENT, WHITE);

    // ════════════════════════════════
    //   RESULT AREA
    // ════════════════════════════════
    if (ss->result == SEARCH_IDLE)
    {
        // Prompt text while waiting
        DrawCenteredText("Type a username and press Search",
                         290, 15, COLOR_MUTED);
    }
    else if (ss->result == SEARCH_NOT_FOUND)
    {
        // ── Not found card ──
        int cw = 340, ch = 80;
        int cx2 = SCREEN_W / 2 - cw / 2;

        DrawPanel(cx2, 280, cw, ch, COLOR_CARD, COLOR_BORDER);

        // X icon
        DrawText("✗", cx2 + 20, 280 + ch / 2 - 14, 28, COLOR_ACCENT);

        DrawText("User not found",
                 cx2 + 60, 280 + 16, 17, COLOR_TEXT);
        DrawText("No registered user with that username",
                 cx2 + 60, 280 + 40, 13, COLOR_MUTED);
    }
    else if (ss->result == SEARCH_FOUND)
    {
        // ── Found card ──
        int cw = 340, ch = 100;
        int cardX = SCREEN_W / 2 - cw / 2;
        int cardY = 272;

        // Card background with teal border (success color)
        DrawRectangle(cardX, cardY, cw, ch, COLOR_CARD);
        DrawRectangleLines(cardX, cardY, cw, ch, COLOR_TEAL);

        // Avatar
        DrawAvatar(cardX + 36, cardY + ch / 2, 22,
                   COLOR_TEAL, ss->foundUser.username[0]);

        // Username
        DrawText(ss->foundUser.username,
                 cardX + 68, cardY + 22, 18, COLOR_TEXT);

        // Online status
        DrawCircle(cardX + 68, cardY + 52, 5, COLOR_GREEN);
        DrawText("Registered & Online",
                 cardX + 78, cardY + 44, 13, COLOR_GREEN);

        // ── Open Chat button ──
        DrawButton(&ss->chatBtn, COLOR_ACCENT, WHITE);
    }

    // ── Bottom hint ──
    DrawCenteredText("Press Enter or click Search to find a user",
                     SCREEN_H - 30, 13, COLOR_MUTED);
}