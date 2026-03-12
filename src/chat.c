#include "raylib.h"
#include "../include/chat.h"
#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

// ── Layout constants ──
// These numbers define where everything is positioned.
// Changing them moves whole sections of the UI.
#define SIDEBAR_W 220
#define HEADER_H 56
#define INPUT_H 60
#define CHAT_W 860
#define CHAT_SH 580

// ══════════════════════════════════════════
//   InitChatScreen
//   Sets up everything when user logs in.
//   Called once after successful login.
// ══════════════════════════════════════════
void InitChatScreen(ChatScreen *cs, const char *username)
{
    // Store who is logged in
    strncpy(cs->currentUser, username, MAX_USERNAME - 1);

    // Clear active peer — no chat open yet
    memset(cs->activePeer, 0, sizeof(cs->activePeer));
    cs->hasPeer = false;
    cs->chatMessageCount = 0;
    cs->scrollOffset = 0;
    cs->showConfirmDialog = false;

    // ── Load all registered users for sidebar ──
    LoadUsers(cs->userList, &cs->userCount);

    // ── Load all messages from file ──
    LoadMessages(cs->allMessages, &cs->allMessageCount);

    // ── Sidebar buttons ──
    cs->searchBtn = (Button){
        .rect = {10, HEADER_H + 10, SIDEBAR_W - 20, 38},
        .label = "Search User"};

    cs->logoutBtn = (Button){
        .rect = {10, CHAT_SH - 90, (SIDEBAR_W - 25) / 2, 36},
        .label = "Logout"};

    cs->deregisterBtn = (Button){
        .rect = {10 + (SIDEBAR_W - 25) / 2 + 5,
                 CHAT_SH - 90,
                 (SIDEBAR_W - 25) / 2, 36},
        .label = "Delete"};

    // ── Message input field ──
    cs->messageInput = (InputField){
        .rect = {SIDEBAR_W + 12, CHAT_SH - INPUT_H + 8,
                 CHAT_W - SIDEBAR_W - 80, 40},
        .placeholder = "Type a message...",
        .isPassword = false};

    // ── Confirm dialog buttons ──
    cs->confirmYesBtn = (Button){
        .rect = {CHAT_W / 2 - 110, CHAT_SH / 2 + 20, 100, 38},
        .label = "Yes, Delete"};
    cs->confirmNoBtn = (Button){
        .rect = {CHAT_W / 2 + 10, CHAT_SH / 2 + 20, 100, 38},
        .label = "Cancel"};
}

// ══════════════════════════════════════════
//   LoadChatForPeer
//   Filters allMessages to find only the
//   messages between currentUser and peer.
//   Called when a contact is clicked.
// ══════════════════════════════════════════
void LoadChatForPeer(ChatScreen *cs, const char *peer)
{
    strncpy(cs->activePeer, peer, MAX_USERNAME - 1);
    cs->hasPeer = true;
    cs->chatMessageCount = 0;
    cs->scrollOffset = 0;

    // Filter messages for this conversation
    for (int i = 0; i < cs->allMessageCount; i++)
    {
        Message *m = &cs->allMessages[i];

        // Include if: (I sent to peer) OR (peer sent to me)
        bool isMine = strcmp(m->sender, cs->currentUser) == 0 && strcmp(m->recipient, peer) == 0;
        bool isTheirs = strcmp(m->sender, peer) == 0 && strcmp(m->recipient, cs->currentUser) == 0;

        if ((isMine || isTheirs) &&
            cs->chatMessageCount < MAX_CHAT_MESSAGES)
        {
            cs->chatMessages[cs->chatMessageCount++] = *m;
        }
    }
}

