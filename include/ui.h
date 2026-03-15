#ifndef UI_H
#define UI_H
/* Screen dimensions, window size of 860x580 pixels */
#define SCREEN_W 860
#define SCREEN_H 580

#include "raylib.h"

/* Colour Palette */
/* Background Colours */
#define COLOR_BG (Color){26, 27, 46, 255}      /* Main Background -Dark navy */
#define COLOR_SURFACE (Color){22, 33, 62, 255} /* Panel Background - Slightly lighter navy */
#define COLOR_CARD (Color){15, 52, 96, 255}    /* Card Background - Dark blue card */
#define COLOR_SIDEBAR (Color){13, 27, 46, 255} /* Sidebar background */

/* Text Colours */
#define COLOR_TEXT (Color){234, 234, 234, 255}  /* Primary Text Colour - Off-white*/
#define COLOR_MUTED (Color){136, 146, 164, 255} /* Secondary Text Colour - Grey text */

/* Accent Colours */
#define COLOR_ACCENT (Color){233, 69, 96, 255}   /* Red */
#define COLOR_ACCENT2 (Color){245, 166, 35, 255} /* Orange */
#define COLOR_TEAL (Color){0, 210, 211, 255}     /* Teal */
#define COLOR_GREEN (Color){29, 209, 161, 255}   /* Green */

/* Input Field Colours */
#define COLOR_INPUT_BG (Color){15, 30, 46, 255}     /* Input field background */
#define COLOR_INPUT_BORDER (Color){26, 58, 85, 255} /* Input field border */
#define COLOR_BORDER (Color){30, 42, 58, 255}       /* General border color */

/* Reusable UI Components */
/* Button Struct */
typedef struct
{
    Rectangle rect;
    const char *label;
    bool hovered;
} Button;

/* Input Field Struct */
typedef struct
{
    Rectangle rect;
    char text[256];
    int textLength;
    bool active;
    const char *placeholder;
    bool isPassword;
} InputField;

/* Drawing Functions */
/* Draws filled rectangle */
void DrawPanel(int x, int y, int w, int h, Color color, Color borderColor);
/* Draws a Button Struct */
void DrawButton(Button *btn, Color bgColor, Color textColor);
/* Draws an Input Field Struct */
void DrawInputField(InputField *field);
/* Draws a Profile Avatar */
void DrawAvatar(int x, int y, int radius, Color color, char initial);
/* Draws a Single Chat Message Bubble */
void DrawChatBubble(const char *text, int x, int y, int maxWidth, bool isSent);
/* Draws centered text */
void DrawCenteredText(const char *text, int y, int fontSize, Color color);
/* Draws Temporary Toast Notification */
void DrawToast(const char *message, bool isOk, float *timer);

/* Input Handling Functions */
bool IsButtonClicked(Button *btn);
void HandleInputField(InputField *field);
void ClearInputField(InputField *field);

#endif