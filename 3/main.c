// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include <stdio.h>
#include <string.h>

#include "cradle.h"

void Expression();
void Ident();

void Factor() {
  if (Look == '(') {
    Match('(');
    Expression();
    Match(')');
  } else if (IsAlpha(Look)) {
    Ident();
  } else {
    snprintf(tmp, MAX_SPRINTF_LEN, "li t0,%s", GetNum());
    EmitLn(tmp);
  }
}

void Multiply() {
  Match('*');
  Factor();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");

  EmitLn("mul t0,t1,t0");
}

void Divide() {
  Match('/');
  Factor();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");

  EmitLn("div t0,t1,t0");
}

void Term() {
  Factor();
  while (IsMulop(Look)) {
    EmitLn("addi sp,sp,-4");
    EmitLn("sw t0,0(sp)");
    switch (Look) {
    case '*':
      Multiply();
      break;
    case '/':
      Divide();
      break;
    default:
      Expected("Mulop");
    }
  }
}

void Add() {
  Match('+');
  Term();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");

  EmitLn("add t0,t1,t0");
}

void Substract() {
  Match('-');
  Term();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");

  EmitLn("sub t0,t1,t0");
}

void Expression() {
  if (IsAddop(Look)) {
    EmitLn("li t0,0");
  } else {
    Term();
  }
  while (IsAddop(Look)) {
    EmitLn("addi sp,sp,-4");
    EmitLn("sw t0,0(sp)");
    switch (Look) {
    case '+':
      Add();
      break;
    case '-':
      Substract();
      break;
    default:
      Expected("Addop");
    }
  }
}

void Ident() {
  char *name = GetName();
  if (Look == '(') {
    Match('(');
    Match(')');
    EmitLn("addi sp,sp,-4");
    EmitLn("sw ra,0(sp)");
    snprintf(tmp, MAX_SPRINTF_LEN, "call %s", name);
    EmitLn(tmp);
    EmitLn("lw ra,0(sp)");
    EmitLn("addi sp,sp,4");
    EmitLn("mv t0,a0");
  } else {
    snprintf(tmp, MAX_SPRINTF_LEN, "li t0,%s", name);
    EmitLn(tmp);
  }
}

void Assignment() {
  char name[MAX_TOKEN_LEN];
  strncpy(name, GetName(), MAX_TOKEN_LEN);

  Match('=');
  Expression();
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", name);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);
}

int main() {
  Init();
  EmitLn(".text");
  EmitLn(".global _start");
  EmitLn("_start:");

  Assignment();
  if (Look != '\n')
    Expected("NewLine");

  return 0;
}
