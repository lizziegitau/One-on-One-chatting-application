#include "raylib.h"
#include "../include/chat.h"
#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Layout constants — changing these repositions whole sections of the UI */
#define SIDEBAR_W 220 /* Width of the left sidebar */
#define HEADER_H 56   /* Height of the top header bar */
#define INPUT_H 60    /* Height of the message input area */
#define CHAT_W 860    /* Total screen width */
#define CHAT_SH 580   /* Total screen height */

/* Initialises the Chat Screen after a successful login.
 * Loads all users into the sidebar and all messages into memory. */
void InitChatScreen(ChatScreen *cs, const char *username)
{
    strncpy(cs->currentUser, username, MAX_USERNAME - 1);

    /* No conversation open yet */
    memset(cs->activePeer, 0, sizeof(cs->activePeer));
    cs->hasPeer = false;
    cs->chatMessageCount = 0;
    cs->scrollOffset = 0;
    cs->showConfirmDialog = false;

    /* Load all registered users for the sidebar contact list */
    LoadUsers(cs->userList, &cs->userCount);

    /* Load full message history into memory — filtered per conversation on contact click */
    LoadMessages(cs->allMessages, &cs->allMessageCount);

    /* Sidebar buttons */
    cs->searchBtn = (Button){
        .rect = {10, HEADER_H + 10, SIDEBAR_W - 20, 38},
        .label = "Search User"};

    cs->logoutBtn = (Button){
        .rect = {10, CHAT_SH - 90, (SIDEBAR_W - 25) / 2, 36},
        .label = "Logout"};

    cs->deregisterBtn = (Button){
        .rect = {10 + (SIDEBAR_W - 25) / 2 + 5, CHAT_SH - 90, (SIDEBAR_W - 25) / 2, 36},
        .label = "Delete"};

    /* Message input field at the bottom of the chat area */
    cs->messageInput = (InputField){
        .rect = {SIDEBAR_W + 12, CHAT_SH - INPUT_H + 8, CHAT_W - SIDEBAR_W - 80, 40},
        .placeholder = "Type a message...",
        .isPassword = false};

    /* Confirmation dialog buttons for account deletion */
    cs->confirmYesBtn = (Button){
        .rect = {CHAT_W / 2 - 110, CHAT_SH / 2 + 20, 100, 38},
        .label = "Yes, Delete"};
    cs->confirmNoBtn = (Button){
        .rect = {CHAT_W / 2 + 10, CHAT_SH / 2 + 20, 100, 38},
        .label = "Cancel"};
}

/* Filters allMessages[] to only the conversation between currentUser and peer.
 * Called each time a contact is clicked in the sidebar. */
void LoadChatForPeer(ChatScreen *cs, const char *peer)
{
    strncpy(cs->activePeer, peer, MAX_USERNAME - 1);
    cs->hasPeer = true;
    cs->chatMessageCount = 0;
    cs->scrollOffset = 0;

    for (int i = 0; i < cs->allMessageCount; i++)
    {
        Message *m = &cs->allMessages[i];

        /* Include messages sent by me to peer, or sent by peer to me */
        bool isMine = strcmp(m->sender, cs->currentUser) == 0 && strcmp(m->recipient, peer) == 0;
        bool isTheirs = strcmp(m->sender, peer) == 0 && strcmp(m->recipient, cs->currentUser) == 0;

        if ((isMine || isTheirs) && cs->chatMessageCount < MAX_CHAT_MESSAGES)
            cs->chatMessages[cs->chatMessageCount++] = *m;
    }
}

/* Handles all Chat Screen logic — contact clicks, sending messages, scroll,
 * sidebar buttons, and the deregister confirmation dialog. */
