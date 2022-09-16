// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#ifndef _CRADLE_H
#define _CRADLE_H
#define MAX_TOKEN_LEN 100
#define MAX_SPRINTF_LEN 255
#define UpCase(c) (~(1 << 5) & (c))

char tmp[MAX_SPRINTF_LEN];
char token_buf[MAX_TOKEN_LEN];
char Look;

void Abort(char *s);
void Emit(char *s);
void EmitLn(char *s);
void Error(char *s);
void Expected(char *s);
void GetChar();
char *GetName();
char *GetNum();
void Halt();
void Init();
int IsAddop(char c);
int IsAlNum(char c);
int IsAlpha(char c);
int IsDigit(char c);
int IsMulop(char c);
int IsWhite(char c);
void Match(char x);
void SkipWhite();
#endif // _CRADLE_H
