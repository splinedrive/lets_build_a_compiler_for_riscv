// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include <stdio.h>
#include <string.h>

#include "cradle.h"

int Expression();
void Ident();

int Factor() {
  int factor = 0;
  if (Look == '(') {
    Match('(');
    factor = Expression();
    Match(')');
  } else if (IsAlpha(Look)) {
    factor = Table[GetName() - 'A'];
  } else {
    factor = GetNum();
  }
  return factor;
}

int Term() {
  int value = Factor();
  while (IsMulop(Look)) {
    switch (Look) {
    case '*':
      Match('*');
      value *= Factor();
      break;
    case '/':
      Match('/');
      value /= Factor();
      break;
    default:
      Expected("Mulop");
    }
  }
  return value;
}

int Expression() {
  int value = 0;
  if (IsAddop(Look)) {
    value = 0;
  } else {
    value = Term();
  }
  while (IsAddop(Look)) {
    switch (Look) {
    case '+':
      Match('+');
      value += Term();
      break;
    case '-':
      Match('-');
      value -= Term();
      break;
    default:
      Expected("Addop");
    }
  }
  return value;
}

void Assignment() {
  char name = GetName();
  Match('=');
  Table[name - 'A'] = Expression();
}

void Input() {
  Match('?');
  Table[GetName() - 'A'] = Expression();
}

void Output() {
  Match('!');
  sprintf(tmp, "%d", Table[GetName() - 'A']);
  EmitLn(tmp);
}

int main() {
  Init();

  do {
    SkipWhite();
    switch (Look) {
    case '?':
      Input();
      break;
    case '!':
      Output();
      break;
    default:
      Assignment();
    }
    NewLine();
  } while (Look != '.');

  return 0;
}
