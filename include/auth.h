#ifndef AUTH_H
#define AUTH_H

#include "../include/ui.h"
#include "../include/screen.h"

/* AuthMode Enum that tracks which form (Login or Register) is being displayed */
typedef enum
{
    AUTH_LOGIN,   /* Show Login form */
    AUTH_REGISTER /* Show Register form */
} AuthMode;

/* AuthScreen Struct */
typedef struct
{
    AuthMode mode;

    InputField usernameField; /* Username input field */
    InputField passwordField; /* Password input field */

    Button submitBtn; /* Button to submit Login or Register form*/
    Button switchBtn; /* Button to switch between Login and Register forms */
    Button backBtn;   /* Button to go back to the previous screen */

    char message[128];  /* Message system to show feedback */
    bool messageIsOk;   /* Flag to indicate if the message is an error or success */
    float messageTimer; /* Timer for message display */

    bool startAsRegister;
} AuthScreen;

/* Function Declarations */
void InitAuthScreen(AuthScreen *as, bool startAsRegister);
AppScreen UpdateAuthScreen(AuthScreen *as, char *loggedInUser);
void DrawAuthScreen(AuthScreen *as);

#endif