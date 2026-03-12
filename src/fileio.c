#include "../include/fileio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ── File paths ──
// All data files live in the data/ folder
#define USERS_FILE "data/users.txt"
#define MESSAGES_FILE "data/messages.txt"

// ══════════════════════════════════════════
//   LoadUsers
//   Reads all users from users.txt into
//   an array of User structs.
//   Returns true if successful.
// ══════════════════════════════════════════
bool LoadUsers(User *users, int *count)
{
    *count = 0;

    FILE *f = fopen(USERS_FILE, "r");
    if (f == NULL)
        return false; // file doesn't exist yet

    // Read one line at a time
    // Each line format: username,password
    char line[128];
    while (fgets(line, sizeof(line), f) && *count < MAX_USERS)
    {
        // Remove newline character at end
        line[strcspn(line, "\n")] = '\0';

        // Split by comma
        char *comma = strchr(line, ',');
        if (comma == NULL)
            continue; // skip malformed lines

        // Split the line into two parts at the comma
        *comma = '\0';
        strncpy(users[*count].username, line, MAX_USERNAME - 1);
        strncpy(users[*count].password, comma + 1, MAX_PASSWORD - 1);
        (*count)++;
    }

    fclose(f);
    return true;
}

// ══════════════════════════════════════════
//   SaveUser
//   Appends a new user to users.txt.
//   Returns true if successful.
// ══════════════════════════════════════════
bool SaveUser(User *user)
{
    // "a" mode = append — adds to end of file
    // without deleting existing content
    FILE *f = fopen(USERS_FILE, "a");
    if (f == NULL)
        return false;

    fprintf(f, "%s,%s\n", user->username, user->password);
    fclose(f);
    return true;
}

// ══════════════════════════════════════════
//   UserExists
//   Returns true if username is already
//   registered in users.txt
// ══════════════════════════════════════════
bool UserExists(const char *username)
{
    User users[MAX_USERS];
    int count = 0;
    LoadUsers(users, &count);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(users[i].username, username) == 0)
            return true;
    }
    return false;
}

// ══════════════════════════════════════════
//   ValidateLogin
//   Returns true if username + password
//   match a record in users.txt
// ══════════════════════════════════════════
bool ValidateLogin(const char *username, const char *password)
{
    User users[MAX_USERS];
    int count = 0;
    LoadUsers(users, &count);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0)
            return true;
    }
    return false;
}

// ══════════════════════════════════════════
//   DeleteUser
//   Removes a user from users.txt by
//   rewriting the file without that user.
// ══════════════════════════════════════════
bool DeleteUser(const char *username)
{
    // ── Step 1: Remove user from users.txt ──
    User users[MAX_USERS];
    int count = 0;
    LoadUsers(users, &count);

    FILE *f = fopen(USERS_FILE, "w");
    if (f == NULL)
        return false;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(users[i].username, username) != 0)
            fprintf(f, "%s,%s\n",
                    users[i].username,
                    users[i].password);
    }
    fclose(f);

    // ── Step 2: Remove all messages involving this user ──
    // Load all messages
    Message messages[MAX_MESSAGES];
    int msgCount = 0;
    LoadMessages(messages, &msgCount);

    // Rewrite messages.txt skipping any message
    // where this user is the sender OR recipient
    FILE *mf = fopen(MESSAGES_FILE, "w");
    if (mf == NULL)
        return false;

    for (int i = 0; i < msgCount; i++)
    {
        bool userIsSender =
            strcmp(messages[i].sender, username) == 0;
        bool userIsRecipient =
            strcmp(messages[i].recipient, username) == 0;

        // Only keep messages that don't involve this user
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

// ══════════════════════════════════════════
//   LoadMessages
//   Reads all messages from messages.txt
// ══════════════════════════════════════════
bool LoadMessages(Message *messages, int *count)
{
    *count = 0;

    FILE *f = fopen(MESSAGES_FILE, "r");
    if (f == NULL)
        return false;

    // Each line: sender,recipient,content,timestamp
    char line[512];
    while (fgets(line, sizeof(line), f) && *count < MAX_MESSAGES)
    {
        line[strcspn(line, "\n")] = '\0';

        // Parse each comma-separated field
        char *p1 = strtok(line, ",");
        char *p2 = strtok(NULL, ",");
        char *p3 = strtok(NULL, ",");
        char *p4 = strtok(NULL, ",");

        if (!p1 || !p2 || !p3 || !p4)
            continue;

        strncpy(messages[*count].sender, p1, MAX_USERNAME - 1);
        strncpy(messages[*count].recipient, p2, MAX_USERNAME - 1);
        strncpy(messages[*count].content, p3, MAX_MESSAGE - 1);
        strncpy(messages[*count].timestamp, p4, 19);
        (*count)++;
    }

    fclose(f);
    return true;
}

// ══════════════════════════════════════════
//   SaveMessage
//   Appends a new message to messages.txt
// ══════════════════════════════════════════
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