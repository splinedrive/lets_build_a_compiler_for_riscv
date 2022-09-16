// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include <stdio.h>
#include <string.h>

#include "cradle.h"

void Expression();
void Ident();
void Block(char *L);

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

void Other() {
  snprintf(tmp, MAX_SPRINTF_LEN, "%c", GetName());
  EmitLn(tmp);
}

void Expression() {
#if 0
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
#else
  EmitLn("<expr>");
#endif
}

void Ident() {
  char name = GetName();
  if (Look == '(') {
    Match('(');
    Match(')');
    EmitLn("addi sp,sp,-4");
    EmitLn("sw ra,0(sp)");
    snprintf(tmp, MAX_SPRINTF_LEN, "call %c", name);
    EmitLn(tmp);
    EmitLn("lw ra,0(sp)");
    EmitLn("addi sp,sp,4");
    EmitLn("mv t0,a0");
  } else {
    snprintf(tmp, MAX_SPRINTF_LEN, "li t0,%c", name);
    EmitLn(tmp);
  }
}

void Assignment() {
  char name = GetName();

  Match('=');
  Expression();
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%c", name);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);
}

void Condition() { EmitLn("<condition>"); }

void DoProgram() {
  Block(0);
  if (Look != 'e')
    Expected("End");
  EmitLn("End");
}

void DoRepeat() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];

  Match('r');
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);
  PostLabel(L1);
  Block(L2);
  Match('u');
  Condition();
  snprintf(tmp, MAX_SPRINTF_LEN, "beq t0,x0,%s", L1);
  EmitLn(tmp);
}

void DoLoop() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  Match('p');
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);
  PostLabel(L1);
  Block(L2);
  Match('e');
  snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L1);
  EmitLn(tmp);
  PostLabel(L2);
}

void DoWhile() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  Match('w');
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);

  PostLabel(L1);
  Condition();

  snprintf(tmp, MAX_SPRINTF_LEN, "beq t0,x0,%s", L2);
  EmitLn(tmp);

  Block(L2);
  Match('e');
  snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L1);
  EmitLn(tmp);
  PostLabel(L2);
}

void DoIf(char *L) {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  Match('i');
  Condition();
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, L1, MAX_LABEL_LEN);
  snprintf(tmp, MAX_SPRINTF_LEN, "beq t0,x0,%s", L1);
  EmitLn(tmp);
  Block(L);
  if (Look == 'l') {
    Match('l');
    strncpy(L2, NewLabel(), MAX_LABEL_LEN);
    snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L2);
    EmitLn(tmp);
    PostLabel(L1);
    Block(L);
  }
  Match('e');
  PostLabel(L2);
}

void DoFor() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];

  Match('f');
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);

  // <ident> = <expr1>
  char Name = GetName();
  Match('=');
  Expression();
  // EmitLn("li t1,1");
  //  EmitLn("sub t0,t0,t1");
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%c", Name);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);

  // TO
  Expression();
  EmitLn("addi sp,sp,-4");
  EmitLn("sw t0,0(sp)");

  PostLabel(L1);
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%c", Name);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "lw t0,0(t1)");
  EmitLn(tmp);
  EmitLn("addi t0,x0,1");
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);
  EmitLn("lw t1,0(sp)");
  snprintf(tmp, MAX_SPRINTF_LEN, "bgt t0,t1,%s", L2);
  EmitLn(tmp);
  Block(L2);

  Match('e');
  snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L1);
  EmitLn(tmp);
  PostLabel(L2);
  EmitLn("addi sp,sp,4");
}

void DoDo() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];

  Match('d');
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);
  Expression();

  EmitLn("li t1,1");
  EmitLn("sub t0,t0,t1");
  PostLabel(L1);

  EmitLn("addi sp,sp,-4");
  EmitLn("sw t0,0(sp)");
  Block(L2);

  EmitLn("lw t0,0(sp)");
  EmitLn("addi sp,sp,4");

  snprintf(tmp, MAX_SPRINTF_LEN, "bne t0,x0,%s", L1);
  EmitLn(tmp);

  EmitLn("addi sp,sp,-4");
  PostLabel(L2);
  EmitLn("addi sp,sp,4");
}

void DoBreak(char *L) {
  Match('b');
  if (L != 0) {
    snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L);
    EmitLn(tmp);
  } else
    Abort("No loop to break from");
}

void Block(char *L) {
  while (!strchr("elu", Look)) {
    switch (Look) {
    case 'i':
      DoIf(L);
      break;
    case 'w':
      DoWhile();
      break;
    case 'p':
      DoLoop();
      break;
    case 'r':
      DoRepeat();
      break;
    case 'f':
      DoFor();
      break;
    case 'd':
      DoDo();
      break;
    case 'b':
      DoBreak(L);
      break;
    default:
      Other();
      break;
    }
  }
}

int main() {
  Init();
  EmitLn(".text");
  EmitLn(".global _start");
  EmitLn("_start:");

  DoProgram();
#if 0
   Assignment();
  if (Look != '\n')
    Expected("NewLine");
#endif

  return 0;
}
