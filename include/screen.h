#ifndef SCREEN_H
#define SCREEN_H

typedef enum
{
    SCREEN_WELCOME, // The welcome screen when app opens
    SCREEN_AUTH,    // Login / Register screen
    SCREEN_CHAT,    // Main chat screen with sidebar
    SCREEN_SEARCH   // Search for a user screen
} AppScreen;

#endif