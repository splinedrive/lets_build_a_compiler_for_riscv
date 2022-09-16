// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Expression();
void BoolExpression();
void Block();
void Assignment();

void Header() {
  EmitLn(".global _start");
  EmitLn(".section .data");
  // EmitLn("_start:");
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

void LoadConst(char *n) {
  snprintf(tmp, MAX_SPRINTF_LEN, "li t0,%s", n);
  EmitLn(tmp);
}

void LoadVar(char *Name) {
  if (!InTable(Name)) {
    Undefined(Name);
  }
  snprintf(tmp, MAX_SPRINTF_LEN, "lw t0,%s", Name);
  EmitLn(tmp);
}

void Push() {
  EmitLn("addi sp,sp,-4");
  EmitLn("sw t0,0(sp)");
}

void PopAdd() {
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("add t0,t1,t0");
}

void PopSub() {
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("sub t0,t1,t0");
  //  EmitLn("negw t0,t0");
}

void PopMul() {
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("mul t0,t1,t0");
}

void PopDiv() {
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("div t0,t1,t0");
}

void StoreVar(char *Name) {
  if (!InTable(Name)) {
    Undefined(Name);
  }

  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", Name);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);
}

void Factor() {
  if (Token == '(') {
    Next();
    BoolExpression();
    MatchString(")");
  } else {
    if (Token == 'x') {
      LoadVar(Value);
    } else if (Token == '#') {
      LoadConst(Value);
    } else {
      Expected("Math Factor");
    }
    Next();
  }
}

void Multiply() {
  Next();
  Factor();
  PopMul();
}

void Divide() {
  Next();
  Factor();
  PopDiv();
}

void Term() {
  Factor();
  while (IsMulop(Token)) {
    Push();
    switch (Token) {
    case '*':
      Multiply();
      break;
    case '/':
      Divide();
      break;
    }
  }
}

void Add() {
  Next();
  Term();
  PopAdd();
}

void Subtract() {
  Next();
  Term();
  PopSub();
}

void Expression() {
  if (IsAddop(Token)) {
    Clear();
  } else {
    Term();
  }
  while (IsAddop(Token)) {
    Push();
    switch (Token) {
    case '+':
      Add();
      break;
    case '-':
      Subtract();
      break;
    }
  }
}

void NotIt() { EmitLn("not t0,t0"); }

void PopAnd() {
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("and t0,t1,t0");
}

void PopOr() {
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("or t0,t1,t0");
}

void PopXor() {
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("xor t0,t1,t0");
}

void PopCompare() {
  EmitLn("addi sp,sp,4");
  EmitLn("lw t1,-4(sp)");
  EmitLn("sub t0,t1,t0");
}

void ReadIt(char *Name) {
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", UART_READ);
  EmitLn(tmp);
  EmitLn("lw t0,0(t1)");
  StoreVar(Value);
}
void ReadVar() {
  CheckIdent();
  CheckTable(Value);
  ReadIt(Value);
}

void WriteIt() {
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", UART_WRITE);
  EmitLn(tmp);
  EmitLn("sw t0,0(t1)");
}

void DoRead() {
  Next();
  MatchString("(");
  ReadVar();
  while (Token == ',') {
    Next();
    ReadVar();
  }
  MatchString(")");
}

void DoWrite() {
  Next();
  MatchString("(");
  Expression();
  WriteIt();
  while (Token == ',') {
    Next();
    Expression();
    WriteIt();
  }
  MatchString(")");
}

void SetEqual() { EmitLn("seqz t0,t0"); }

void SetNEqual() { EmitLn("snez t0,t0"); }

void SetGreater() { EmitLn("sgtz t0,t0"); }

void SetLess() { EmitLn("sltz t0,t0"); }

void SetLessOrEqual() {
  EmitLn("seqz t1,t0");
  EmitLn("sltz t0,t0");
  EmitLn("or t0,t0,t1");
}

void SetGreaterOrEqual() {
  EmitLn("seqz t1,t0");
  EmitLn("sgtz t0,t0");
  EmitLn("or t0,t0,t1");
}

void CompareExpression() {
  Expression();
  PopCompare();
}

void NextExpression() {
  Next();
  CompareExpression();
}

void Equals() {
  NextExpression();
  SetEqual();
}

void NotEqual() {
  NextExpression();
  SetNEqual();
}

void LessOrEqual() {
  NextExpression();
  SetLessOrEqual();
}

void Less() {
  Next();
  switch (Token) {
  case '=':
    LessOrEqual();
    break;
  case '>':
    NotEqual();
    break;
  default:
    CompareExpression();
    SetLess();
    break;
  }
}

void Greater() {
  Next();
  if (Token == '=') {
    NextExpression();
    SetGreaterOrEqual();
  } else {
    CompareExpression();
    SetGreater();
  }
}

void Relation() {
  Expression();
  if (IsRelop(Token)) {
    Push();
    switch (Token) {
    case '=':
      Equals();
      break;
    case '<':
      Less();
      break;
    case '>':
      Greater();
      break;
    }
  }
}

void NotFactor() {
  if (Token == '!') {
    Next();
    Relation();
    NotIt();
  } else {
    Relation();
  }
}

void BoolTerm() {
  NotFactor();
  while (Token == '&') {
    Push();
    Next();
    NotFactor();
    PopAnd();
  }
}

void BoolOr() {
  Next();
  BoolTerm();
  PopOr();
}

void BoolXor() {
  Next();
  BoolTerm();
  PopXor();
}

void BoolExpression() {
  BoolTerm();
  while (IsOrop(Token)) {
    Push();
    switch (Token) {
    case '|':
      BoolOr();
      break;
    case '~':
      BoolXor();
      break;
    }
  }
}

void Branch(char *L) {
  snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L);
  EmitLn(tmp);
}

