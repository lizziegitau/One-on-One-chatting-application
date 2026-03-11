#include "raylib.h"
#include "../include/ui.h"
#include <string.h> // for strlen, strcpy
#include <stdio.h>  // for sprintf

// ══════════════════════════════════════════
//   DrawPanel
//   Draws a filled rectangle with a border.
//   Used for cards, sidebars and popups.
// ══════════════════════════════════════════
void DrawPanel(int x, int y, int w, int h, Color color, Color borderColor)
{
    // Draw the filled background
    DrawRectangle(x, y, w, h, color);

    // Draw a 1-pixel border around it
    DrawRectangleLines(x, y, w, h, borderColor);
}

// ══════════════════════════════════════════
//   DrawButton
//   Draws a button and changes color when
//   the mouse hovers over it.
// ══════════════════════════════════════════
void DrawButton(Button *btn, Color bgColor, Color textColor)
{
    // Check if mouse is hovering over this button
    btn->hovered = CheckCollisionPointRec(
        GetMousePosition(), // where is the mouse?
        btn->rect           // does it overlap the button's rectangle?
    );

    // If hovered, make the button slightly brighter
    Color drawColor = btn->hovered
                          ? (Color){bgColor.r + 20, bgColor.g + 20, bgColor.b + 20, 255}
                          : bgColor;

    // Draw rounded rectangle for the button background
    DrawRectangleRounded(btn->rect, 0.3f, 6, drawColor);

    // Measure text width so we can center it
    int fontSize = 16;
    int textWidth = MeasureText(btn->label, fontSize);

    // Draw the label centered inside the button
    DrawText(
        btn->label,
        (int)(btn->rect.x + btn->rect.width / 2 - textWidth / 2),
        (int)(btn->rect.y + btn->rect.height / 2 - fontSize / 2),
        fontSize,
        textColor);

    // If hovered, draw a subtle border to highlight it
    if (btn->hovered)
        DrawRectangleRoundedLines(btn->rect, 0.3f, 6, COLOR_ACCENT);
}

