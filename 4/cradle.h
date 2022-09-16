// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#ifndef _CRADLE_H
#define _CRADLE_H
#define MAX_TOKEN_LEN 100
#define MAX_SPRINTF_LEN 255
#define UpCase(c) (~(1 << 5) & (c))

#define TABLE_SIZE ('Z' - 'A')
char tmp[MAX_SPRINTF_LEN];
char token_buf[MAX_TOKEN_LEN];

char Look;
int Table[TABLE_SIZE];

void Abort(char *s);
void Emit(char *s);
void EmitLn(char *s);
void Error(char *s);
void Expected(char *s);
void GetChar();
char GetName();
int GetNum();
void Halt();
void Init();
int IsAddop(char c);
int IsAlNum(char c);
int IsAlpha(char c);
int IsDigit(char c);
int IsMulop(char c);
int IsWhite(char c);
void Match(char x);
void NewLine();
void SkipWhite();
#endif // _CRADLE_H
