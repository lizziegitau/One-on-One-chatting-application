#ifndef CHAT_SCREEN_H
#define CHAT_SCREEN_H

#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"

/** Maximum number of chat messages to load */
#define MAX_CHAT_MESSAGES 200

/* ChatScreen Struct */
typedef struct
{
    char currentUser[MAX_USERNAME];

    /* User Arrays */
    User userList[MAX_USERS];
    int userCount;

    char activePeer[MAX_USERNAME];
    bool hasPeer;

    /* Chat Message Arrays */
    Message chatMessages[MAX_CHAT_MESSAGES];
    int chatMessageCount;

    Message allMessages[MAX_MESSAGES];
    int allMessageCount;

    /* Message Input */
    InputField messageInput;

    /* Sidebar Buttons */
    Button logoutBtn;
    Button deregisterBtn;
    Button searchBtn;

    /* Scroll State */
    float scrollOffset;

    /* Deregister Confirmation Dialog */
    bool showConfirmDialog;
    Button confirmYesBtn;
    Button confirmNoBtn;

} ChatScreen;

/* Function Declarations */
void InitChatScreen(ChatScreen *cs, const char *username);
void LoadChatForPeer(ChatScreen *cs, const char *peer);
AppScreen UpdateChatScreen(ChatScreen *cs, char *loggedInUser);
void DrawChatScreen(ChatScreen *cs);

#endif