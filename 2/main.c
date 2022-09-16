// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include <stdio.h>
#include <string.h>

#include "cradle.h"

void Expression();

void Factor() {
  if (Look == '(') {
    Match('(');
    Expression();
    Match(')');
  } else {
    sprintf(tmp, "addi t0,x0,%c", GetNum()); // li
    EmitLn(tmp);
  }
}

void Multiply() {
  Match('*');
  Factor();
  EmitLn("lb t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("mul t0,t1,t0");
}

void Divide() {
  Match('/');
  Factor();
  EmitLn("lb t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("div t0,t1,t0");
}

void Term() {
  Factor();
  while (IsMulop(Look)) {
    EmitLn("addi sp,sp,-4");
    EmitLn("sb t0,0(sp)");
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
  EmitLn("lb t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("add t0,t1,t0");
}

void Substract() {
  Match('-');
  Term();
  EmitLn("lb t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("sub t0,t1,t0");
}

void Expression() {
  if (IsAddop(Look)) {
    EmitLn("addi t0,x0,0");
  } else {
    Term();
  }
  while (IsAddop(Look)) {
    EmitLn("addi sp,sp,-4");
    EmitLn("sb t0,0(sp)");
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

int main() {
  Init();
  EmitLn(".text");
  EmitLn(".global _start");
  EmitLn("_start:");
  Expression();

  return 0;
}
