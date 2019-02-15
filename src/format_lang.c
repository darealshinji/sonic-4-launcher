/**
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

// compile: cl /O2 format_lang.c
// usage: format_lang.exe < lang.txt > lang.h

#include <stdio.h>


int main(void)
{
  char c = getchar();
  char hex, newEntry;
  size_t i;

  const char *ui[] = {
    "Settings",
    "GraphicsSettings",
    "GraphicsDevice",
    "Resolution",
    "Fullscreen",
    "AudioSettings",
    "OutputDevice",
    "Language",
    "ControllerSelection",
    "AdditionalController",
    "ControllerNumber",
    "Layout",
    "ConfigurationLayout",
    "Movement",
    "Action",
    "Up",
    "Down",
    "Left",
    "Right",
    "Jump",
    "Start",
    "SaveSettings",
    "Player",
    "Select",
    "Back",
    "Keyboard",
    "Gamepad",
    "ScoreAttack",
    "Press",
    "ResetToDefault",
    "Configuration",
    "ConfigurationSaved",
    "SuperSonic",
    "Vibrate",
    "Leaderboards",
    NULL
  };

  if (c == EOF || c == '\0') {
    return 1;
  }

  printf("const char *ui_%s[] = { \"", ui[0]);
  newEntry = 0;
  hex = 0;
  i = 1;

  while (c != EOF && c != 0) {
    if (c >= ' ' && c <= '~') {
      if (c == '|') {
        printf("\", \"");
        newEntry = 1;
        hex = 0;
      } else {
        if (hex) {
          printf("\" \"");
        }
        putchar(c);
        newEntry = 0;
        hex = 0;
      }
    } else if (c >= '\a' && c <= '\r') {
      if (c == '\n') {
        printf("\" };\n");
        if (ui[i] == NULL) {
          return 0;
        }
        printf("const char *ui_%s[] = { \"", ui[i]);
        newEntry = 0;
        hex = 0;
        i++;
      }
    } else {
      if (!hex) {
        if (!newEntry) {
          printf("\" \"");
        }
      }
      printf("\\x%02X", (unsigned char)c);
      newEntry = 0;
      hex = 1;
    }

    c = getchar();
  }

  if (!hex) {
    putchar('"');
  }
  printf(" };\n");

  return 0;
}

