# ChatLink — One-on-One Chat Application

> **Language:** C (Structured Programming) + Raylib GUI Library

---

## What is ChatLink?

ChatLink is a standalone one-on-one chat application built in C using the Raylib graphics library. It runs entirely on a single machine and allows multiple users to:

- Register and create an account
- Login and logout securely
- Search for other registered users
- Send and receive messages
- View full chat history
- Deregister and delete their account

All data (users and messages) is saved to plain text files so everything persists between sessions.

---

## What You Need to Install

You only need **one thing** installed — the Raylib package for Windows, which includes both the compiler (GCC) and the Raylib graphics library bundled together.

| Requirement      | Version  | Notes                          |
| ---------------- | -------- | ------------------------------ |
| Raylib w64devkit | 5.x      | Includes GCC compiler + Raylib |
| Windows          | 10 or 11 | 64-bit                         |

> No need to install MinGW, GCC, or anything else separately — Raylib's package includes everything.

---

## Step 1 — Install the Raylib Package

### 1.1 Download Raylib

1. Go to **https://github.com/raysan5/raylib/releases**
2. Find the latest release (e.g. `raylib-5.0`)
3. Download the file named: **`raylib-5.0_win64_mingw-w64.zip`**
   - Must say `win64`
   - Must say `mingw-w64`
   - Download the ZIP file

### 1.2 Extract and Place

1. Right-click the downloaded ZIP → **Extract All**
2. Move the extracted folder so the final path is exactly:

```
C:\raylib\
```

3. Inside, confirm these paths exist:

```
C:\raylib\w64devkit\bin\gcc.exe
C:\raylib\w64devkit\x86_64-w64-mingw32\include\raylib.h
C:\raylib\w64devkit\x86_64-w64-mingw32\lib\libraylib.a
```

> If your folder structure looks different, adjust the compile command in Step 3 to match your actual paths.

---

## Step 2 — Set Up the Project

### 2.1 Place the Project Folder

Copy the entire **ChatLink** project folder anywhere on your computer, for example:

```
C:\ChatLink\
```

Make sure the folder structure looks like this:

```
ChatLink/
  ├── src/
  │     ├── main.c
  │     ├── ui.c
  │     ├── welcome.c
  │     ├── auth.c
  │     ├── fileio.c
  │     ├── chat.c
  │     └── search.c
  ├── include/
  │     ├── screen.h
  │     ├── ui.h
  │     ├── welcome.h
  │     ├── auth.h
  │     ├── fileio.h
  │     ├── chat.h
  │     └── search.h
  ├── data/
  │     ├── users.txt       (created automatically on first run)
  │     └── messages.txt    (created automatically on first run)
  └── README.md
```

### 2.2 Create the data folder

If the `data/` folder does not exist, create it manually:

1. Open the ChatLink folder in File Explorer
2. Right-click inside → **New → Folder**
3. Name it exactly `data`

> The app will automatically create `users.txt` and `messages.txt` inside it when the first user registers.

---

## Step 3 — Compile the Code

### Option A — Using Command Prompt (Recommended)

1. Open **Command Prompt** — press `Windows key`, type `cmd`, press Enter
2. Navigate to your ChatLink folder:

```
cd C:\ChatLink
```

3. Copy and paste this compile command exactly:

```
C:\raylib\w64devkit\bin\gcc.exe src/main.c src/ui.c src/welcome.c src/auth.c src/fileio.c src/chat.c src/search.c -o ChatLink.exe -I C:/raylib/w64devkit/x86_64-w64-mingw32/include -L C:/raylib/w64devkit/x86_64-w64-mingw32/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lm -Wall
```

4. If successful you will see **no error messages** and `ChatLink.exe` will appear in your ChatLink folder

### Option B — Using VS Code

1. Open VS Code → **File → Open Folder** → select your ChatLink folder
2. Press **`Ctrl+Shift+B`** to build
3. Check the terminal at the bottom — no errors means success

---

## Step 4 — Run the App

### From Command Prompt

In the same Command Prompt window (make sure you are inside the ChatLink folder):

```
.\ChatLink.exe
```

### From File Explorer

Double-click `ChatLink.exe` directly in File Explorer.

