// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>

void Halt() { exit(1); }

void GetChar() { Look = getchar(); }

void Error(char *s) { printf("\nError: %s.", s); }

void Abort(char *s) {
  Error(s);
  Halt();
}

void Expected(char *s) {
  sprintf(tmp, "%s Expected", s);
  Abort(tmp);
}

void Match(char x) {
  if (Look == x) {
    GetChar();
  } else {
    sprintf(tmp, "' %c ' ", x);
    Expected(tmp);
  }
}

int isAlpha(char c) {
  char uc = UpCase(c);
  return (uc >= 'A' && uc <= 'Z');
}

int IsDigit(char c) { return (c >= '0' && c <= '9'); }

char GetName() {
  char c;
  if (!isAlpha(Look))
    Expected("Name");
  c = UpCase(Look);
  GetChar();
  return c;
}

char GetNum() {
  char c;
  if (!IsDigit(Look))
    Expected("Integer");
  c = Look;
  GetChar();
  return c;
}

void Emit(char *s) { printf("\t%s", s); }

void EmitLn(char *s) {
  Emit(s);
  printf("\n");
}

void Init() { GetChar(); }
