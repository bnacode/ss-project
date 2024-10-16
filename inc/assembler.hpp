#include "common.hpp"

extern int yyparse();

class Assembler{

  public:
    static SectionType currentSection;
    static int currentSectionNum;
    static unordered_map<string, int> sectionMap;
    static int locationCounter;
    static bool first;
    static vector<Symbol*> symbolTable;
    static vector<Relocation*> relocationTable;
    static vector<Section*> sectionTable;

    Assembler();

    static int rotateBytes(int value);
    static bool isLocal(string symbol);
    static int getOffsetToSymbolPool();
    static int getRelativeOffsetToSymbolPool();
    
    static bool isInSymbolTable(string, int*);
    static int getSymbolNum(string s);
    static void pushByByte(int val);
    static void pushInstructionsByByte(int op_code, int mod, int A, int B, int C, int D);
    static bool isLargerThan12Bits(int value);
    static void setCurrentSectionSize();
    static void setInstructionContext(unsigned char op, int regS, int regD);

    static void global(string symbolList);
    static void externF(string symbolList);
    static void wordNum(int literal);
    static void wordSym(string symbol);
    static void skip(int num);
    static void section(string sectionName);
    static void label(string label);
    static void halt();
    static void intF();
    static void iret();
    static void callNum(int num);
    static void callSym(string symbol);
    static void ret();
    static void jmpNum(int num);
    static void jmpSym(string symbol);
    static void beqNum(int reg1, int reg2, int num);
    static void beqSym(int reg1, int reg2, string symbol);
    static void bneNum(int reg1, int reg2, int num);
    static void bneSym(int reg1, int reg2, string symbol);
    static void bgtNum(int reg1, int reg2, int num);
    static void bgtSym(int reg1, int reg2, string symbol);
    static void push(int reg);
    static void pop(int reg);
    static void xchg(int regS, int regD);
    static void add(int regS, int regD);
    static void sub(int regS, int regD);
    static void mul(int regS, int regD);
    static void div(int regS, int regD);
    static void notF(int reg);
    static void andF(int regS, int regD);
    static void orF(int regS, int regD);
    static void xorF(int regS, int regD);
    static void shl(int regS, int regD);
    static void shr(int regS, int regD);


    static void ldNum(int num, int regD);
    static void ldSym(string symbol, int regD);
    static void ldReg(int regS, int regD);
    static void ldMemNum(int num, int regD); 
    static void ldMemSym(string symbol, int regD);
    static void ldMemReg(int regS, int regD);
    static void ldMemRegDispNum(int regS,int numDisp, int regD);
    
    static void stMemNum(int regS, int num);
    static void stMemSym(int regS, string symbol);
    static void stMemReg(int regS, int regD);
    static void stMemRegDispNum(int regS, int regD, int dispNum);
    

    static void csrrd(int csr, int reg);
    static void csrwr(int csr, int reg);

    static void end();

   
};





