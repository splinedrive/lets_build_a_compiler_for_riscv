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

void Negate() { EmitLn("negw t0,t0"); }

void Undefined(char *n) {
  snprintf(tmp, MAX_SPRINTF_LEN, "Undefined Identifier %s ", n);
  Abort(tmp);
}

void LoadConst(int n) {
  snprintf(tmp, MAX_SPRINTF_LEN, "li t0,%d", n);
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
  if (Look == '(') {
    Match('(');
    BoolExpression();
    Match(')');
  } else if (IsAlpha(Look)) {
    GetName();
    LoadVar(Value);
  } else {
    LoadConst(GetNum());
  }
}

void NegFactor() {
  Match('-');
  if (IsDigit(Look)) {
    LoadConst(-GetNum());
  } else {
    Factor();
    Negate();
  }
}

void FirstFactor() {
  switch (Look) {
  case '+':
    Match('+');
    Factor();
    break;
  case '-':
    NegFactor();
    break;
  default:
    Factor();
    break;
  }
}

void Multiply() {
  Match('*');
  Factor();
  PopMul();
}

void Divide() {
  Match('/');
  Factor();
  PopDiv();
}

void Term1() {
  while (IsMulop(Look)) {
    Push();
    switch (Look) {
    case '*':
      Multiply();
      break;
    case '/':
      Divide();
      break;
    }
  }
}

void Term() {
  Factor();
  Term1();
}

void FirstTerm() {
  FirstFactor();
  Term1();
}

void Add() {
  Match('+');
  Term();
  PopAdd();
}

void Subtract() {
  Match('-');
  Term();
  PopSub();
}

void Expression() {
  FirstTerm();
  while (IsAddop(Look)) {
    Push();
    switch (Look) {
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

void ReadVar() {
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", UART_READ);
  EmitLn(tmp);
  EmitLn("lw t0,0(t1)");
  StoreVar(Value);
}

void WriteVar() {
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", UART_WRITE);
  EmitLn(tmp);
  EmitLn("sw t0,0(t1)");
}

void DoRead() {
  Match('(');
  GetName();
  ReadVar();
  while (Look == ',') {
    Match(',');
    GetName();
    ReadVar();
  }
  Match(')');
}

void DoWrite() {
  Match('(');
  Expression();
  WriteVar();
  while (Look == ',') {
    Match(',');
    Expression();
    WriteVar();
  }
  Match(')');
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

void Equals() {
  Match('=');
  Expression();
  PopCompare();
  SetEqual();
}

void NotEqual() {
  Match('>');
  Expression();
  PopCompare();
  SetNEqual();
}

void LessOrEqual() {
  Match('=');
  Expression();
  PopCompare();
  SetLessOrEqual();
}

void Less() {
  Match('<');
  switch (Look) {
  case '=':
    LessOrEqual();
    break;
  case '>':
    NotEqual();
    break;
  default:
    Expression();
    PopCompare();
    SetLess();
    break;
  }
}

void Greater() {
  Match('>');
  if (Look == '=') {
    Match('=');
    Expression();
    PopCompare();
    SetGreaterOrEqual();
  } else {
    Expression();
    PopCompare();
    SetGreater();
  }
}

void Relation() {
  Expression();
  if (IsRelop(Look)) {
    Push();
    switch (Look) {
    case '=':
      Equals();
      break;
      /*
    case '#':
      NotEquals();
      break;
      */
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
  if (Look == '!') {
    Match('!');
    Relation();
    NotIt();
  } else {
    Relation();
  }
}

void BoolTerm() {
  NotFactor();
  while (Look == '&') {
    Push();
    Match('&');
    NotFactor();
    PopAnd();
  }
}

void BoolOr() {
  Match('~');
  BoolTerm();
  PopOr();
}

void BoolXor() {
  Match('~');
  BoolTerm();
  PopXor();
}

void BoolExpression() {
  BoolTerm();
  while (IsOrop(Look)) {
    Push();
    switch (Look) {
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

void DoIf() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];

  BoolExpression();
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, L1, MAX_LABEL_LEN);
  BranchFalse(L1);
  Block();
  if (Token == 'l') {
    strncpy(L2, NewLabel(), MAX_LABEL_LEN);
    Branch(L2);
    PostLabel(L1);
    Block();
  }
  PostLabel(L2);
  MatchString("ENDIF");
}

void DoWhile() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];

  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);
  PostLabel(L1);
  BoolExpression();
  BranchFalse(L2);
  Block();
  MatchString("ENDWHILE");
  Branch(L1);
  PostLabel(L2);
}

void Block() {
  Scan();
  while (!(strchr("el", Token))) {
    switch (Token) {
    case 'i':
      DoIf();
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
    default:
      Assignment();
      break;
    }
    Scan();
  }
}

void Assignment() {
  char *Name;
  Name = Value;
  Match('=');
  // Expression();
  BoolExpression();
  StoreVar(Name);
  SkipWhite();
}

void Alloc(Symbol N) {
  if (InTable(N)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "Duplicate Variable Name %s ", N);
    Abort(tmp);
  }
  AddEndtry(N, 'v');
  printf("%s:\t .word ", N);

  if (Look == '=') {
    Match('=');
    if (Look == '-') {
      printf("%c", Look);
      Match('-');
    }
    printf("%d\n", GetNum());
  } else {
    printf("0\n");
  }
}

void Main() {
  MatchString("BEGIN");
  Prolog();
  Block();
  MatchString("END");
  Epilog();
}

void Decl() {
  GetName();
  Alloc(Value);

  while (Look == ',') {
    Match(',');
    GetName();
    Alloc(Value);
  }
}

void TopDecls() {
  Scan();
  while (Token != 'b') {
    char tmp[MAX_MSG_LEN];
    switch (Token) {
    case 'v':
      Decl();
      break;
    default:
      snprintf(tmp, MAX_SPRINTF_LEN, "Unrecognized Keyword %s ", Value);
      Abort(tmp);
      break;
    }
    Scan();
  }
}

void Prog() {
  MatchString("PROGRAM");
  Header();
  TopDecls();
  Main();
  Match('.');
}

int main() {
  Init();
  Prog();
  return 0;
}
