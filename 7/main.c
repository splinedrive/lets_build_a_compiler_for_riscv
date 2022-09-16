// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include <stdio.h>
#include <string.h>

#include "cradle.h"

void BoolExpression();
void Expression();
void Ident();
void Block(char *L);
void Relation();

void Factor() {
  if (Look == '(') {
    Match('(');
    Expression();
    Match(')');
  } else if (IsAlpha(Look)) {
    Ident();
  } else {
    GetNum();
    char tmp[MAX_SPRINTF_LEN];
    snprintf(tmp, MAX_SPRINTF_LEN, "li t0,%s", Value);
    EmitLn(tmp);
  }
}

void SignedFactor() {
  int s = Look == '-';
  if (IsAddop(Look)) {
    GetChar();
    SkipWhite();
  }
  Factor();
  if (s) {
    EmitLn("negw t0,t0");
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

void Term1() {
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

void FirstTerm() {
  SignedFactor();
  Term1();
}

void Term() {
  Factor();
  Term1();
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
  char tmp[MAX_SPRINTF_LEN];
  snprintf(tmp, MAX_SPRINTF_LEN, "%s", Value);
  EmitLn(tmp);
}

void Expression() {
  FirstTerm();
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
  char tmp[MAX_SPRINTF_LEN];
  GetName();
  if (Look == '(') {
    Match('(');
    Match(')');
    EmitLn("addi sp,sp,-4");
    EmitLn("sw ra,0(sp)");
    snprintf(tmp, MAX_SPRINTF_LEN, "call %s", Value);
    EmitLn(tmp);
    EmitLn("lw ra,0(sp)");
    EmitLn("addi sp,sp,4");
    EmitLn("mv t0,a0");
  } else {
    snprintf(tmp, MAX_SPRINTF_LEN, "lw t0,%s", Value);
    EmitLn(tmp);
  }
}

void Assignment() {
  char tmp[MAX_SPRINTF_LEN];
  char Name[MAX_TOKEN_LEN];
  strncpy(Name, Value, MAX_LABEL_LEN);

  Match('=');
  //  BoolExpression();
  Expression();
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", Name);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);
}

void Condition() { BoolExpression(); }

void DoProgram() {
  Block(0);
  MatchString("END");
  EmitLn("End");
}

void DoRepeat() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  char tmp[MAX_SPRINTF_LEN];

  //  Match('r');
  MatchString("REPEAT");
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);
  PostLabel(L1);
  Block(L2);
  // Match('u');
  MatchString("UNTIL");
  Condition();
  snprintf(tmp, MAX_SPRINTF_LEN, "beq t0,x0,%s", L1);
  EmitLn(tmp);
}

void DoLoop() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  char tmp[MAX_SPRINTF_LEN];
  // Match('p');
  MatchString("LOOP");
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);
  PostLabel(L1);
  Block(L2);
  // Match('e');
  MatchString("END");
  snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L1);
  EmitLn(tmp);
  PostLabel(L2);
}

void DoWhile() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  char tmp[MAX_SPRINTF_LEN];
  // Match('w');
  MatchString("WHILE");
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);

  PostLabel(L1);
  Condition();

  snprintf(tmp, MAX_SPRINTF_LEN, "beq t0,x0,%s", L2);
  EmitLn(tmp);

  Block(L2);
  // Match('e');
  MatchString("END");
  snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L1);
  EmitLn(tmp);
  PostLabel(L2);
}

void DoIf(char *L) {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  char tmp[MAX_SPRINTF_LEN];
  MatchString("IF");
  Condition();
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, L1, MAX_LABEL_LEN);
  snprintf(tmp, MAX_SPRINTF_LEN, "beq t0,x0,%s", L1);
  EmitLn(tmp);
  Block(L);
  if (Token == 'l') {
    strncpy(L2, NewLabel(), MAX_LABEL_LEN);
    snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L2);
    EmitLn(tmp);
    PostLabel(L1);
    Block(L);
  }
  PostLabel(L2);
  MatchString("ENDIF");
}

