// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char Factor();
void Push(char Size);
char Multiply(char T1);
char Divide(char T1);

void Clear() { EmitLn("li t0,0"); }

void AllocVar(char N, char T) {
  snprintf(tmp, MAX_SPRINTF_LEN, "%c:\t .%s 0", N,
           UpCase(T) == 'B' ? "byte" : UpCase(T) == 'W' ? "short" : "long");
  EmitLn(tmp);
}

void Alloc(char N, char T) {
  AddEntry(N, T);
  AllocVar(N, T);
}

void Decl() {
  char Typ = GetName();
  Alloc(GetName(), Typ);
}

void TopDecls() {
  while (Look != 'B') {
    switch (Look) {
    case 'b':
    case 'w':
    case 'l':
      Decl();
      break;
    default:
      snprintf(tmp, MAX_SPRINTF_LEN, "Unrecognized Keyword %c", Look);
      break;
    }
    Fin();
  }
}

void LOAD(char Size, char *Source, char *Dest) {
  snprintf(tmp, MAX_SPRINTF_LEN, "%s %s,%s",
           (UpCase(Size) == 'B' ? "lb" : UpCase(Size) == 'W' ? "lh" : "lw"),
           Dest, Source);
  EmitLn(tmp);
}

void LoadVar(char Name, char Typ) {
  char Source[MAX_MSG_LEN];
  snprintf(tmp, MAX_SPRINTF_LEN, "%c", Name);
  strncpy(Source, tmp, MAX_MSG_LEN);
  // Move(Typ, Source, "t0");
  LOAD(Typ, Source, "t0");
}

void Convert(char Source, char Dest, char *Reg) {
  if (Source != Dest) {
    if (Source == 'B') {
      snprintf(tmp, MAX_SPRINTF_LEN, "andi %s,%s,0xff", Reg, Reg);
      EmitLn(tmp);
    }
    if (Dest == 'L') {
      // treated signed extended by signed load instruction
    }
  }
}

char Promote(char T1, char T2, char *Reg) {
  char Typ;
  Typ = T1;
  if (T1 != T2) {
    if ((T1 == 'B') || ((T1 == 'W') && (T2 == 'L'))) {
      Convert(T1, T2, Reg);
      Typ = T2;
    }
  }
  return Typ;
}

char SameType(char T1, char T2) {
  T1 = Promote(T1, T2, "t1");
  return Promote(T2, T1, "t0");
}

char VarType(char Name) {
  char Typ = TypeOf(Name);
  if (!IsVarType(Typ)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "Identifier %c is not a variable", Name);
    EmitLn(tmp);
  }
  return Typ;
}

void LoadConst(int N, char Typ __attribute__((unused))) {
  snprintf(tmp, MAX_SPRINTF_LEN, "li t0,%d", N);
  EmitLn(tmp);
}

char LoadNum(int N) {
  char Typ;
  if (abs(N) <= 127) {
    Typ = 'B';
  } else if (abs(N) <= 32767) {
    Typ = 'W';
  } else {
    Typ = 'L';
  }
  LoadConst(N, Typ);
  return Typ;
}

char Load(char Name) {
  char Typ = VarType(Name);
  LoadVar(Name, Typ);
  return Typ;
}

void StoreVar(char Name, char Typ) {
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%c", Name);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "%s t0,0(t1)",
           (UpCase(Typ) == 'B' ? "sb" : UpCase(Typ) == 'W' ? "sh" : "sw"));
  EmitLn(tmp);
}

void Store(char Name, char T1) {
  char T2;
  T2 = VarType(Name);
  Convert(T1, T2, "t0");
  StoreVar(Name, T2);
}

char Term() {
  char Typ = Factor();

  while (IsMulop(Look)) {
    Push(Typ);

    switch (Look) {
    case '*':
      Typ = Multiply(Typ);
      break;
    case '/':
      Typ = Divide(Typ);
      break;
    }
  }

  return Typ;
}