AppScreen UpdateChatScreen(ChatScreen *cs, char *loggedInUser)
{
    /* Handle confirmation dialog before anything else — blocks all other input */
    if (cs->showConfirmDialog)
    {
        if (IsButtonClicked(&cs->confirmYesBtn))
        {
            /* Confirmed — delete account and all messages, then logout */
            DeleteUser(cs->currentUser);
            memset(loggedInUser, 0, MAX_USERNAME);
            return SCREEN_WELCOME;
        }
        if (IsButtonClicked(&cs->confirmNoBtn))
            cs->showConfirmDialog = false;

        return SCREEN_CHAT;
    }

    /* Sidebar contact rows — each is a clickable rectangle */
    int contactY = HEADER_H + 64;
    for (int i = 0; i < cs->userCount; i++)
    {
        if (strcmp(cs->userList[i].username, cs->currentUser) == 0)
            continue; /* Skip self — cannot chat with yourself */

        Rectangle contactRect = {0, (float)contactY, SIDEBAR_W, 54};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), contactRect))
            LoadChatForPeer(cs, cs->userList[i].username);

        contactY += 56;
    }

    /* Sidebar action buttons */
    if (IsButtonClicked(&cs->searchBtn))
        return SCREEN_SEARCH;
    if (IsButtonClicked(&cs->logoutBtn))
    {
        memset(loggedInUser, 0, MAX_USERNAME);
        return SCREEN_WELCOME;
    }
    if (IsButtonClicked(&cs->deregisterBtn))
        cs->showConfirmDialog = true;

    /* Message input and sending — only active when a conversation is open */
    if (cs->hasPeer)
    {
        /* Activate the input field on click */
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            cs->messageInput.active = CheckCollisionPointRec(
                GetMousePosition(), cs->messageInput.rect);

        HandleInputField(&cs->messageInput);

        Rectangle sendRect = {CHAT_W - 58.0f, CHAT_SH - INPUT_H + 8.0f, 48, 40};
        bool sendClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                           CheckCollisionPointRec(GetMousePosition(), sendRect);
        bool enterPressed = IsKeyPressed(KEY_ENTER) && cs->messageInput.active;

        if ((sendClicked || enterPressed) && cs->messageInput.textLength > 0)
        {
            /* Build the message struct */
            Message msg;
            strncpy(msg.sender, cs->currentUser, MAX_USERNAME - 1);
            strncpy(msg.recipient, cs->activePeer, MAX_USERNAME - 1);
            strncpy(msg.content, cs->messageInput.text, MAX_MESSAGE - 1);

            /* Stamp with current local time */
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            snprintf(msg.timestamp, sizeof(msg.timestamp), "%02d:%02d", tm->tm_hour, tm->tm_min);

            /* Save to file then add to both in-memory arrays for instant display */
            SaveMessage(&msg);

            if (cs->allMessageCount < MAX_MESSAGES)
                cs->allMessages[cs->allMessageCount++] = msg;
            if (cs->chatMessageCount < MAX_CHAT_MESSAGES)
                cs->chatMessages[cs->chatMessageCount++] = msg;

            ClearInputField(&cs->messageInput);
            cs->messageInput.active = true; /* Keep focus on the input field */
            cs->scrollOffset = 999999;      /* Auto-scroll to bottom */
        }

        /* Mouse wheel scrolling — clamp so it never goes above the top */
        float wheel = GetMouseWheelMove();
        cs->scrollOffset -= wheel * 30;
        if (cs->scrollOffset < 0)
            cs->scrollOffset = 0;
    }

    return SCREEN_CHAT;
}

