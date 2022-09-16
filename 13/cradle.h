// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#ifndef _CRADLE_H
#define _CRADLE_H
#define DEBUG
#define MAX_TOKEN_LEN 100
#define MAX_MSG_LEN 100
#define MAX_SPRINTF_LEN 255
#define MAX_LABEL_LEN 25
#define MaxEntry 100

#define UART_WRITE "0x30000000"
#define UART_READ "0x30000000"

#define UpCase(c) (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c
#define SYMBOL_LEN 8
char tmp[MAX_SPRINTF_LEN];

extern char Look;
extern char ST['Z' - 'A'];
extern int Params['Z' - 'A'];
extern int NumParams;

// int InTable(Symbol n);
int InTable(char n);
void NewLine();
void Scan();
void GetChar();
void Error(char *s);
void Abort(char *s);
void Expected(char *s);
void MatchString(char *x);
void Undefined(char *x);
void Duplicate(char *x);
void CheckIdent();
void Match(char x);
void Semi();
int IsAlNum(char c);
int IsMulop(char c);
int IsAddop(char c);
int IsOrop(char c);
int IsRelop(char c);
int IsAlpha(char c);
int IsDigit(char c);
char GetName();
char GetNum();
void GetOp();
void Next();
void SkipWhite();
void Emit(char *s);
void EmitLn(char *s);
void Init();
char *NewLabel();
void PostLabel(char *L);
// void AddEndtry(Symbol N, char T);
void AddEndtry(char N, char T);
char TypeOf(char n);
void Fin();
void ClearParams();
int IsParam(char N);
int ParamNumber(char N);
void AddParam(char Name);

#endif // _CRADLE_H
