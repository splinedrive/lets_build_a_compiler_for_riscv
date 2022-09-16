// adapted to c/RISCV 2022 by hirosh dabui <hirosh@dabui.de>
#include "cradle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void Move(char Size, char *Source, char *Dest) {
  snprintf(tmp, MAX_SPRINTF_LEN, "%s %s,%s",
           (UpCase(Size) == 'B' ? "lb" : UpCase(Size) == 'W' ? "lh" : "lw"),
           Dest, Source);
  EmitLn(tmp);
}

void LoadVar(char Name, char Typ) {
  char Source[MAX_MSG_LEN];
  snprintf(tmp, MAX_SPRINTF_LEN, "%c", Name);
  strncpy(Source, tmp, MAX_MSG_LEN);
  Move(Typ, Source, "t0");
}

void Convert(char Source, char Dest) {
  if (Source != Dest) {
    if (Source == 'B') {
      EmitLn("andi t0,t0,0xff");
    }
    if (Source == 'L') {
      // treated signed extended lh
    }
  }
}

char VarType(char Name) {
  char Typ = TypeOf(Name);
  if (!IsVarType(Typ)) {
    snprintf(tmp, MAX_SPRINTF_LEN, "Identifier %c is not a variable", Name);
    EmitLn(tmp);
  }
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
  Convert(T1, T2);
  StoreVar(Name, T2);
}

char Expression() { return Load(GetName()); }

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
