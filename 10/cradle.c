// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char labelName[MAX_LABEL_LEN];
static int LCount = 0;
static int NEntry = 0;
Symbol ST[MaxEntry];
char SType[MaxEntry];
char Token;

char *token_buf;
char Value[MAX_TOKEN_LEN];
char Look;

Symbol KWList[] = {{"IF"},       {"ELSE"}, {"ENDIF"},  {"WHILE"},
                   {"ENDWHILE"}, {"READ"}, {"WRITE"},  {"VAR"},
                   {"BEGIN"},    {"END"},  {"PROGRAM"}};

const char KWCode[] = "xileweRWvbep";

const int NKW = sizeof(KWList) / sizeof(Symbol);
const int NKW1 = sizeof(KWCode) / sizeof(char);

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

int Locate(Symbol N) { return (Lookup(ST, N, MaxEntry)); }

int InTable(Symbol n) { return Lookup(ST, n, MaxEntry) >= 0; }

void AddEndtry(Symbol N, char T) {
  if (InTable(N)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "Duplicate Variable Name %s ", N);
    Abort(tmp);
  }
  strncpy(ST[NEntry], N, SYMBOL_LEN);
  SType[NEntry] = T;

  NEntry++;
}

void Scan() {
  GetName();
  Token = KWCode[Lookup(&KWList[0], Value, NKW) + 1];
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

void MatchString(char *x) {
  if (strncmp(Value, x, MAX_TOKEN_LEN) != 0) {
    char tmp[MAX_SPRINTF_LEN];
    snprintf(tmp, MAX_SPRINTF_LEN, "' %s ' ", x);
    Expected(tmp);
  }
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

void GetName() {
  token_buf = Value;

  NewLine();
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

int GetNum() {
  int Val = 0;
  NewLine();
  if (!IsDigit(Look)) {
    Expected("Integer");
  }
  while (IsDigit(Look)) {
    Val = 10 * Val + (Look - '0');
    GetChar();
  }
  SkipWhite();
  return Val;
}

void Emit(char *s) { printf("\t%s", s); }

void EmitLn(char *s) {
  Emit(s);
  printf("\n");
}

// \f\n\r\t\v
int IsWhite(char c) { return strchr(" \t\r\n", c) != 0; }

void SkipWhite() {
  while (IsWhite(Look)) {
    GetChar();
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

void Init() {
  for (int i = 0; i <= MaxEntry; i++) {
    strncpy(ST[i], " ", SYMBOL_LEN);
  }

  GetChar();
  Scan();
}