// ══════════════════════════════════════════
//   UpdateChatScreen
//   Handles all input and logic.
// ══════════════════════════════════════════
AppScreen UpdateChatScreen(ChatScreen *cs, char *loggedInUser)
{
    // ── Handle confirm dialog first ──
    if (cs->showConfirmDialog)
    {
        if (IsButtonClicked(&cs->confirmYesBtn))
        {
            // Delete account and logout
            DeleteUser(cs->currentUser);
            memset(loggedInUser, 0, MAX_USERNAME);
            return SCREEN_WELCOME;
        }
        if (IsButtonClicked(&cs->confirmNoBtn))
            cs->showConfirmDialog = false;

        return SCREEN_CHAT;
    }

    // ── Sidebar contact clicks ──
    // Each contact is a clickable row in the sidebar
    int contactY = HEADER_H + 64; // start below search button

    for (int i = 0; i < cs->userCount; i++)
    {
        // Skip yourself — you can't chat with yourself
        if (strcmp(cs->userList[i].username, cs->currentUser) == 0)
            continue;

        Rectangle contactRect = {
            0, (float)contactY, SIDEBAR_W, 54};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), contactRect))
        {
            LoadChatForPeer(cs, cs->userList[i].username);
        }

        contactY += 56;
    }

    // ── Search button ──
    if (IsButtonClicked(&cs->searchBtn))
        return SCREEN_SEARCH;

    // ── Logout button ──
    if (IsButtonClicked(&cs->logoutBtn))
    {
        memset(loggedInUser, 0, MAX_USERNAME);
        return SCREEN_WELCOME;
    }

    // ── Deregister button ──
    if (IsButtonClicked(&cs->deregisterBtn))
        cs->showConfirmDialog = true;

    // ── Message input (only if a chat is open) ──
    if (cs->hasPeer)
    {
        // Activate input on click
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            cs->messageInput.active = CheckCollisionPointRec(
                GetMousePosition(), cs->messageInput.rect);
        }

        HandleInputField(&cs->messageInput);

        // Send button area — a simple rectangle to the right
        Rectangle sendRect = {
            CHAT_W - 58.0f, CHAT_SH - INPUT_H + 8.0f, 48, 40};

        bool sendClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), sendRect);
        bool enterPressed = IsKeyPressed(KEY_ENTER) && cs->messageInput.active;

        if ((sendClicked || enterPressed) && cs->messageInput.textLength > 0)
        {
            // ── Build the message ──
            Message msg;
            strncpy(msg.sender, cs->currentUser, MAX_USERNAME - 1);
            strncpy(msg.recipient, cs->activePeer, MAX_USERNAME - 1);
            strncpy(msg.content, cs->messageInput.text, MAX_MESSAGE - 1);

            // Get current time as timestamp
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            snprintf(msg.timestamp, sizeof(msg.timestamp),
                     "%02d:%02d", tm->tm_hour, tm->tm_min);

            // ── Save to file ──
            SaveMessage(&msg);

            // ── Add to in-memory lists ──
            if (cs->allMessageCount < MAX_MESSAGES)
                cs->allMessages[cs->allMessageCount++] = msg;

            if (cs->chatMessageCount < MAX_CHAT_MESSAGES)
                cs->chatMessages[cs->chatMessageCount++] = msg;

            // ── Clear the input field ──
            ClearInputField(&cs->messageInput);
            cs->messageInput.active = true; // keep focus

            // ── Auto scroll to bottom ──
            cs->scrollOffset = 999999;
        }

        // ── Mouse wheel scrolling ──
        float wheel = GetMouseWheelMove();
        cs->scrollOffset -= wheel * 30;
        if (cs->scrollOffset < 0)
            cs->scrollOffset = 0;
    }

    return SCREEN_CHAT;
}