// ══════════════════════════════════════════
//   IsButtonClicked
//   Returns true if the button was clicked
//   this frame (hovered + left mouse pressed)
// ══════════════════════════════════════════
bool IsButtonClicked(Button *btn)
{
    return btn->hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ══════════════════════════════════════════
//   DrawInputField
//   Draws a text input box. If active,
//   shows a blinking cursor.
// ══════════════════════════════════════════
void DrawInputField(InputField *field)
{
    // Change border color if field is active (selected)
    Color borderColor = field->active ? COLOR_ACCENT : COLOR_INPUT_BORDER;

    // Draw the background box
    DrawRectangleRounded(field->rect, 0.2f, 6, COLOR_INPUT_BG);

    // Draw the border
    DrawRectangleRoundedLines(field->rect, 0.2f, 6, borderColor);

    // Decide what text to display
    int fontSize = 16;
    Color textCol = COLOR_TEXT;

    if (field->textLength == 0)
    {
        // Show placeholder text in muted grey
        DrawText(
            field->placeholder,
            (int)field->rect.x + 12,
            (int)field->rect.y + (int)field->rect.height / 2 - fontSize / 2,
            fontSize,
            COLOR_MUTED);
    }
    else if (field->isPassword)
    {
        // Show asterisks for password fields
        char masked[256] = {0};
        for (int i = 0; i < field->textLength; i++)
            masked[i] = '*';
        DrawText(
            masked,
            (int)field->rect.x + 12,
            (int)field->rect.y + (int)field->rect.height / 2 - fontSize / 2,
            fontSize,
            textCol);
    }
    else
    {
        // Show the actual typed text
        DrawText(
            field->text,
            (int)field->rect.x + 12,
            (int)field->rect.y + (int)field->rect.height / 2 - fontSize / 2,
            fontSize,
            textCol);
    }

    // Draw blinking cursor if field is active
    if (field->active)
    {
        // GetTime() returns seconds since start
        // We use it to make the cursor blink on/off
        if ((int)(GetTime() * 2) % 2 == 0)
        {
            // Calculate cursor x position (after the last character)
            int cursorX = (int)field->rect.x + 12 + MeasureText(field->isPassword ? (char[256]){[0 ... 255] = 0} // workaround
                                                                                  : field->text,
                                                                fontSize);

            // For password fields, measure asterisks instead
            if (field->isPassword)
            {
                char masked[256] = {0};
                for (int i = 0; i < field->textLength; i++)
                    masked[i] = '*';
                cursorX = (int)field->rect.x + 12 + MeasureText(masked, fontSize);
            }

            int cursorY = (int)field->rect.y + 6;
            DrawLine(cursorX, cursorY,
                     cursorX, cursorY + (int)field->rect.height - 12,
                     COLOR_ACCENT);
        }
    }
}

// ══════════════════════════════════════════
//   HandleInputField
//   Processes keyboard input for a text field.
//   Call this every frame for the active field.
// ══════════════════════════════════════════
void HandleInputField(InputField *field)
{
    if (!field->active)
        return;

    // Get any character key pressed this frame
    int key = GetCharPressed();

    // Keep reading keys until none are left in the queue
    while (key > 0)
    {
        // Only accept printable characters, max 255 chars
        if (key >= 32 && key <= 125 && field->textLength < 255)
        {
            field->text[field->textLength] = (char)key;
            field->textLength++;
            field->text[field->textLength] = '\0'; // always null-terminate
        }
        key = GetCharPressed(); // check for more keys
    }

    // Handle backspace — delete the last character
    if (IsKeyPressed(KEY_BACKSPACE) && field->textLength > 0)
    {
        field->textLength--;
        field->text[field->textLength] = '\0';
    }
}

// ══════════════════════════════════════════
//   ClearInputField
//   Wipes all text from an input field.
//   Call this after a form is submitted.
// ══════════════════════════════════════════
void ClearInputField(InputField *field)
{
    memset(field->text, 0, sizeof(field->text));
    field->textLength = 0;
    field->active = false;
}

// ══════════════════════════════════════════
//   DrawAvatar
//   Draws a colored circle with an initial
//   letter — like contact profile pictures.
// ══════════════════════════════════════════
void DrawAvatar(int x, int y, int radius, Color color, char initial)
{
    // Draw filled circle
    DrawCircle(x, y, (float)radius, color);

    // Draw the initial letter centered in the circle
    char str[2] = {initial, '\0'};
    int fw = MeasureText(str, 18);
    DrawText(str, x - fw / 2, y - 9, 18, WHITE);
}

// ══════════════════════════════════════════
//   DrawChatBubble
//   Draws a message bubble.
//   isSent = true  → right side, red   (your message)
//   isSent = false → left side,  blue  (their message)
// ══════════════════════════════════════════
void DrawChatBubble(const char *text, int x, int y, int maxWidth, bool isSent)
{
    int fontSize = 15;
    int padding = 12;
    int textWidth = MeasureText(text, fontSize);

    // Cap bubble width at maxWidth
    int bubbleWidth = textWidth + padding * 2;
    if (bubbleWidth > maxWidth)
        bubbleWidth = maxWidth;

    int bubbleHeight = 40;

    // Position: sent messages go on the right, received on the left
    int bx = isSent ? x - bubbleWidth : x;

    Rectangle bubble = {(float)bx, (float)y,
                        (float)bubbleWidth, (float)bubbleHeight};

    // Color: red for sent, dark blue for received
    Color bgCol = isSent ? COLOR_ACCENT : COLOR_CARD;

    DrawRectangleRounded(bubble, 0.4f, 8, bgCol);
    DrawText(text,
             bx + padding,
             y + bubbleHeight / 2 - fontSize / 2,
             fontSize, WHITE);
}

// ══════════════════════════════════════════
//   DrawCenteredText
//   Draws text horizontally centered on screen.
//   Useful for titles and headings.
// ══════════════════════════════════════════
void DrawCenteredText(const char *text, int y, int fontSize, Color color)
{
    int screenWidth = GetScreenWidth();
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, screenWidth / 2 - textWidth / 2, y, fontSize, color);
}

// ══════════════════════════════════════════
//   DrawToast
//   Draws a temporary notification that
//   fades away. Call every frame while
//   timer > 0. Timer counts down to 0.
// ══════════════════════════════════════════
void DrawToast(const char *message, bool isOk, float *timer)
{
    if (*timer <= 0)
        return;

    // Count the timer down using frame time
    *timer -= GetFrameTime();

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Fade out as timer approaches 0
    unsigned char alpha = (unsigned char)((*timer / 2.0f) * 255);
    if (alpha > 255)
        alpha = 255;

    Color bgCol = isOk
                      ? (Color){29, 209, 161, alpha} // green for success
                      : (Color){233, 69, 96, alpha}; // red for error

    int tw = MeasureText(message, 16);
    int bw = tw + 40;
    int bh = 44;
    int bx = screenWidth / 2 - bw / 2;
    int by = screenHeight - 70;

    DrawRectangleRounded(
        (Rectangle){(float)bx, (float)by, (float)bw, (float)bh},
        0.4f, 8, bgCol);
    DrawText(message, bx + 20, by + bh / 2 - 8, 16,
             (Color){255, 255, 255, alpha});
}