/* Draws the full Chat Screen every frame — sidebar, chat area, input and dialog */
void DrawChatScreen(ChatScreen *cs)
{
    ClearBackground(COLOR_BG);

    /* ── SIDEBAR ───────────────────────────────────────────────── */
    DrawPanel(0, 0, SIDEBAR_W, CHAT_SH, COLOR_SIDEBAR, COLOR_BORDER);

    /* Sidebar header — logged-in user avatar, name and online status */
    DrawPanel(0, 0, SIDEBAR_W, HEADER_H, COLOR_CARD, COLOR_BORDER);
    DrawAvatar(28, HEADER_H / 2, 18, COLOR_ACCENT, cs->currentUser[0]);
    DrawText(cs->currentUser, 52, HEADER_H / 2 - 8, 15, COLOR_TEXT);
    DrawCircle(52, HEADER_H / 2 + 12, 4, COLOR_GREEN);
    DrawText("Online", 60, HEADER_H / 2 + 5, 12, COLOR_GREEN);

    DrawButton(&cs->searchBtn, COLOR_CARD, COLOR_TEAL);
    DrawText("CONTACTS", 12, HEADER_H + 58, 11, COLOR_MUTED);

    /* Contact list — one row per registered user (excluding self) */
    int contactY = HEADER_H + 74;
    for (int i = 0; i < cs->userCount; i++)
    {
        if (strcmp(cs->userList[i].username, cs->currentUser) == 0)
            continue;

        const char *uname = cs->userList[i].username;
        bool isActive = cs->hasPeer && strcmp(cs->activePeer, uname) == 0;

        /* Highlight the active contact with a tinted background and accent bar */
        if (isActive)
        {
            DrawRectangle(0, contactY, SIDEBAR_W, 54, (Color){233, 69, 96, 40});
            DrawRectangle(0, contactY + 8, 3, 38, COLOR_ACCENT);
        }

        /* Avatar — color derived from first letter of username for variety */
        Color avatarColor = (Color){
            (unsigned char)(60 + (uname[0] * 37) % 120),
            (unsigned char)(80 + (uname[0] * 71) % 100),
            (unsigned char)(120 + (uname[0] * 53) % 100),
            255};
        DrawAvatar(24, contactY + 27, 16, avatarColor, uname[0]);

        DrawText(uname, 48, contactY + 14, 14, isActive ? COLOR_ACCENT : COLOR_TEXT);

        /* Last message preview — scan allMessages backwards for the most recent */
        const char *preview = "No messages yet";
        for (int j = cs->allMessageCount - 1; j >= 0; j--)
        {
            Message *m = &cs->allMessages[j];
            bool relevant =
                (strcmp(m->sender, cs->currentUser) == 0 && strcmp(m->recipient, uname) == 0) ||
                (strcmp(m->sender, uname) == 0 && strcmp(m->recipient, cs->currentUser) == 0);
            if (relevant)
            {
                preview = m->content;
                break;
            }
        }

        /* Truncate preview to 22 chars to fit the sidebar width */
        char previewShort[26] = {0};
        strncpy(previewShort, preview, 22);
        if (strlen(preview) > 22)
            strcat(previewShort, "...");
        DrawText(previewShort, 48, contactY + 32, 12, COLOR_MUTED);

        contactY += 56;
    }

    /* Logout and Delete buttons at the bottom of the sidebar */
    DrawButton(&cs->logoutBtn, COLOR_CARD, COLOR_TEXT);
    DrawButton(&cs->deregisterBtn, COLOR_CARD, COLOR_ACCENT);

    /* ── MAIN CHAT AREA ────────────────────────────────────────── */
    int chatX = SIDEBAR_W;
    int chatW = CHAT_W - SIDEBAR_W;

    /* Chat header — shows active peer name and status, or a prompt if no chat is open */
    DrawPanel(chatX, 0, chatW, HEADER_H, COLOR_SURFACE, COLOR_BORDER);
    if (cs->hasPeer)
    {
        DrawAvatar(chatX + 28, HEADER_H / 2, 18, COLOR_TEAL, cs->activePeer[0]);
        DrawText(cs->activePeer, chatX + 52, HEADER_H / 2 - 8, 16, COLOR_TEXT);
        DrawCircle(chatX + 52, HEADER_H / 2 + 12, 4, COLOR_GREEN);
        DrawText("Online", chatX + 60, HEADER_H / 2 + 5, 12, COLOR_GREEN);
    }
    else
    {
        DrawText("Select a contact to start chatting",
                 chatX + chatW / 2 - MeasureText("Select a contact to start chatting", 16) / 2,
                 HEADER_H / 2 - 8, 16, COLOR_MUTED);
    }

    /* Message bubble area — clipped so bubbles don't overflow outside the chat region */
    int msgAreaY = HEADER_H;
    int msgAreaH = CHAT_SH - HEADER_H - INPUT_H;
    BeginScissorMode(chatX, msgAreaY, chatW, msgAreaH);

    if (!cs->hasPeer)
    {
        /* Empty state prompt when no conversation is selected */
        DrawText("No chat open",
                 chatX + chatW / 2 - 60, CHAT_SH / 2 - 20, 20, COLOR_MUTED);
        DrawText("Click a contact on the left to begin",
                 chatX + chatW / 2 - MeasureText("Click a contact on the left to begin", 14) / 2,
                 CHAT_SH / 2 + 10, 14, COLOR_MUTED);
    }
    else
    {
        /* Calculate scroll bounds and clamp scrollOffset */
        int totalMsgHeight = cs->chatMessageCount * 54;
        int visibleHeight = msgAreaH - 20;
        float maxScroll = (float)(totalMsgHeight - visibleHeight);
        if (maxScroll < 0)
            maxScroll = 0;
        if (cs->scrollOffset > maxScroll)
            cs->scrollOffset = maxScroll;

        int startY = msgAreaY + 10 - (int)cs->scrollOffset;

        /* Draw each message bubble — skip if above visible area, stop if below */
        for (int i = 0; i < cs->chatMessageCount; i++)
        {
            Message *m = &cs->chatMessages[i];
            bool sent = strcmp(m->sender, cs->currentUser) == 0;
            int bubbleY = startY + i * 54;
            int maxBubW = chatW - 40;

            if (bubbleY + 44 < msgAreaY)
                continue; /* Above viewport — skip */
            if (bubbleY > msgAreaY + msgAreaH)
                break; /* Below viewport — stop */

            if (sent)
            {
                /* Sent — right-aligned red bubble with timestamp below */
                DrawChatBubble(m->content, chatX + chatW - 14, bubbleY, maxBubW, true);
                DrawText(m->timestamp,
                         chatX + chatW - 14 - MeasureText(m->timestamp, 11) - 4,
                         bubbleY + 44, 11, COLOR_MUTED);
            }
            else
            {
                /* Received — left-aligned blue bubble with timestamp below */
                DrawChatBubble(m->content, chatX + 14, bubbleY, maxBubW, false);
                DrawText(m->timestamp, chatX + 18, bubbleY + 44, 11, COLOR_MUTED);
            }
        }
    }

    EndScissorMode();

    /* Input area panel at the bottom of the chat area */
    DrawPanel(chatX, CHAT_SH - INPUT_H, chatW, INPUT_H, COLOR_SURFACE, COLOR_BORDER);
    if (cs->hasPeer)
    {
        DrawInputField(&cs->messageInput);

        /* Send button — brightens on hover */
        Rectangle sendRect = {CHAT_W - 58.0f, CHAT_SH - INPUT_H + 8.0f, 48, 40};
        bool sendHovered = CheckCollisionPointRec(GetMousePosition(), sendRect);
        DrawRectangleRounded(sendRect, 0.3f, 6,
                             sendHovered ? COLOR_ACCENT : (Color){233, 69, 96, 180});
        DrawText(">>", CHAT_W - 48, CHAT_SH - INPUT_H + 20, 16, WHITE);
    }
    else
    {
        /* Greyed-out prompt when no chat is open */
        DrawText("Select a contact to send a message",
                 chatX + chatW / 2 - MeasureText("Select a contact to send a message", 14) / 2,
                 CHAT_SH - INPUT_H + 22, 14, COLOR_MUTED);
    }

    /* ── DEREGISTER CONFIRMATION DIALOG ───────────────────────── */
    if (cs->showConfirmDialog)
    {
        /* Semi-transparent dark overlay over the entire screen */
        DrawRectangle(0, 0, CHAT_W, CHAT_SH, (Color){0, 0, 0, 160});

        /* Centered dialog box */
        int dw = 340, dh = 160;
        int dx = CHAT_W / 2 - dw / 2;
        int dy = CHAT_SH / 2 - dh / 2;
        DrawPanel(dx, dy, dw, dh, COLOR_CARD, COLOR_ACCENT);
        DrawCenteredText("Delete Account?", dy + 20, 20, COLOR_TEXT);
        DrawCenteredText("This cannot be undone.", dy + 50, 15, COLOR_MUTED);
        DrawButton(&cs->confirmYesBtn, COLOR_ACCENT, WHITE);
        DrawButton(&cs->confirmNoBtn, COLOR_SURFACE, COLOR_TEXT);
    }
}