void DoFor() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  char tmp[MAX_SPRINTF_LEN];

  // Match('f');
  MatchString("FOR");
  strncpy(L1, NewLabel(), MAX_LABEL_LEN);
  strncpy(L2, NewLabel(), MAX_LABEL_LEN);

  // <ident> = <expr1>
  GetName();
  Match('=');
  // MatchString("=");
  Expression();
  //  EmitLn("li t1,1");
  //  EmitLn("sub t0,t0,t1");
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", Value);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);

  // TO
  Expression();
  EmitLn("addi sp,sp,-4");
  EmitLn("sw t0,0(sp)");

  PostLabel(L1);
  snprintf(tmp, MAX_SPRINTF_LEN, "la t1,%s", Value);
  EmitLn(tmp);
  snprintf(tmp, MAX_SPRINTF_LEN, "lw t0,0(t1)");
  EmitLn(tmp);
  EmitLn("addi t0,t0,1");
  snprintf(tmp, MAX_SPRINTF_LEN, "sw t0,0(t1)");
  EmitLn(tmp);
  EmitLn("lw t1,0(sp)");
  snprintf(tmp, MAX_SPRINTF_LEN, "bgt t0,t1,%s", L2);
  EmitLn(tmp);
  Block(L2);

  // Match('e');
  MatchString("END");
  snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L1);
  EmitLn(tmp);
  PostLabel(L2);
  EmitLn("addi sp,sp,4");
}

void DoDo() {
  char L1[MAX_LABEL_LEN];
  char L2[MAX_LABEL_LEN];
  char tmp[MAX_SPRINTF_LEN];

  MatchString("DO");
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
  char tmp[MAX_SPRINTF_LEN];
  // Match('b');
  MatchString("BREAK");
  if (L != 0) {
    snprintf(tmp, MAX_SPRINTF_LEN, "j %s", L);
    EmitLn(tmp);
  } else
    Abort("No loop to break from");
}

void BoolFactor() {
  if (IsBoolean(Look)) {
    if (GetBoolean()) {
      EmitLn("li t0,-1");
    } else {
      EmitLn("li t0,0");
    }
  } else {
    Relation();
  }
}

void NotFactor() {
  if (Look == '!') {
    Match('!');
    BoolFactor();
    EmitLn("not t0,t0");
  } else {
    BoolFactor();
  }
}

void BoolTerm() {
  NotFactor();
  while (Look == '&') {
    EmitLn("addi sp,sp,-4");
    EmitLn("sw t0,0(sp)");
    Match('&');
    NotFactor();
    EmitLn("lw t1,0(sp)");
    EmitLn("addi sp,sp,4");
    EmitLn("and t0,t1,t0");
  }
}

void BoolOr() {
  Match('|');
  BoolTerm();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("or t0,t1,t0");
}

void BoolXor() {
  Match('~');
  BoolTerm();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("xor t0,t1,t0");
}

void Equals() {
  Match('=');
  Expression();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("sub t0,t1,t0");
  EmitLn("seqz t0,t0");
}

void NotEquals() {
  Match('#');
  Expression();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("sub t0,t1,t0");
  EmitLn("snez t0,t0");
}

void Less() {
  Match('<');
  Expression();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("sub t0,t1,t0");
  EmitLn("sltz t0,t0");
}

void Greater() {
  Match('>');
  Expression();
  EmitLn("lw t1,0(sp)");
  EmitLn("addi sp,sp,4");
  EmitLn("sub t0,t1,t0");
  EmitLn("sgtz t0,t0");
}

void BoolExpression() {
  BoolTerm();
  while (IsOrOp(Look)) {
    EmitLn("addi sp,sp,-4");
    EmitLn("sw t0,0(sp)");

    switch (Look) {
    case '|':
      BoolOr();
      break;
    case '~':
      BoolXor();
      break;
    default:
      Expected("BoolExpression");
    }
  }
}

void Relation() {
  Expression();
  if (IsRelop(Look)) {
    EmitLn("addi sp,sp,-4");
    EmitLn("sw t0,0(sp)");

    switch (Look) {
    case '=':
      Equals();
      break;
    case '#':
      NotEquals();
      break;
    case '<':
      Less();
      break;
    case '>':
      Greater();
      break;
    default:
      Expected("Relation");
    }
  }
}

void Block(char *L) {
  Scan();
  while (!strchr("elu", Token)) {
    switch (Token) {
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
      Assignment();
      break;
    }
    Scan();
  }
}

int main() {
  Init();
  EmitLn(".text");
  EmitLn(".global _start");
  EmitLn("_start:");

  DoProgram();

  return 0;
}
