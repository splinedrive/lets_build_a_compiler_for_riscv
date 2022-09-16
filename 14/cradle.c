// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB '\t'
#define CR '\r'
#define LF '\n'

char Look;
char ST['Z' - 'A'];
#define GET_ST(i) ST[i - 'A']

void GetChar() { Look = getchar(); }

// \f\n\r\t\v
int IsWhite(char c) { return strchr(" \t\r\n{", c) != 0; }

void SkipWhite() {
  while (IsWhite(Look)) {
    GetChar();
  }
}

void Halt() {
  printf("\n");
#ifdef DEBUG
  for (;;)
    ;
#else
  exit(1);
#endif
}

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
  SkipWhite();
}
void Fin() {
  if (Look == '\n') {
    GetChar();
    if (Look == '\r') {
      GetChar();
    }
  }
}

int IsVarType(char c) { return strchr("BWL", c) != 0; }
int IsOrop(char c) { return strchr("|~", c) != 0; }

int IsRelop(char c) { return strchr("=#<>", c) != 0; }

int IsMulop(char c) { return (c == '*' || c == '/'); }

int IsAddop(char c) { return (c == '+' || c == '-'); }

int IsAlpha(char c) {
  char uc = UpCase(c);
  return (uc >= 'A' && uc <= 'Z');
}

int IsDigit(char c) { return (c >= '0' && c <= '9'); }

int IsAlNum(char c) { return IsDigit(c) || IsAlpha(c); }

char GetName() {
  char Name;
  if (!IsAlpha(Look)) {
    Expected("Name");
  }

  Name = UpCase(Look);
  GetChar();
  SkipWhite();
  return Name;
}

int GetNum() {
  int Val = 0;
  if (!IsDigit(Look)) {
    Expected("Integer");
  }

  while (IsDigit(Look)) {
    Val += Val * 10 + (Look - '0');
    GetChar();
  }
  SkipWhite();
  return Val;
}

/*
char GetNum() {
  char Name;
  if (!IsDigit(Look)) {
    Expected("Integer");
  }
  Name = Look;
  GetChar();
  SkipWhite();
  return Name;
}
*/

void Emit(char *s) { printf("\t%s", s); }

void EmitLn(char *s) {
  Emit(s);
  printf("\n");
}

char TypeOf(char N) { return GET_ST(N); }

int InTable(char N) { return TypeOf(N) != '?'; }

void CheckDup(char N) {
  if (InTable(N)) {
    sprintf(tmp, "Duplicate Name %c", N);
    Abort(tmp);
  }
}

void AddEntry(char N, char T) {
  CheckDup(N);
  GET_ST(N) = T;
}

void DumpTable() {
  for (char i = 'A'; i < 'Z'; i++) {
    if (GET_ST(i) != '?')
      printf(" %c\n", GET_ST(i));
  }
}

void Init() {
  for (char i = 'A'; i < 'Z'; i++) {
    GET_ST(i) = '?';
  }
  GetChar();
  SkipWhite();
}
