// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Base;

void Expression();
void BoolExpression();
void Block();
void DoProc();
void DoMain();
// void ParamList();
int ParamList();
int LocDecls();
int LocDecls();

#define COMMENT_SEMANTIC                                                       \
  snprintf(tmp, MAX_SPRINTF_LEN, "\n# %s", __func__);                          \
  EmitLn(tmp)

void Header() {
  EmitLn(".global _start");
  EmitLn(".section .data");
}

void Prolog() {
  EmitLn(".section .text");
  PostLabel("_start");
}

void Epilog() { EmitLn("ret"); /*EmitLn(".section .data");*/ }

void Clear() { EmitLn("li t0,0"); }

// void Negate() { EmitLn("negw t0,t0"); }

void Allocate(char *Name, char *Val) {
  snprintf(tmp, MAX_SPRINTF_LEN, "%s:\t .word %s", Name, Val);
  EmitLn(tmp);
}

void LoadParam(int N) {
  COMMENT_SEMANTIC;
#if 0
  int offset = 4*(NumParams - N);
  snprintf(tmp, MAX_SPRINTF_LEN, "la a0,%d(sp)", offset); // by ref
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "lw t0,0(a0)");
#else
  int offset = 4 * (N - Base);
  snprintf(tmp, MAX_SPRINTF_LEN, "lw t0,%d(sp)", offset); // by val
#endif
  EmitLn(tmp);
}

void StoreParam(int N) {
  COMMENT_SEMANTIC;
#if 0
  int offset = 4*(NumParams - N);
  snprintf(tmp, MAX_SPRINTF_LEN, "la a0,%d(sp)", offset); // by ref
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(a0)");
#else
  int offset = 4 * (N - Base);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,%d(sp)", offset);
#endif
  EmitLn(tmp);
}

void LoadConst(char *n) {
  COMMENT_SEMANTIC;
  snprintf(tmp, MAX_SPRINTF_LEN, "li t0,%s", n);
  EmitLn(tmp);
}

void LoadVar(char Name) {
  COMMENT_SEMANTIC;
  if (!InTable(Name)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "%c", Name);
    Undefined(tmp);
  }
  snprintf(tmp, MAX_SPRINTF_LEN, "lw t0,%c", Name);
  EmitLn(tmp);
}

void Push() {
  COMMENT_SEMANTIC;
  EmitLn("addi sp,sp,-4");
  EmitLn("sw t0,0(sp)");
}

void PopAdd() {
  COMMENT_SEMANTIC;
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("add t0,t1,t0");
}

void PopSub() {
  COMMENT_SEMANTIC;
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("sub t0,t1,t0");
  //  EmitLn("negw t0,t0");
}

void PopMul() {
  COMMENT_SEMANTIC;
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("mul t0,t1,t0");
}

void PopDiv() {
  COMMENT_SEMANTIC;
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("div t0,t1,t0");
}

void Return() {
  COMMENT_SEMANTIC;
  EmitLn("ret");
}

void Call(char N) {
  snprintf(tmp, MAX_SPRINTF_LEN, "call %c", N);
  EmitLn(tmp);
}

void CleanStack(int N) {
  if (N > 0) {
    snprintf(tmp, MAX_SPRINTF_LEN, "addi sp,sp,%d", N);
    EmitLn(tmp);
  }
}

void CallProc(char Name) {
  COMMENT_SEMANTIC;
  int N = ParamList();
  Call(Name);
  CleanStack(N);
}

void StoreVar(char Name) {
  if (!InTable(Name)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "%c", Name);
    Undefined(tmp);
  }

  COMMENT_SEMANTIC;
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%c", Name);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);
}

void Expression() {
  char Name = GetName();
  if (IsParam(Name)) {
    LoadParam(ParamNumber(Name));
  } else {
    LoadVar(Name);
  }
}

void Branch(char *L) {
  COMMENT_SEMANTIC;
  snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L);
  EmitLn(tmp);
}

void BranchFalse(char *L) {
  COMMENT_SEMANTIC;
  snprintf(tmp, MAX_SPRINTF_LEN, "beq t0,x0,%s", L);
  EmitLn(tmp);
}

void NotIt() {
  COMMENT_SEMANTIC;
  EmitLn("not t0,t0");
}

void PopAnd() {
  COMMENT_SEMANTIC;
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("and t0,t1,t0");
}

void PopOr() {
  COMMENT_SEMANTIC;
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("or t0,t1,t0");
}

void PopXor() {
  COMMENT_SEMANTIC;
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("xor t0,t1,t0");
}

