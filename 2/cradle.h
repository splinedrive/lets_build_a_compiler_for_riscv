// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#ifndef _CRADLE_H
#define _CRADLE_H
#define MAX_BUF 255
#define UpCase(c) (~(1 << 5) & (c))

static char tmp[MAX_BUF];
char Look;

void GetChar();
void Error(char *s);
void Abort(char *s);
void Expected(char *s);
void Match(char x);
int IsAlpha(char c);
int IsDigit(char c);
char GetName();
char GetNum();
int IsMulop(char c);
int IsAddop(char c);
void Emit(char *s);
void EmitLn(char *s);
void Init();

#endif // _CRADLE_H