// ══════════════════════════════════════════
//   DrawChatScreen
//   Draws the entire chat UI every frame.
// ══════════════════════════════════════════
void DrawChatScreen(ChatScreen *cs)
{
    ClearBackground(COLOR_BG);

    // ════════════════════════════════
    //   SIDEBAR
    // ════════════════════════════════
    DrawPanel(0, 0, SIDEBAR_W, CHAT_SH, COLOR_SIDEBAR, COLOR_BORDER);

    // ── Sidebar header — show logged in user ──
    DrawPanel(0, 0, SIDEBAR_W, HEADER_H, COLOR_CARD, COLOR_BORDER);

    // Avatar circle with first letter
    DrawAvatar(28, HEADER_H / 2, 18,
               COLOR_ACCENT, cs->currentUser[0]);

    // Username next to avatar
    DrawText(cs->currentUser,
             52, HEADER_H / 2 - 8, 15, COLOR_TEXT);

    // "Online" status dot + text
    DrawCircle(52, HEADER_H / 2 + 12, 4, COLOR_GREEN);
    DrawText("Online", 60, HEADER_H / 2 + 5, 12, COLOR_GREEN);

    // ── Search button ──
    DrawButton(&cs->searchBtn, COLOR_CARD, COLOR_TEAL);

    // ── Contacts list ──
    DrawText("CONTACTS", 12, HEADER_H + 58, 11, COLOR_MUTED);

    int contactY = HEADER_H + 74;

    for (int i = 0; i < cs->userCount; i++)
    {
        // Skip self
        if (strcmp(cs->userList[i].username, cs->currentUser) == 0)
            continue;

        const char *uname = cs->userList[i].username;
        bool isActive = cs->hasPeer &&
                        strcmp(cs->activePeer, uname) == 0;

        // Highlight active contact
        if (isActive)
        {
            DrawRectangle(0, contactY, SIDEBAR_W, 54,
                          (Color){233, 69, 96, 40});
            // Active indicator bar on left edge
            DrawRectangle(0, contactY + 8, 3, 38, COLOR_ACCENT);
        }

        // Avatar
        Color avatarColor = (Color){
            (unsigned char)(60 + (uname[0] * 37) % 120),
            (unsigned char)(80 + (uname[0] * 71) % 100),
            (unsigned char)(120 + (uname[0] * 53) % 100),
            255};
        DrawAvatar(24, contactY + 27, 16, avatarColor, uname[0]);

        // Username
        DrawText(uname, 48, contactY + 14, 14,
                 isActive ? COLOR_ACCENT : COLOR_TEXT);

        // Last message preview
        // Find the last message between these two users
        const char *preview = "No messages yet";
        for (int j = cs->allMessageCount - 1; j >= 0; j--)
        {
            Message *m = &cs->allMessages[j];
            bool relevant =
                (strcmp(m->sender, cs->currentUser) == 0 &&
                 strcmp(m->recipient, uname) == 0) ||
                (strcmp(m->sender, uname) == 0 &&
                 strcmp(m->recipient, cs->currentUser) == 0);
            if (relevant)
            {
                preview = m->content;
                break;
            }
        }

        // Truncate preview to 22 chars
        char previewShort[26] = {0};
        strncpy(previewShort, preview, 22);
        if (strlen(preview) > 22)
            strcat(previewShort, "...");

        DrawText(previewShort, 48, contactY + 32, 12, COLOR_MUTED);

        contactY += 56;
    }

    // ── Logout & Delete buttons at bottom ──
    DrawButton(&cs->logoutBtn, COLOR_CARD, COLOR_TEXT);
    DrawButton(&cs->deregisterBtn, COLOR_CARD, COLOR_ACCENT);

    // ════════════════════════════════
    //   MAIN CHAT AREA
    // ════════════════════════════════
    int chatX = SIDEBAR_W;
    int chatW = CHAT_W - SIDEBAR_W;

    // ── Chat header ──
    DrawPanel(chatX, 0, chatW, HEADER_H, COLOR_SURFACE, COLOR_BORDER);

    if (cs->hasPeer)
    {
        // Show peer name and status
        DrawAvatar(chatX + 28, HEADER_H / 2, 18,
                   COLOR_TEAL, cs->activePeer[0]);
        DrawText(cs->activePeer,
                 chatX + 52, HEADER_H / 2 - 8, 16, COLOR_TEXT);
        DrawCircle(chatX + 52, HEADER_H / 2 + 12, 4, COLOR_GREEN);
        DrawText("Online", chatX + 60, HEADER_H / 2 + 5, 12, COLOR_GREEN);
    }
    else
    {
        // No chat open — show prompt
        DrawText("Select a contact to start chatting",
                 chatX + chatW / 2 - MeasureText("Select a contact to start chatting", 16) / 2,
                 HEADER_H / 2 - 8, 16, COLOR_MUTED);
    }

    // ── Messages area ──
    int msgAreaY = HEADER_H;
    int msgAreaH = CHAT_SH - HEADER_H - INPUT_H;

    // Clip drawing to the message area
    BeginScissorMode(chatX, msgAreaY, chatW, msgAreaH);

    if (!cs->hasPeer)
    {
        // Empty state
        DrawText("No chat open",
                 chatX + chatW / 2 - 60,
                 CHAT_SH / 2 - 20, 20, COLOR_MUTED);
        DrawText("Click a contact on the left to begin",
                 chatX + chatW / 2 -
                     MeasureText("Click a contact on the left to begin",
                                 14) /
                         2,
                 CHAT_SH / 2 + 10, 14, COLOR_MUTED);
    }
    else
    {
        // Calculate total height of all messages
        // so we can scroll correctly
        int totalMsgHeight = cs->chatMessageCount * 54;
        int visibleHeight = msgAreaH - 20;

        // Auto-scroll to bottom
        float maxScroll = (float)(totalMsgHeight - visibleHeight);
        if (maxScroll < 0)
            maxScroll = 0;
        if (cs->scrollOffset > maxScroll)
            cs->scrollOffset = maxScroll;

        int startY = msgAreaY + 10 - (int)cs->scrollOffset;

        // Draw each message bubble
        for (int i = 0; i < cs->chatMessageCount; i++)
        {
            Message *m = &cs->chatMessages[i];
            bool sent = strcmp(m->sender, cs->currentUser) == 0;

            int bubbleY = startY + i * 54;
            int maxBubW = chatW - 40;

            // Only draw if visible on screen
            if (bubbleY + 44 < msgAreaY)
            {
                continue;
            }
            if (bubbleY > msgAreaY + msgAreaH)
            {
                break;
            }

            if (sent)
            {
                // Sent: right side, red bubble
                DrawChatBubble(m->content,
                               chatX + chatW - 14,
                               bubbleY, maxBubW, true);
                // Timestamp
                DrawText(m->timestamp,
                         chatX + chatW - 14 -
                             MeasureText(m->timestamp, 11) - 4,
                         bubbleY + 44, 11, COLOR_MUTED);
            }
            else
            {
                // Received: left side, dark blue bubble
                DrawChatBubble(m->content,
                               chatX + 14,
                               bubbleY, maxBubW, false);
                // Timestamp
                DrawText(m->timestamp,
                         chatX + 18, bubbleY + 44, 11, COLOR_MUTED);
            }
        }
    }

    EndScissorMode();

    // ── Input area ──
    DrawPanel(chatX, CHAT_SH - INPUT_H,
              chatW, INPUT_H, COLOR_SURFACE, COLOR_BORDER);

    if (cs->hasPeer)
    {
        DrawInputField(&cs->messageInput);

        // Send button
        Rectangle sendRect = {
            CHAT_W - 58.0f, CHAT_SH - INPUT_H + 8.0f, 48, 40};
        bool sendHovered = CheckCollisionPointRec(
            GetMousePosition(), sendRect);

        DrawRectangleRounded(sendRect, 0.3f, 6,
                             sendHovered ? COLOR_ACCENT : (Color){233, 69, 96, 180});
        DrawText(">>",
                 CHAT_W - 48,
                 CHAT_SH - INPUT_H + 20, 16, WHITE);
    }
    else
    {
        // Greyed out input when no chat is open
        DrawText("Select a contact to send a message",
                 chatX + chatW / 2 -
                     MeasureText("Select a contact to send a message",
                                 14) /
                         2,
                 CHAT_SH - INPUT_H + 22, 14, COLOR_MUTED);
    }

    // ════════════════════════════════
    //   CONFIRM DEREGISTER DIALOG
    // ════════════════════════════════
    if (cs->showConfirmDialog)
    {
        // Dark overlay
        DrawRectangle(0, 0, CHAT_W, CHAT_SH,
                      (Color){0, 0, 0, 160});

        // Dialog box
        int dw = 340, dh = 160;
        int dx = CHAT_W / 2 - dw / 2;
        int dy = CHAT_SH / 2 - dh / 2;

        DrawPanel(dx, dy, dw, dh, COLOR_CARD, COLOR_ACCENT);

        DrawCenteredText("Delete Account?",
                         dy + 20, 20, COLOR_TEXT);
        DrawCenteredText("This cannot be undone.",
                         dy + 50, 15, COLOR_MUTED);

        DrawButton(&cs->confirmYesBtn, COLOR_ACCENT, WHITE);
        DrawButton(&cs->confirmNoBtn, COLOR_SURFACE, COLOR_TEXT);
    }
}