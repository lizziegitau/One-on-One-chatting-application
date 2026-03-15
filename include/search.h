#ifndef SEARCH_H
#define SEARCH_H

#include "../include/ui.h"
#include "../include/screen.h"
#include "../include/fileio.h"

/* Search Result Enum */
typedef enum
{
    SEARCH_IDLE,     /* No search performed */
    SEARCH_FOUND,    /* User found */
    SEARCH_NOT_FOUND /* User not found */
} SearchResult;

/* Search Screen Struct */
typedef struct
{
    InputField searchField; /* Search input field */
    Button searchBtn;       /* Search button */
    Button backBtn;         /* Back button */
    Button chatBtn;         /* Open Chat button */

    /* Search Results */
    SearchResult result;
    User foundUser;

    /* User Information */
    char currentUser[MAX_USERNAME];
    char selectedPeer[MAX_USERNAME];
    bool openChat;
} SearchScreen;

/* Function Declarations */
void InitSearchScreen(SearchScreen *ss, const char *currentUser);
AppScreen UpdateSearchScreen(SearchScreen *ss, char *peerToOpen);
void DrawSearchScreen(SearchScreen *ss);

#endif