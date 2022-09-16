// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#ifndef _CRADLE_H
#define _CRADLE_H
#define DEBUG
#define MAX_TOKEN_LEN 100
#define MAX_MSG_LEN 100
#define MAX_SPRINTF_LEN 255
#define MAX_LABEL_LEN 25
#define MaxEntry 100
#define UpCase(c) ((c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c)
char tmp[MAX_SPRINTF_LEN];
extern char Look;
extern char ST['Z' - 'A'];

void Init();
void DumpTable();
void AddEntry(char N, char T);
void EmitLn(char *s);
void Match(char x);
char GetName();
int GetNum();
void Fin();
void SkipWhite();
void GetChar();
int IsVarType(char c);
int IsAlpha(char c);
int IsAddop(char c);
int IsMulop(char c);
char TypeOf(char N);
#endif // _CRADLE_H
