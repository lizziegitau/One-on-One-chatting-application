#ifndef FILEIO_H
#define FILEIO_H

#include <stdbool.h>

/* Maximum Limits */
#define MAX_USERS 100
#define MAX_MESSAGES 1000
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_MESSAGE 256

/* User Struct */
typedef struct
{
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} User;

/* Message Struct */
typedef struct
{
    char sender[MAX_USERNAME];
    char recipient[MAX_USERNAME];
    char content[MAX_MESSAGE];
    char timestamp[20];
} Message;

/* Function Declarations */
/* User Operations */
bool LoadUsers(User *users, int *count);
bool SaveUser(User *user);
bool UserExists(const char *username);
bool ValidateLogin(const char *username, const char *password);
bool DeleteUser(const char *username);

/* Message Operations */
bool LoadMessages(Message *messages, int *count);
bool SaveMessage(Message *message);

#endif