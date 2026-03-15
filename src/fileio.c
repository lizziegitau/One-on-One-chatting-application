#include "../include/fileio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* File paths for the two data files */
#define USERS_FILE "data/users.txt"
#define MESSAGES_FILE "data/messages.txt"

/* Reads all users from users.txt into the users[] array.
 * Returns false if the file does not exist yet (first run). */
bool LoadUsers(User *users, int *count)
{
    *count = 0;

    FILE *f = fopen(USERS_FILE, "r");
    if (f == NULL)
        return false;

    char line[128];
    while (fgets(line, sizeof(line), f) && *count < MAX_USERS)
    {
        line[strcspn(line, "\n")] = '\0'; /* Strip trailing newline */

        /* Split line at the comma into username and password */
        char *comma = strchr(line, ',');
        if (comma == NULL)
            continue; /* Skip malformed lines */

        *comma = '\0';
        strncpy(users[*count].username, line, MAX_USERNAME - 1);
        strncpy(users[*count].password, comma + 1, MAX_PASSWORD - 1);
        (*count)++;
    }

    fclose(f);
    return true;
}

/* Appends a single new user record to users.txt.
 * Uses "a" (append) mode so existing records are not overwritten. */
bool SaveUser(User *user)
{
    FILE *f = fopen(USERS_FILE, "a");
    if (f == NULL)
        return false;

    fprintf(f, "%s,%s\n", user->username, user->password);
    fclose(f);
    return true;
}

/* Returns true if the given username already exists in users.txt */
bool UserExists(const char *username)
{
    User users[MAX_USERS];
    int count = 0;
    LoadUsers(users, &count);

    for (int i = 0; i < count; i++)
        if (strcmp(users[i].username, username) == 0)
            return true;

    return false;
}

/* Returns true if the username and password match a record in users.txt */
bool ValidateLogin(const char *username, const char *password)
{
    User users[MAX_USERS];
    int count = 0;
    LoadUsers(users, &count);

    for (int i = 0; i < count; i++)
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0)
            return true;

    return false;
}

/* Removes a user from users.txt and deletes all their messages from messages.txt.
 * Both files are rewritten from scratch excluding the deleted user's records. */
bool DeleteUser(const char *username)
{
    /* Step 1 — Rewrite users.txt excluding the deleted user */
    User users[MAX_USERS];
    int count = 0;
    LoadUsers(users, &count);

    FILE *f = fopen(USERS_FILE, "w");
    if (f == NULL)
        return false;

    for (int i = 0; i < count; i++)
        if (strcmp(users[i].username, username) != 0)
            fprintf(f, "%s,%s\n", users[i].username, users[i].password);

    fclose(f);

    /* Step 2 — Rewrite messages.txt excluding any message sent or received by this user */
    Message messages[MAX_MESSAGES];
    int msgCount = 0;
    LoadMessages(messages, &msgCount);

    FILE *mf = fopen(MESSAGES_FILE, "w");
    if (mf == NULL)
        return false;

    for (int i = 0; i < msgCount; i++)
    {
        bool userIsSender = strcmp(messages[i].sender, username) == 0;
        bool userIsRecipient = strcmp(messages[i].recipient, username) == 0;

        /* Only write messages that do not involve the deleted user */
        if (!userIsSender && !userIsRecipient)
            fprintf(mf, "%s,%s,%s,%s\n",
                    messages[i].sender,
                    messages[i].recipient,
                    messages[i].content,
                    messages[i].timestamp);
    }
    fclose(mf);

    return true;
}

/* Reads all messages from messages.txt into the messages[] array.
 * Each line is parsed into 4 comma-separated fields: sender,recipient,content,timestamp. */
bool LoadMessages(Message *messages, int *count)
{
    *count = 0;

    FILE *f = fopen(MESSAGES_FILE, "r");
    if (f == NULL)
        return false;

    char line[512];
    while (fgets(line, sizeof(line), f) && *count < MAX_MESSAGES)
    {
        line[strcspn(line, "\n")] = '\0'; /* Strip trailing newline */

        /* Parse the four comma-separated fields using strtok */
        char *p1 = strtok(line, ","); /* sender    */
        char *p2 = strtok(NULL, ","); /* recipient */
        char *p3 = strtok(NULL, ","); /* content   */
        char *p4 = strtok(NULL, ","); /* timestamp */

        if (!p1 || !p2 || !p3 || !p4)
            continue; /* Skip malformed lines */

        strncpy(messages[*count].sender, p1, MAX_USERNAME - 1);
        strncpy(messages[*count].recipient, p2, MAX_USERNAME - 1);
        strncpy(messages[*count].content, p3, MAX_MESSAGE - 1);
        strncpy(messages[*count].timestamp, p4, 19);
        (*count)++;
    }

    fclose(f);
    return true;
}

/* Appends a single new message to messages.txt.
 * Uses "a" (append) mode so existing messages are not overwritten. */
bool SaveMessage(Message *message)
{
    FILE *f = fopen(MESSAGES_FILE, "a");
    if (f == NULL)
        return false;

    fprintf(f, "%s,%s,%s,%s\n",
            message->sender,
            message->recipient,
            message->content,
            message->timestamp);

    fclose(f);
    return true;
}