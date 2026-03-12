#ifndef CHAT_SCREEN_H
#define CHAT_SCREEN_H

#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"

#define MAX_CHAT_MESSAGES 200

typedef struct
{
    char currentUser[MAX_USERNAME];

    User userList[MAX_USERS];
    int userCount;

    char activePeer[MAX_USERNAME];
    bool hasPeer;

    Message chatMessages[MAX_CHAT_MESSAGES];
    int chatMessageCount;

    Message allMessages[MAX_MESSAGES];
    int allMessageCount;

    InputField messageInput;

    Button logoutBtn;
    Button deregisterBtn;
    Button searchBtn;

    float scrollOffset;

    bool showConfirmDialog;
    Button confirmYesBtn;
    Button confirmNoBtn;

} ChatScreen;

void InitChatScreen(ChatScreen *cs, const char *username);
void LoadChatForPeer(ChatScreen *cs, const char *peer);
AppScreen UpdateChatScreen(ChatScreen *cs, char *loggedInUser);
void DrawChatScreen(ChatScreen *cs);

#endif