> A window titled **"ChatLink - One on One Chat"** will open and you are ready to go!

---

## How to Use ChatLink

### Registering an Account (First Time)

1. On the **Welcome Screen**, click **Register**
2. Type a username — e.g. `alice`
3. Type a password — e.g. `pass123`
4. Click **Register** — you are logged in automatically

### Logging Into an Existing Account

1. On the **Welcome Screen**, click **Login**
2. Enter your username and password
3. Click **Login** or press **Enter**

### Sending a Message

1. After logging in you will see the **Chat Screen**
2. Click any contact name in the left sidebar
3. Type your message in the input box at the bottom
4. Press **Enter** or click **>>** to send
5. Your messages appear as **red bubbles** on the right
6. Their messages appear as **dark blue bubbles** on the left

### Searching for a User

1. Click **Search User** in the sidebar
2. Type the exact username you are looking for
3. Press **Enter** or click **Search**
4. If found, a card appears — click **Open Chat** to chat with them

### Logging Out

Click the **Logout** button at the bottom left of the sidebar.

### Deleting Your Account

1. Click the **Delete** button at the bottom left of the sidebar
2. A confirmation dialog will appear
3. Click **Yes, Delete** to permanently remove your account and all your messages
4. Click **Cancel** to go back

---

## Testing with Multiple Users

To simulate two people chatting on the same machine:

1. Register as **alice** → send a message to `bob` → Logout
2. Register as **bob** → click alice in the sidebar → see alice's message → reply → Logout
3. Login as **alice** again → open chat with bob → see his reply

> All messages are saved to `data/messages.txt` so they persist even after closing the app.

---

## Project File Structure

```
ChatLink/
│
├── src/                    C source files — the logic
│   ├── main.c              Entry point and main game loop
│   ├── ui.c                Drawing helpers: buttons, bubbles, inputs
│   ├── welcome.c           Welcome screen
│   ├── auth.c              Login and registration
│   ├── fileio.c            File read/write for users and messages
│   ├── chat.c              Chat screen, contacts, messaging
│   └── search.c            User search screen
│
├── include/                Header files — declarations
│   ├── screen.h            AppScreen enum
│   ├── ui.h                Colors, Button and InputField structs
│   ├── welcome.h           WelcomeScreen declarations
│   ├── auth.h              AuthScreen declarations
│   ├── fileio.h            User and Message structs, file functions
│   ├── chat.h              ChatScreen declarations
│   └── search.h            SearchScreen declarations
│
├── data/                   Auto-created on first run
│   ├── users.txt           Stores registered users as username,password
│   └── messages.txt        Stores all messages as sender,recipient,content,time
│
├── ChatLink.exe            The compiled app (created after building)
└── README.md               This file
```

---

## Troubleshooting

| Problem                                  | Likely Cause                   | Fix                                                                              |
| ---------------------------------------- | ------------------------------ | -------------------------------------------------------------------------------- |
| `gcc.exe not found`                      | Wrong compiler path            | Use the full path `C:\raylib\w64devkit\bin\gcc.exe` in the compile command       |
| `raylib.h: No such file`                 | Wrong include path             | Confirm `raylib.h` exists at `C:\raylib\w64devkit\x86_64-w64-mingw32\include\`   |
| `undefined reference` errors             | Compiler/library mismatch      | Always use `C:\raylib\w64devkit\bin\gcc.exe` — never mix with other GCC installs |
| Window closes immediately                | Runtime crash                  | Run from Command Prompt to see the error message printed before it closes        |
| `data/` folder missing                   | Folder not created             | Manually create a folder named `data` inside the ChatLink folder before running  |
| Login fails                              | Wrong password                 | Passwords are case-sensitive — type exactly what you used when registering       |
| Old messages appear after re-registering | Messages not cleared on delete | Make sure you click Delete Account (not just Logout) before re-registering       |
| Contacts not showing in sidebar          | No other users registered      | Register at least one other account so it appears as a contact                   |

---

> **Built with:** C · Raylib 5.x · GCC w64devkit · Windows 64-bit  
> **Assignment 1** — Standalone single-machine chat application  
> **Course:** Network and Distributed Programming
