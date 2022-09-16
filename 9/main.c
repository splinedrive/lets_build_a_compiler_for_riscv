// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char Class;
char Typ;
char Sign;

void Statements() {
  Match('b');
  while (Look != 'e') {
    GetChar();
  }
  Match('e');
}

void Labels() { Match('l'); }

void Constants() { Match('c'); }

void Types() { Match('t'); }

void Variables() { Match('v'); }

void DoProcedure() { Match('p'); }

void DoFunction() { Match('f'); }

void Declarations() {
  while (strchr("lctvpf", Look)) {
    switch (Look) {
    case 'l':
      Labels();
      break;
    case 'c':
      Constants();
      break;
    case 't':
      Types();
      break;
    case 'v':
      Variables();
      break;
    case 'p':
      DoProcedure();
      break;
    case 'f':
      DoFunction();
      break;
    }
  }
}

void DoBlock(char Name) {
  Declarations();
  PostLabel(Name);
  Statements();
}

void Prolog() {
  EmitLn(".text");
  EmitLn(".global _start");
  EmitLn("_start:");
}
void Epilog(char Name) {
  EmitLn(".section .data");
  sprintf(tmp, "%c: .word 0", Name);
  EmitLn(tmp);
}

#if 0
void Prog() {
  while (Look != EOF) {
    switch (Look) {
      case '#': PreProc(); break;
      case 'i': IntDecl(); break;
      case 'c': CharDecl(); break;
      default:
      DoFunction();
    }
  }
}
#endif

void Prog() {
  char Name;
  Match('p');
  Name = GetName();
  Prolog();
  DoBlock(Name);
  Match('.');
  Epilog(Name);
}

void GetClass() {
  if (strchr("axs", Look)) {
    Class = Look;
    GetChar();
  } else {
    Class = 'a';
  }
}
void GetType() {
  Typ = ' ';
  if (Look == 'u') {
    Sign = 'u';
    Typ = 'i';
    GetChar();
  } else {
    Sign = 's';
  }
  if (strchr("ilc", Look)) {
    Typ = Look;
    GetChar();
  }
}

void DoFunc(char n) {
  Match('(');
  Match(')');
  Match('{');
  Match('}');

  if (Typ == ' ') {
    Typ = 'i';
  }
  printf("%c, %c, %c function %c", Class, Sign, Typ, n);
}

void DoData(char n) {
  if (Typ == ' ') {
    Expected("Type declaration");
  }
  printf("%c, %c, %c data %c ", Class, Sign, Typ, n);
  while (Look == ',') {
    Match(',');
    n = GetName();
    printf("%c, %c, %c data %c ", Class, Sign, Typ, n);
  }
  Match(';');
}

void TopDecl() {
  char Name;

  Name = GetName();
  if (Look == '(') {
    DoFunc(Name);
  } else {
    DoData(Name);
  }
}

int main() {
  Init();
  // Prog();
  while (Look != EOF) {
    GetClass();
    GetType();
    TopDecl();
    while (strchr("\n\f", Look) != 0) {
      printf("%c", Look);
      GetChar();
    }
  }

  return 0;
}
