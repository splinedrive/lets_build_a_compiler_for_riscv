// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char labelName[MAX_LABEL_LEN];
static int LCount = 0;

void Halt() {
  printf("\n");
  // for (;;);
  exit(1);
}

void GetChar() { Look = getchar(); }

void Error(char *s) { printf("\nError: %s.", s); }

void Abort(char *s) {
  Error(s);
  Halt();
}

void Expected(char *s) {
  snprintf(tmp, MAX_SPRINTF_LEN, "%s Expected", s);
  Abort(tmp);
}

char *NewLabel() {
  snprintf(labelName, MAX_SPRINTF_LEN, "L%02d", LCount);
  LCount++;
  return labelName;
}

void PostLabel(char *L) { printf("%s:\n", L); }

void Match(char x) {
  if (Look == x) {
    GetChar();
    SkipWhite();
  } else {
    snprintf(tmp, MAX_SPRINTF_LEN, "' %c ' ", x);
    Expected(tmp);
  }
}

int IsAlpha(char c) {
  char uc = UpCase(c);
  return (uc >= 'A' && uc <= 'Z');
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

char GetName() {
  char c = UpCase(Look);
  if (!IsAlpha(Look)) {
    Expected("Name");
  }

  GetChar();
  return c;
}

char *GetNum() {
  char *value = token_buf;
  if (!IsDigit(Look))
    Expected("Integer");
  while (IsDigit(Look)) {
    *value = Look;
    value++;
    GetChar();
  }
  SkipWhite();
  *value = '\0';
  return token_buf;
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

int IsOrOp(char c) { return strchr("|~", c) != 0; }
int IsRelop(char c) { return strchr("=#<>", c) != 0; }

int GetBoolean() {
  char c = UpCase(Look);

  if (!IsBoolean(Look))
    Expected("Boolean Literal");
  GetChar();
  return (c == 'T');
}

void Init() {
  GetChar();
  //  SkipWhite();
}
