#ifndef APP_SCREEN_H
#define APP_SCREEN_H

typedef enum
{
    SCREEN_WELCOME, /* Opening screen with Login and Register buttons */
    SCREEN_AUTH,    /* Auth screen with Login and Register forms */
    SCREEN_CHAT,    /* Main chat screen */
    SCREEN_SEARCH   /* Search screen where you find user by their username */
} AppScreen;

#endif