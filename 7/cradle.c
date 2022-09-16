// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char labelName[MAX_LABEL_LEN];
static int LCount = 0;
char *token_buf;
char Value[MAX_TOKEN_LEN];
char Look = 0;
char Token = 0;

Symbol KWList[] = {{"IF"},    {"ELSE"},  {"ENDIF"},  {"END"},
                   {"WHILE"}, {"LOOP"},  {"REPEAT"}, {"FOR"},
                   {"DO"},    {"BREAK"}, {"UNTIL"}};

const char KWCode[] = "xileewprfdbu";

void Halt() {
  printf("\n");
  for (;;)
    ;
  //  exit(1);
}

void GetChar() { Look = getchar(); }

void Error(char *s) { printf("\nError: %s.", s); }

void Abort(char *s) {
  Error(s);
  Halt();
}

void Expected(char *s) {
  char tmp[MAX_SPRINTF_LEN];
  snprintf(tmp, MAX_SPRINTF_LEN, "%s Expected", s);
  Abort(tmp);
}

char *NewLabel() {
  snprintf(labelName, MAX_SPRINTF_LEN, "L%02d", LCount);
  LCount++;
  return labelName;
}

void PostLabel(char *L) { printf("%s:\n", L); }

void Fin() {
  if (Look == '\r')
    GetChar();
  if (Look == '\n')
    GetChar();
}

void Match(char x) {
  if (Look == x) {
    GetChar();
    SkipWhite();
  } else {
    char tmp[MAX_SPRINTF_LEN];
    snprintf(tmp, MAX_SPRINTF_LEN, "' %c ' ", x);
    Expected(tmp);
  }
}

void MatchString(char *x) {
  if (strncmp(Value, x, MAX_TOKEN_LEN) != 0) {
    char tmp[MAX_SPRINTF_LEN];
    snprintf(tmp, MAX_SPRINTF_LEN, "' %s ' ", x);
    Expected(tmp);
  }
}

int IsAlpha(char c) {
  char uc = UpCase(c);
  return (uc >= 'A' && uc <= 'Z') || (uc >= 'a' && uc <= 'z');
}

int IsDigit(char c) { return (c >= '0' && c <= '9'); }

int IsAlNum(char c) { return IsAlpha(c) || IsDigit(c); }

// \f\n\r\t\v
int IsWhite(char c) { return strchr(" \t", c) != 0; }

void SkipWhite() {
  while (IsWhite(Look)) {
    GetChar();
  }
}

void GetName() {
  token_buf = Value;

  while (Look == '\n')
    Fin();

  if (!IsAlpha(Look)) {
    Expected("Name");
  }

  while (IsAlNum(Look)) {
    *token_buf = UpCase(Look);
    token_buf++;
    GetChar();
  }
  *token_buf = '\0';
  SkipWhite();
}

void Scan() {
  GetName();
  Token =
      KWCode[Lookup(&KWList[0], Value, sizeof(KWList) / sizeof(Symbol)) + 1];
  SkipWhite();
}

void GetOp() {
  token_buf = Value;

  if (!IsOp(Look)) {
    Expected("Operand");
  }

  *token_buf = Look;
  token_buf++;
  GetChar();
  *token_buf = '\0';
  Token = '?';
}

void GetNum() {
  token_buf = Value;
  if (!IsDigit(Look))
    Expected("Integer");
  while (IsDigit(Look)) {
    *token_buf = Look;
    token_buf++;
    GetChar();
  }
  *token_buf = '\0';
  Token = '#';
  SkipWhite();
}

int IsMulop(char c) { return strchr("*/", c) != 0; }
int IsAddop(char c) { return strchr("+-", c) != 0; }

void Emit(char *s) { printf("\t%s", s); }

void EmitLn(char *s) {
  Emit(s);
  printf("\n");
}

int IsBoolean(char c) {
  char uc = UpCase(c);
  return strchr("TF", uc) != 0;
}

int IsOp(char c) { return strchr("+-*/<>:=", c) != 0; }
int IsOrOp(char c) { return strchr("|~", c) != 0; }
int IsRelop(char c) { return strchr("=#<>", c) != 0; }

int Lookup(TabPtr T, char *s, int n) {
  int found = 0;
  int i = n - 1;

  while (i >= 0 && !found) {
    if (strncmp(s, T[i], SYMBOL_LEN) == 0) {
      found = ~0;
    } else {
      i--;
    }
  }

  return i;
}

int GetBoolean() {
  char c = UpCase(Look);

  if (!IsBoolean(Look)) {
    Expected("Boolean Literal");
  }
  GetChar();
  return (c == 'T');
}

void SkipComma() {
  SkipWhite();
  if (Look == ',') {
    GetChar();
    SkipWhite();
  }
}

void Init() {
  LCount = 0;
  GetChar();
}
