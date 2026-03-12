#ifndef SEARCH_H
#define SEARCH_H

#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"

// ── Result states ──
typedef enum
{
    SEARCH_IDLE,     // user hasn't searched yet
    SEARCH_FOUND,    // user was found
    SEARCH_NOT_FOUND // user was not found
} SearchResult;

// ── Holds everything the Search screen needs ──
typedef struct
{
    InputField searchField; // the search text box
    Button searchBtn;       // Search button
    Button backBtn;         // Back to chat
    Button chatBtn;         // Open Chat with result

    SearchResult result; // current result state
    User foundUser;      // the user that was found

    char currentUser[MAX_USERNAME];  // who is logged in
    char selectedPeer[MAX_USERNAME]; // who to open chat with
    bool openChat;                   // signal to open a chat
} SearchScreen;

// ── Function declarations ──
void InitSearchScreen(SearchScreen *ss, const char *currentUser);
AppScreen UpdateSearchScreen(SearchScreen *ss, char *peerToOpen);
void DrawSearchScreen(SearchScreen *ss);

#endif