void Assignment(char Name) {
  Match('=');
  Expression();
  if (IsParam(Name)) {
    StoreParam(ParamNumber(Name));
  } else {
    StoreVar(Name);
  }
}

void AssignmentOrProc() {
  char Name;
  Name = GetName();

  switch (TypeOf(Name)) {
  case ' ':
    snprintf(tmp, MAX_SPRINTF_LEN, "%c", Name);
    Undefined(tmp);
    break;
  case 'v':
  case 'f':
    Assignment(Name);
    break;
  case 'p':
    CallProc(Name);
    break;
  default:
    snprintf(tmp, MAX_SPRINTF_LEN, "Identifier %c Cannot Be Used Here", Name);
    Abort(tmp);
    break;
  }
}

void DoBlock() {
  while (!strchr("e", Look)) {
    AssignmentOrProc();
    Fin();
  }
}

void Alloc(char N) {
  if (InTable(N)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "%c", N);
    Duplicate(tmp);
  }
  ST[N - 'A'] = 'v';
  snprintf(tmp, MAX_SPRINTF_LEN, "%c:\t .word 0", N);
  EmitLn(tmp);
}

void LocDecl() {
  Match('v');
  AddParam(GetName());
  Fin();
}

int LocDecls() {
  int n = 0;
  while (Look == 'v') {
    LocDecl();
    n++;
  }
  return n;
}

void Decl() {
  Match('v');
  Alloc(GetName());
}

void TopDecls() {
  while (Look != '.') {
    switch (Look) {
    case 'v':
      Decl();
      break;
    case 'p':
      DoProc();
      break;
    case 'P':
      DoMain();
      break;
    default:
      snprintf(tmp, MAX_SPRINTF_LEN, "Unrecognized Keyword %c", Look);
      Abort(tmp);
      break;
    }
    Fin();
  }
}

void BeginBlock() {
  Match('b');
  Fin();
  DoBlock();
  Match('e');
  Fin();
}

/*
void Param() {
  // return GetName();
  COMMENT_SEMANTIC;
  Expression();
  Push();
}
*/

void Param() {
  COMMENT_SEMANTIC;
  char N = GetName();
  snprintf(tmp, MAX_SPRINTF_LEN, "la t0,%c", N);
  EmitLn(tmp);
  EmitLn("addi sp,sp,-4");
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(sp)");
  EmitLn(tmp);
}

#if 0
void ParamList() {
  Match('(');
  if (Look != ')') {
    Param();
    while (Look == ',') {
      Match(',');
      Param();
    }
  }

  Match(')');
}
#else
int ParamList() {
  int N = 0;
  Match('(');
  if (Look != ')') {
    Param();
    N++;
    while (Look == ',') {
      Match(',');
      Param();
      N++;
    }
  }
  Match(')');
  return 4 * N;
}
#endif

void FormalParam() {
  // return GetName();
  AddParam(GetName());
}

void FormalList() {
  Match('(');
  if (Look != ')') {
    FormalParam();
    while (Look == ',') {
      Match(',');
      FormalParam();
    }
  }
  Match(')');
  Fin();
  Base = NumParams;
  // NumParams += 4;
}

void ProcProlog(char N, int k) {
  snprintf(tmp, MAX_SPRINTF_LEN, "%c", N);
  PostLabel(tmp);
  int offset = -4 * k;
  snprintf(tmp, MAX_SPRINTF_LEN, "addi sp,sp,%d", offset);
  EmitLn(tmp);
}

void ProcEpilog(char N, int k) {
  int offset = 4 * k;
  snprintf(tmp, MAX_SPRINTF_LEN, "addi sp,sp,%d", offset);
  EmitLn(tmp);
  Return();
}

void DoProc() {
  char N;
  int k;
  Match('p');
  N = GetName();

  if (InTable(N)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "%c", N);
    Duplicate(tmp);
  }

  snprintf(tmp, MAX_SPRINTF_LEN, "%c", N);
  ST[N - 'A'] = 'p';
  FormalList();
  k = LocDecls(); // howmany vars
  ProcProlog(N, k);
  BeginBlock();
  ProcEpilog(N, k);
  ClearParams();
}

void DoMain() {
  char N;
  Match('P');
  N = GetName();
  Fin();
  if (InTable(N)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "%c", N);
    Duplicate(tmp);
  }
  // ST[N - 'A'] = 'P';
  Prolog();
  BeginBlock();
}

int main() {
  Init();
  TopDecls();
  Epilog();
  return 0;
}
