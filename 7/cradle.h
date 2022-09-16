// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#ifndef _CRADLE_H
#define _CRADLE_H
#define MAX_TOKEN_LEN 100
#define MAX_SPRINTF_LEN 255
#define MAX_LABEL_LEN 25
#define UpCase(c) (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c
//#define UpCase(c) c
#define SYMBOL_LEN 8
typedef char Symbol[SYMBOL_LEN];

typedef Symbol SymTab[1000];
typedef SymTab TabPtr;

/*
typedef enum {
  IfSym,
  ElseSym,
  EndifSym,
  EndSym,
  IdentSym,
  NumberSym,
  OperatorSym
} SymType;
*/

extern char Token;

extern char *token_buf;
extern char Value[MAX_TOKEN_LEN];
extern char Look;

void Abort(char *s);
void Emit(char *s);
void EmitLn(char *s);
void Error(char *s);
void Expected(char *s);
void GetChar();
void GetOp();
void GetName();
void GetNum();
void Halt();
void Init();
int IsAddop(char c);
int IsAlNum(char c);
int IsAlpha(char c);
int IsDigit(char c);
int IsMulop(char c);
int IsWhite(char c);
void Match(char x);
void MatchString(char *x);
void SkipWhite();
char *NewLabel();
void PostLabel(char *L);
int IsBoolean(char c);
int IsOp(char c);
int IsOrOp(char c);
int IsRelop(char c);
int GetBoolean();
void SkipComma();
int Lookup(TabPtr T, char *s, int n);
void Scan();
#endif // _CRADLE_H
