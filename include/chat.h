#ifndef CHAT_H
#define CHAT_H

#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"

// ── How many messages we show in memory at once ──
#define MAX_CHAT_MESSAGES 200

// ── Holds the full state of the chat screen ──
typedef struct
{

    // ── Logged in user ──
    char currentUser[MAX_USERNAME];

    // ── All registered users (for sidebar contacts) ──
    User userList[MAX_USERS];
    int userCount;

    // ── The person currently being chatted with ──
    char activePeer[MAX_USERNAME];
    bool hasPeer; // is a chat open?

    // ── Messages for the active chat ──
    Message chatMessages[MAX_CHAT_MESSAGES];
    int chatMessageCount;

    // ── All messages loaded from file ──
    Message allMessages[MAX_MESSAGES];
    int allMessageCount;

    // ── Message input field ──
    InputField messageInput;

    // ── Sidebar buttons ──
    Button logoutBtn;
    Button deregisterBtn;
    Button searchBtn;

    // ── Scroll offset for chat messages ──
    float scrollOffset;

    // ── Confirmation dialog for deregister ──
    bool showConfirmDialog;
    Button confirmYesBtn;
    Button confirmNoBtn;

} ChatScreen;

// ── Function declarations ──
void InitChatScreen(ChatScreen *cs, const char *username);
void LoadChatForPeer(ChatScreen *cs, const char *peer);
AppScreen UpdateChatScreen(ChatScreen *cs, char *loggedInUser);
void DrawChatScreen(ChatScreen *cs);

#endif