/*
 * Copyright (C) 2026 TheCRatTime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Print PreProcessor. */
static const char* PrintPP(const char* text) {
  fputs("\033[31m", stdout);

  putchar(*(text++));

  while(*text) {
    if (*text != ' ') {
      break;
    }

    putchar(*(text++));
  }
  
  while(*text) {
    if (*text == ' ') {
      break;
    }

    putchar(*(text++));
  }

  fputs("\033[34m", stdout);

repeat:
  while(*text) {
    if (*text == '\n') {
      if (*(text-1) == '\\') {
        putchar(*(text++));
        goto repeat;
      } else {
        break;
      }
    }

    putchar(*(text++));
  }
  
  fputs("\033[0m", stdout);
  
  return text;
}

/* '"' quote print. */
static const char* PrintDoubleQuotes(const char* text,
                                     const char* const first_ptr) {
  fputs("\033[32m", stdout);
  putchar(*(text++));

  while(*text) {
    if (*text == '"') {
      const char* saved = text;
      int slashes = 0;
      
      saved--;

      while(*saved == '\\' && saved != first_ptr) {
        slashes++;
        saved--;
      }

      if ((slashes & 0x1) == 0) {
        break;
      }
    }

    putchar(*(text++));
  }

  putchar(*(text++));

  fputs("\033[0m", stdout);

  return text;
}

/* "'" quote print. */
static const char* PrintSingleQuotes(const char* text,
                                     const char* const first_ptr) {
  fputs("\033[32m", stdout);
  putchar(*(text++));

  while(*text) {
    if (*text == '\'') {
      const char* saved = text;
      int slashes = 0;
      
      saved--;

      while(*saved == '\\' && saved != first_ptr) {
        slashes++;
        saved--;
      }

      if ((slashes & 0x1) == 0) {
        break;
      }
    }

    putchar(*(text++));
  }

  putchar(*(text++));

  fputs("\033[0m", stdout);

  return text;
}

/* Print digit. */
static const char* PrintDigit(const char* text) {
  fputs("\033[36m", stdout);
  
  while((*text >= '0' && *text <= '9') || *text == 'x') {
    putchar(*(text++));
  }
  
  fputs("\033[0m", stdout);
  return text;
}

/* Punctuation print. */
static const char* PutPunct(const char* text) {
  fputs("\033[34m", stdout);
  
  while (*text == '&' || *text == '*' || *text == '=' ||
         *text == '<' || *text == '>' || *text == ',' ||
         *text == '%' || *text == '+' || *text == '-' ||
         *text == '.' || *text == ';' || *text == '!' ||
         *text == ':' || *text == '|') {
    putchar(*(text++));
  }
  
  fputs("\033[0m", stdout);
  
  return text;
}

/* Keyword checker. */
#define TOTAL_KEYWORDS 34
static const char* PrintKeyword(const char* text,
                                const char* const first_ptr) {
  int i = 0;
  
  const struct {
    char* name;
    int len;
    char unused[4];
  } Keywords[TOTAL_KEYWORDS] = {
    {"int", 3, {0}},
    {"char", 4, {0}},
    {"const", 5, {0}},
    {"union", 5, {0}},
    {"struct", 6, {0}},
    {"if", 2, {0}},
    {"for", 3, {0}},
    {"else", 4, {0}},
    {"restrict", 8, {0}},
    {"extern", 6, {0}},
    {"inline", 6, {0}},
    {"bool", 4, {0}},
    {"true", 4, {0}},
    {"false", 5, {0}},
    {"sizeof", 6, {0}},
    {"unsigned", 8, {0}},
    {"signed", 6, {0}},
    {"long", 4, {0}},
    {"short", 5, {0}},
    {"switch", 6, {0}},
    {"case", 4, {0}},
    {"break", 5, {0}},
    {"while", 5, {0}},
    {"continue", 8, {0}},
    {"return", 6, {0}},
    {"static", 6, {0}},
    {"void", 4, {0}},
    {"double", 6, {0}},
    {"float", 5, {0}},
    {"typedef", 7, {0}},
    {"size_t", 6, {0}},
    {"FILE", 4, {0}},
    {"goto", 4, {0}},
    {"default", 7, {0}}
  };

  for(i = 0; i < TOTAL_KEYWORDS; i++) {
    if (!strncmp(text, Keywords[i].name, (size_t)Keywords[i].len)) {
      if (text != first_ptr && *(text-1) != ' ' &&
          *(text-1) != '\t' && *(text-1) != '(' &&
          *(text-1) != '\n' && *(text-1) != ';') {
        return text;
      }

      if (*(text+Keywords[i].len) != ' ' &&
          *(text+Keywords[i].len) != '*' &&
          *(text+Keywords[i].len) != ';' &&
          *(text+Keywords[i].len) != '(' &&
          *(text+Keywords[i].len) != ':' &&
          *(text+Keywords[i].len) != ')') {
        return text;
      }

      fputs("\033[31m", stdout);
      fwrite(Keywords[i].name, 1, (size_t)Keywords[i].len, stdout);
      if (*(text+Keywords[i].len) == '*') {
        fputs("\033[34m*", stdout);
        text++;
      }
      
      fputs("\033[0m", stdout);
      return text+Keywords[i].len;
    }
  }
  return text;
}

