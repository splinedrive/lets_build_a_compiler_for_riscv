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

Symbol KWList[] = {{"IF"},    {"ELSE"}, {"ENDIF"}, {"WHILE"}, {"ENDWHILE"},
                   {"BREAK"}, {"READ"}, {"WRITE"}, {"VAR"},   {"END"}};

const char KWCode[] = "xilewebRWve";

const int NKW = sizeof(KWList) / sizeof(Symbol);
const int NKW1 = sizeof(KWCode) / sizeof(char);

int IsWhite(char c);

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

void CheckTable(Symbol N) {
  if (!InTable(N)) {
    Undefined(N);
  }
}

void CheckDup(Symbol N) {
  if (InTable(N)) {
    Duplicate(N);
  }
}

void AddEndtry(Symbol N, char T) {
  //  CheckDup(N);
  if (NEntry == MaxEntry) {
    Abort("Symbol Table Full");
  }
  strncpy(ST[NEntry], N, SYMBOL_LEN);
  SType[NEntry] = T;

  NEntry++;
}

void Scan() {
  if (Token == 'x') {
    Token = KWCode[Lookup(&KWList[0], Value, NKW) + 1];
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

void CheckIdent() {
  if (Token != 'x') {
    Expected("Identifier");
  }
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
  Next();
}

void Semi() {
  if (Token == ';') {
    Next();
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

  SkipWhite();
  if (!IsAlpha(Look)) {
    Expected("Identifier");
  }
  Token = 'x';
  *token_buf = 0;

  do {
    *token_buf = UpCase(Look);
    token_buf++;
    GetChar();
  } while (IsAlNum(Look));
  *token_buf = '\0';
}

void GetNum() {
  token_buf = Value;
  SkipWhite();
  if (!IsDigit(Look)) {
    Expected("Number");
  }
  Token = '#';
  *token_buf = 0;
  do {
    *token_buf = Look;
    token_buf++;
    GetChar();
  } while (IsDigit(Look));
  *token_buf = 0;
}

void GetOp() {
  SkipWhite();
  token_buf = Value;
  *token_buf = Look;
  token_buf++;
  Token = Look;
  *token_buf = 0;
  GetChar();
  /*
  token_buf = Value;
  Token = Look;
  *token_buf = 0;
  do {
    *token_buf = Look;
    token_buf++;
    GetChar();
  } while (!(IsAlpha(Look) || IsDigit(Look) || IsWhite(Look)));
  *token_buf = 0;
  */
}

void Next() {
  SkipWhite();
  if (IsAlpha(Look)) {
    GetName();
  } else if (IsDigit(Look)) {
    GetNum();
  } else {
    GetOp();
  }
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
    if (Look == '{') {
      SkipComment();
    } else {
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

void Init() {
  for (int i = 0; i <= MaxEntry; i++) {
    strncpy(ST[i], " ", SYMBOL_LEN);
  }

  GetChar();
  Next();
}
