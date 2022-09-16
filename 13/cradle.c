// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char labelName[MAX_LABEL_LEN];
static int LCount = 0;
char ST['Z' - 'A'];

char Value[MAX_TOKEN_LEN];
char Look;

int Params['Z' - 'A'];
int NumParams;

int IsWhite(char c);

int InTable(char n) { return ST[n - 'A'] != ' '; }

void AddEndtry(char Name, char T) {
  if (InTable(Name)) {
    sprintf(tmp, "%c", Name);
    Duplicate(tmp);
  }
  ST[Name - 'A'] = T;
}

void CheckVar(char Name) {
  if (!InTable(Name)) {
    sprintf(tmp, "%c", Name);
    Undefined(tmp);
  }
  if (TypeOf(Name) != 'v') {
    sprintf(tmp, "%c is not a variable", Name);
    Abort(tmp);
  }
}

char *NewLabel() {
  snprintf(labelName, MAX_SPRINTF_LEN, "L%02d", LCount);
  LCount++;
  return labelName;
}

void PostLabel(char *L) { printf("%s:\n", L); }

void Halt() {
  printf("\n");
#ifdef DEBUG
  for (;;)
    ;
#else
  exit(1);
#endif
}

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

void Undefined(char *s) {
  sprintf(tmp, "Undefined Identifier %s", s);
  Abort(tmp);
}

void Duplicate(char *s) {
  sprintf(tmp, "Duplicate Identifier %s", s);
  Abort(tmp);
}

char TypeOf(char n) {
  char res;
  if (IsParam(n)) {
    res = 'f';
  } else {
    res = ST[n - 'A'];
  }
  return res;
}

void Match(char x) {
  NewLine();
  if (Look == x) {
    GetChar();
  } else {
    sprintf(tmp, "' %c ' ", x);
    Expected(tmp);
  }
  SkipWhite();
}

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

void Emit(char *s) { printf("\t%s", s); }

void EmitLn(char *s) {
  Emit(s);
  printf("\n");
}

void SkipComment() {
  while (Look != '}') {
    GetChar();
    if (Look == '{') {
      SkipComment();
    }
  }
  GetChar();
}

// \f\n\r\t\v
int IsWhite(char c) { return strchr(" \t\r\n{", c) != 0; }

void SkipWhite() {
  while (IsWhite(Look)) {
    GetChar();
  }
}

void Fin() {
  if (Look == '\n') {
    GetChar();
    if (Look == '\r') {
      GetChar();
    }
  }
}

void NewLine() {
  while (Look == '\r') {
    GetChar();
    if (Look == '\n') {
      GetChar();
    }
    SkipWhite();
  }
}

void ClearParams() {
  for (char i = 'A'; i <= 'Z'; i++) {
    Params[i - 'A'] = 0;
  }
  NumParams = 0;
}

int ParamNumber(char N) { return Params[N - 'A']; }

int IsParam(char N) { return ParamNumber(N) != 0; }

void AddParam(char Name) {
  if (IsParam(Name)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "%c", Name);
    Duplicate(tmp);
  }
  NumParams++;
  Params[Name - 'A'] = NumParams;
}

void Init() {
  GetChar();
  SkipWhite();
  for (char i = 'A'; i < 'Z'; i++) {
    ST[i - 'A'] = ' ';
  }
  ClearParams();
}
