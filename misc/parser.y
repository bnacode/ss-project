%{
  #include <iostream>
  using namespace std;
  #include <stdio.h>
  extern int yylex();
  extern int yyparse();
  extern void yyabort();
  extern FILE *yyin;
  
  void yyerror(const char *s);


  #include <string>
  #include <string.h>

 

  #include "../inc/assembler.hpp"



%}



%union{
  int  ival;
  char *sval;
}


%token GLOBAL EXTERN SECTION WORD SKIP END
%token HALT INT IRET CALL RET JMP BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSRRD CSRWR
%token <sval> SYMBOL
%token <ival> NUM
%token<ival>REG
%token<ival>CSR
%token<sval>COMMA
%token<sval>COLON
%type<sval> SYMBOLLIST
%type<sval> SYMBOLNUMLIST
%token EOL
%token '$' '[' ']'


%%

input : lines;
lines : lines line | line;
line : label | label directive eol | directive eol| label instruction eol| instruction eol| eol;

directive:

GLOBAL SYMBOLLIST           {Assembler::global($2); }
  
|
  EXTERN SYMBOLLIST         {Assembler::externF($2);}
|
  SECTION SYMBOL            {Assembler::section($2);}
|
  WORD SYMBOLNUMLIST        {;}
| 
  SKIP NUM                  {Assembler::skip($2);}
|
  END                       {Assembler::end();}

instruction:
  halt | int | iret | call | ret | jmp | beq | bne | bgt | push | pop | xchg | add | sub | mul | div | not | and | or | xor | shl | shr | ld | st | csrrd | csrwr;

halt :  
  HALT {Assembler::halt();}
;

int :   
  INT  {Assembler::intF();}
;

iret :  
  IRET  {Assembler::iret();}
;

call :
  CALL NUM {Assembler::callNum($2);}
|
  CALL SYMBOL {Assembler::callSym($2);}
;

ret :  
  RET  {Assembler::ret();}
;

jmp:
  JMP NUM  {Assembler::jmpNum($2);}
|
  JMP SYMBOL  {Assembler::jmpSym($2);}
;

beq:  
  BEQ REG COMMA REG COMMA NUM {Assembler::beqNum($2, $4, $6);}
|
  BEQ REG COMMA REG COMMA SYMBOL {Assembler::beqSym($2, $4, $6);}
;

bne:
  BNE REG COMMA REG COMMA NUM {Assembler::bneNum($2, $4, $6);}
|
  BNE REG COMMA REG COMMA SYMBOL {Assembler::bneSym($2, $4, $6);}
;

bgt:
  BGT REG COMMA REG COMMA NUM {Assembler::bgtNum($2, $4, $6);}
|
  BGT REG COMMA REG COMMA SYMBOL {Assembler::bgtSym($2, $4, $6);}
;

push:
  PUSH REG {Assembler::push($2);}
;

pop:
  POP REG {Assembler::pop($2);}
;

xchg:
  XCHG REG COMMA REG {Assembler::xchg($2, $4);}
;

add:
  ADD REG COMMA REG {Assembler::add($2, $4);}
;

sub:
  SUB REG COMMA REG {Assembler::sub($2, $4);}
;

mul:
  MUL REG COMMA REG {Assembler::mul($2, $4);}
;

div:
  DIV REG COMMA REG {Assembler::div($2, $4);}
;

not:
  NOT REG {Assembler::notF($2);}
;

and:
  AND REG COMMA REG {Assembler::andF($2, $4);}
;

or:
  OR REG COMMA REG {Assembler::orF($2, $4);}
;

xor:
  XOR REG COMMA REG {Assembler::xorF($2, $4);}
;

shl:
  SHL REG COMMA REG {Assembler::shl($2, $4);}
;

shr:
  SHR REG COMMA REG {Assembler::shr($2, $4);}
;

ld:
  LD '$' NUM COMMA REG {Assembler::ldNum($3, $5);}
|
  LD '$' SYMBOL COMMA REG {Assembler::ldSym($3, $5);}
|
  LD NUM COMMA REG {Assembler::ldMemNum($2, $4);}
|
 LD SYMBOL COMMA REG {Assembler::ldMemSym($2, $4);}
|
  LD REG COMMA REG {Assembler::ldReg($2, $4);}
|
  LD '[' REG ']' COMMA REG {Assembler::ldMemReg($3, $6);}
|
  LD '[' REG '+' NUM ']' COMMA REG {Assembler::ldMemRegDispNum($3, $5, $8);}
;


st:
  ST REG COMMA NUM {Assembler::stMemNum($2, $4);}
|
  ST REG COMMA SYMBOL {Assembler::stMemSym($2, $4);}
|
  ST REG COMMA '[' REG ']' {Assembler::stMemReg($2, $5);}
|
  ST REG COMMA '[' REG '+' NUM ']' {Assembler::stMemRegDispNum($2, $5, $7);}
;

csrrd:
 CSRRD CSR COMMA REG {Assembler::csrrd($2, $4);}
;

csrwr:
  CSRWR REG COMMA CSR {Assembler::csrwr($2, $4);}
;





label: 
  SYMBOL COLON          {Assembler::label($1);}
|
  SYMBOL COLON eol       {Assembler::label($1);}
;



SYMBOLLIST:
  SYMBOL                      {$$=$1; strcat($$, " ");}
| 
  SYMBOLLIST COMMA SYMBOL     {strcat($$,$3); strcat($$, " "); };
;

SYMBOLNUMLIST:
  SYMBOL                        {Assembler::wordSym($1);}
|
  NUM                           {Assembler::wordNum($1);}
|
  SYMBOLNUMLIST COMMA SYMBOL    {Assembler::wordSym($3);}
|
  SYMBOLNUMLIST COMMA NUM       {Assembler::wordNum($3);}
;


eol : EOL {;}



%%

