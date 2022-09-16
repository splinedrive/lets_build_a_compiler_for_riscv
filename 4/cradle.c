// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void Match(char x) {
  if (Look == x) {
    GetChar();
  } else {
    snprintf(tmp, MAX_SPRINTF_LEN, "' %c ' ", x);
    Expected(tmp);
  }
  SkipWhite();
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
  SkipWhite();
  return c;
}

int GetNum() {
  int value = 0;
  if (!IsDigit(Look))
    Expected("Integer");
  while (IsDigit(Look)) {
    value = value * 10 + (Look - '0');
    GetChar();
  }
  return value;
}

int IsMulop(char c) { return strchr("*/", c) != 0; }
int IsAddop(char c) { return strchr("+-", c) != 0; }

void Emit(char *s) { printf("\t%s", s); }

void EmitLn(char *s) {
  Emit(s);
  printf("\n");
}

void NewLine() {
  if (Look == '\r') {
    GetChar();
    if (Look == '\n') {
      GetChar();
    }
  } else if (Look == '\n') {
    GetChar();
  }
}

void InitTable() {
  int i;
  for (i = 'A'; i <= 'Z'; i++) {
    Table[i] = 0;
  }
}
void Init() {
  InitTable();
  GetChar();
}
