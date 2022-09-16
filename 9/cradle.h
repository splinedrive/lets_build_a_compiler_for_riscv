// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#ifndef _CRADLE_H
#define _CRADLE_H
#define MAX_TOKEN_LEN 100
#define MAX_SPRINTF_LEN 255
#define MAX_LABEL_LEN 25

char tmp[MAX_SPRINTF_LEN];
char token_buf[MAX_TOKEN_LEN];
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
char *NewLabel();
void PostLabel(char L);

#endif // _CRADLE_H
