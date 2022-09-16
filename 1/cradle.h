// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#ifndef _CRADLE_H
#define _CRADLE_H
#define MAX_BUF 255

char tmp[MAX_BUF];
#define UpCase(c) (~(1 << 5) & (c))
char Look;
void GetChar();
void Error(char *s);
void Abort(char *s);
void Expected(char *s);
void Match(char x);
int isAlpha(char c);
int IsDigit(char c);
char GetName();
char GetNum();
void Emit(char *s);
void EmitLn(char *s);
void Init();

#endif // _CRADLE_H