void BranchFalse(char *L) {
  snprintf(tmp, MAX_SPRINTF_LEN, "beq t0,x0,%s", L);
  EmitLn(tmp);
}

void DoIf(char *L) {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];

  Next();
  BoolExpression();
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, L1, MAX_LABEL_LEN);
  BranchFalse(L1);
  Block(L);
  if (Token == 'l') {
    Next();
    strncpy(L2, NewLabel(), MAX_LABEL_LEN);
    Branch(L2);
    PostLabel(L1);
    Block(L);
  }
  PostLabel(L2);
  MatchString("ENDIF");
}

void DoWhile() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];

  Next();
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);
  PostLabel(L1);
  BoolExpression();
  BranchFalse(L2);
  Block(L2);
  MatchString("ENDWHILE");
  Branch(L1);
  PostLabel(L2);
}

void DoBreak(char *L) {
  if (L != 0) {
    Branch(L);
    Next();
  } else {
    Abort("No Loop to break from");
  }
}

void Block(char *L) {
  Scan();
  while (!(strchr("el", Token))) {
    switch (Token) {
    case 'i':
      DoIf(L);
      break;
    case 'w':
      DoWhile();
      break;
    case 'R':
      DoRead();
      break;
    case 'W':
      DoWrite();
      break;
    case 'b':
      DoBreak(L);
      break;
    default:
      Assignment();
      break;
    }
    Scan();
  }
}

void Assignment() {
  char Name[MAX_LABEL_LEN];
  CheckTable(Value);
  strncpy(Name, Value, MAX_LABEL_LEN);
  Next();
  MatchString("=");
  BoolExpression();
  StoreVar(Name);
}

void Alloc() {
  char Name[MAX_LABEL_LEN];
  Next();

  if (Token != 'x') {
    Expected("Variable Name");
  }
  strncpy(Name, Value, MAX_LABEL_LEN);

  CheckDup(Name);
  AddEndtry(Name, 'v');
  Next();

  if (Token == '=') {
    Next();
    if (Token != '#') {
      Expected("Number");
    }
    Allocate(Name, Value);
    Next();
  } else {
    Allocate(Name, "0");
  }
}

void TopDecls() {
  Scan();
  while (Token == 'v') {
    Alloc();
    while (Token == ',') {
      Alloc();
    }
  }
}

int main() {
  Init();
  MatchString("PROGRAM");
  Header();
  TopDecls();
  MatchString("BEGIN");
  Prolog();
  Block(0);
  MatchString("END");
  Epilog();

  return 0;
}