/* Print comments in code. */
static const char* PrintComment(const char* text,
                                const char* const first_ptr) {
  if (text != first_ptr) {
    if (*(text-1) != '\n') {
      fputs("\033[34m/\033[0m", stdout);
      return ++text;
    }
  }

  if (*(text+1) == '/') {
    fputs("\033[2m", stdout);
    while (*text) {
      if (*text == '\n') {
        break;
      }

      putchar(*(text++));
    }

    fputs("\033[0m", stdout);
  } else if (*(text+1) == '*') {
    fputs("\033[2m/*", stdout);

    text += 2;
    while (*text) {
      if (*(text-1) == '*' && *text == '/') {
        break;
      }

      putchar(*(text++));
    }

    fputs("/\033[0m", stdout);
    text++;
  }

  return text;
}

/* Main function for parsing string. */
static void ParseString(const char* text) {
  const char* const start_ptr = text;
  
  while(*text) {
    const char* test = PrintKeyword(text, start_ptr);

    if (test != text) {
      text = test;
    }

    if (*text >= '0' && *text <= '9') {
      text = PrintDigit(text);
    }
    
    switch(*text) {
      case '&':
      case '*':
      case '=':
      case '<':
      case '>':
      case ',':
      case '.':
      case '%':
      case '+':
      case '-':
      case ';':
      case '!':
      case ':':
      case '|':
        text = PutPunct(text);
        break;
      case '#':
        text = PrintPP(text);
        break;
      case '"':
        text = PrintDoubleQuotes(text, start_ptr);
        break;
      case '\'':
        text = PrintSingleQuotes(text, start_ptr);
        break;
      case '/':
        text = PrintComment(text, start_ptr);
        break;
      default:
        break;
    }

    if (*text >= '0' && *text <= '9') {
      text = PrintDigit(text);
    }

    if (*text != '\0') {
      switch(*text) {
        case '&':
        case '*':
        case '=':
        case '<':
        case '>':
        case ',':
        case '.':
        case '%':
        case '+':
        case '-':
        case ';':
        case '!':
        case ':':
        case '|':
          text = PutPunct(text);
          break;
        default:
          putchar(*(text++));
          break;
      }
    }
  }
}

/* Main function. */
int main(int argc, char** argv) {
  char* buffer = NULL;
  FILE* file = NULL;
  size_t size = 0;
  
  if (argc < 2) {
    printf("Usage: %s FILE\n", argv[0]);
    return 1;
  }

  file = fopen(argv[1], "rb");
  if (file == NULL) {
    perror(argv[0]);
    return 1;
  }

  fseek(file, 0, SEEK_END);
  size = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer = (char*)malloc(size+1);
  if (buffer == NULL) {
    perror(argv[0]);
    fclose(file);
    return 1;
  }

  if (fread(buffer, 1, size, file) <= 0) {
    perror(argv[0]);
    fclose(file);
    free(buffer);
    return 1;
  }

  buffer[size] = '\0';

  fclose(file);

  ParseString(buffer);

  free(buffer);
  return 0;
}
