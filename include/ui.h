#ifndef UI_H
#define UI_H
#define SCREEN_W 860
#define SCREEN_H 580

#include "raylib.h"

//   APP COLOR PALETTE

// Background colors
#define COLOR_BG (Color){26, 27, 46, 255}      // Dark navy  #1A1B2E
#define COLOR_SURFACE (Color){22, 33, 62, 255} // Slightly lighter navy
#define COLOR_CARD (Color){15, 52, 96, 255}    // Dark blue card #0F3460
#define COLOR_SIDEBAR (Color){13, 27, 46, 255} // Sidebar background

// Text colors
#define COLOR_TEXT (Color){234, 234, 234, 255}  // Off-white #EAEAEA
#define COLOR_MUTED (Color){136, 146, 164, 255} // Grey text #8892A4

// Accent colors
#define COLOR_ACCENT (Color){233, 69, 96, 255}   // Red #E94560
#define COLOR_ACCENT2 (Color){245, 166, 35, 255} // Orange #F5A623
#define COLOR_TEAL (Color){0, 210, 211, 255}     // Teal #00D2D3
#define COLOR_GREEN (Color){29, 209, 161, 255}   // Green #1DD1A1

// UI element colors
#define COLOR_INPUT_BG (Color){15, 30, 46, 255}     // Input field background
#define COLOR_INPUT_BORDER (Color){26, 58, 85, 255} // Input field border
#define COLOR_BORDER (Color){30, 42, 58, 255}       // General border color

//   UI STRUCTS

// Represents a clickable button
typedef struct
{
    Rectangle rect;    // x, y, width, height of the button
    const char *label; // text shown on the button
    bool hovered;      // is the mouse currently over it?
} Button;

// Represents a text input field (like a username box)
typedef struct
{
    Rectangle rect;          // x, y, width, height
    char text[256];          // what the user has typed so far
    int textLength;          // how many characters typed
    bool active;             // is this field currently selected?
    const char *placeholder; // grey hint text shown when empty
    bool isPassword;         // if true, show *** instead of text
} InputField;

//   FUNCTION DECLARATIONS

// Drawing functions
void DrawPanel(int x, int y, int w, int h, Color color, Color borderColor);
void DrawButton(Button *btn, Color bgColor, Color textColor);
void DrawInputField(InputField *field);
void DrawAvatar(int x, int y, int radius, Color color, char initial);
void DrawChatBubble(const char *text, int x, int y, int maxWidth, bool isSent);
void DrawCenteredText(const char *text, int y, int fontSize, Color color);
void DrawToast(const char *message, bool isOk, float *timer);

// Input handling functions
bool IsButtonClicked(Button *btn);
void HandleInputField(InputField *field);
void ClearInputField(InputField *field);

#endif