char Unop() {
  Clear();
  return 'W';
}

void Push(char Size) {
  Size = UpCase(Size);
  if (Size == 'B') {
    EmitLn("addi sp,sp,-1");
    EmitLn("sb t0,0(sp)");
  } else if (Size == 'W') {
    EmitLn("addi sp,sp,-2");
    EmitLn("sh t0,0(sp)");
  } else {
    EmitLn("addi sp,sp,-4");
    EmitLn("sw t0,0(sp)");
  }
}

void Pop(char Size) {
  Size = UpCase(Size);
  if (Size == 'B') {
    EmitLn("addi sp,sp,1");
    EmitLn("lb t1,-1(sp)");
  } else if (Size == 'W') {
    EmitLn("addi sp,sp,2");
    EmitLn("lh t1,-2(sp)");
  } else {
    EmitLn("addi sp,sp,4");
    EmitLn("lw t1,-4(sp)");
  }
}

void GenAdd(char Size) { EmitLn("add t0,t1,t0"); }

void GenSub(char Size) { EmitLn("sub t0,t1,t0"); }

void GenLongMult() { EmitLn("mul t0,t1,t0"); }
void GenMult() { EmitLn("mul t0,t1,t0"); }

void GenDiv() { EmitLn("div t0,t1,t0"); }
void GenLongDiv() { EmitLn("div t0,t1,t0"); }

char PopMul(char T1, char T2) {
  char T;
  Pop(T1);
  T = SameType(T1, T2);
  Convert(T, 'W', "t1");
  if (T == 'L') {
    GenLongMult();
  } else {
    GenMult();
  }
  if (T == 'B') {
    T = 'W';
  } else {
    T = 'L';
  }

  return T;
}

char PopDiv(char T1, char T2) {
  char T;
  Pop(T1);
  Convert(T1, 'L', "t1");
  if (T1 == 'L' || T2 == 'L') {
    Convert(T2, 'L', "t0");
    GenLongDiv();
    T = 'L';
  } else {
    Convert(T2, 'W', "t0");
    GenDiv();
    T = T1;
  }

  return T;
}

char PopAdd(char T1, char T2) {
  Pop(T1);
  T2 = SameType(T1, T2);
  GenAdd(T2);
  return T2;
}

char PopSub(char T1, char T2) {
  Pop(T1);
  T2 = SameType(T1, T2);
  GenSub(T2);
  return T2;
}

char Add(char T1) {
  Match('+');
  return PopAdd(T1, Term());
}

char Substract(char T1) {
  Match('-');
  return PopSub(T1, Term());
}

char Expression() {
  char Typ;
  if (IsAddop(Look)) {
    Typ = Unop();
  } else {
    Typ = Term();
  }

  while (IsAddop(Look)) {
    Push(Typ);
    switch (Look) {
    case '+':
      Typ = Add(Typ);
      break;
    case '-':
      Typ = Substract(Typ);
      break;
    }
  }
  return Typ;
}

char Factor() {
  char res;
  if (Look == '(') {
    Match('(');
    res = Expression();
    Match(')');
  } else if (IsAlpha(Look)) {
    res = Load(GetName());
  } else {
    res = LoadNum(GetNum());
  }

  return res;
}

char Multiply(char T1) {
  Match('*');
  return PopMul(T1, Factor());
}

char Divide(char T1) {
  Match('/');
  return PopDiv(T1, Factor());
}

void Assignment() {
  char Name = GetName();
  Match('=');
  Store(Name, Expression());
}

void Block() {
  while (Look != '.') {
    Assignment();
    Fin();
  }
}

int main() {
  Init();
#if 0
  AddEntry('A', 'b');
  AddEntry('B', 'w');
  AddEntry('C', 'l');
  Load('A');
  Load('B');
  Load('C');
  Store('A');
  Store('B');
  Store('C');
#endif

  TopDecls();
  Match('B');
  Fin();
  Block();
  DumpTable();
  return 0;
}
