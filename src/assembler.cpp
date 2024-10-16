#include "../inc/assembler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <queue>
using namespace std;

extern FILE *yyin;


vector<Symbol *> Assembler::symbolTable = vector<Symbol *>();
vector<Relocation *> Assembler::relocationTable = vector<Relocation *>();
vector<Section *> Assembler::sectionTable = vector<Section *>();

SectionType Assembler::currentSection = "undefined";
bool Assembler::first = true;
int Assembler::locationCounter = 0;
int Assembler::currentSectionNum = -1;

#define pc 15
#define sp 14
#define status 0


void Assembler::setCurrentSectionSize()
{
  for (Section *s : Assembler::sectionTable)
  {
    if (s->section == Assembler::currentSection)
    {
      s->size = Assembler::locationCounter;
      break;
    }
  }
}

void Assembler::global(string symbolList)
{

  // Parsing list of symbols
  queue<string> symbolQueue;

  istringstream iss(symbolList);
  string symbol;

  while (iss >> symbol)
  {
    symbolQueue.push(symbol);
  }

  if (Assembler::first)
  {

    while (!symbolQueue.empty())
    {
      symbol = symbolQueue.front();
      int row;

      // If its found in symbolTable
      if (!isInSymbolTable(symbol, &row))
      {

        
        Symbol *s = new Symbol(symbol, currentSection, 0, Glocal::global, symbolTable.size() + 1);
        symbolTable.push_back(s);

        symbolQueue.pop();
      }
      else
      {
        std::cout << "row";
        symbolQueue.pop();
      }
    }
  }
  else
  {
   
  }
}

void Assembler::externF(string symbolList)
{

  // Parsing list of symbols
  queue<string> symbolQueue;

  istringstream iss(symbolList);
  string symbol;

  while (iss >> symbol)
  {
    symbolQueue.push(symbol);
  }

  if (Assembler::first)
  {

    while (!symbolQueue.empty())
    {
      symbol = symbolQueue.front();
      int row;

      // If its found in symbolTable
      if (!Assembler::isInSymbolTable(symbol, &row))
      {

        // da li je locationCounter ili locationCounter + mesto u listi simbola
        Symbol *s = new Symbol(symbol, Assembler::currentSection, 0, Glocal::global, Assembler::symbolTable.size() + 1, true);
        Assembler::symbolTable.push_back(s);
        cout << endl;

        symbolQueue.pop();
      }
      else
      {
        cout << row;
        symbolQueue.pop();
      }
    }
  }
}

void Assembler::section(string sectionName)
{

  if (first)
  {

    setCurrentSectionSize();
    locationCounter = 0;
    currentSection = sectionName;
    // sectionMap[currentSection] = currentSectionNum;
    currentSectionNum++;
    int row;

    if (!isInSymbolTable(sectionName, &row))
    {

      Section *section = new Section(sectionName);
      sectionTable.push_back(section);

      //cout << "UBACUJEM SIMBOL SEKCIJE " << sectionName << endl;
      Symbol *sym = new Symbol(sectionName, Assembler::currentSection, locationCounter, Glocal::global, Assembler::symbolTable.size() + 1);
      sym->isSection = true;
      Assembler::symbolTable.push_back(sym);
    }
    else
    {
      cout << "Vec u tabeli";
    }
  }
  else
  {
     if(currentSectionNum != -1){
     
      if(!sectionTable[currentSectionNum]->pool.empty()){
        
        int poolSize = sectionTable[currentSectionNum]->pool.size()*4;
        
        pushInstructionsByByte(3,0,pc,0,0,poolSize);

        sectionTable[currentSectionNum]->size += 4;
      }
    }
    
    currentSection = sectionName;
    locationCounter = 0;
    currentSectionNum++;
  }
}

void Assembler::pushInstructionsByByte(int op_code, int mod, int A, int B, int C, int D){
  unsigned char firstByte = ((op_code & 0x0F) << 4) | (mod & 0x0F);
  unsigned char secondByte = ((A & 0x0F) << 4) | (B & 0x0F);
  unsigned char thirdByte = ((C & 0x0F )<< 4) | ((D & 0xF00)>>8);
  unsigned char fourthByte = (D & 0xFF );

  sectionTable[currentSectionNum]->content->push_back(firstByte);
  sectionTable[currentSectionNum]->content->push_back(secondByte);
  sectionTable[currentSectionNum]->content->push_back(thirdByte);
  sectionTable[currentSectionNum]->content->push_back(fourthByte);
}

int Assembler::getSymbolNum(string symbol)
{
  for (Symbol *s : symbolTable)
  {
    if (s->label == symbol)
    {
      return s->num;
    }
  }
  return 0;
}

bool Assembler::isInSymbolTable(string symbol, int *row = nullptr)
{
  int iteration = 0;
  for (Symbol *s : Assembler::symbolTable)
  {
    if (s->label == symbol)
    {
      *row = iteration;
      return true;
    }
    iteration++;
  }
  return false;
}

void Assembler::pushByByte(int literal) {
  for (int i = 0; i < 4; i++) {
    unsigned char byte = static_cast<unsigned char>(literal & 0xFF);
    sectionTable[currentSectionNum]->content->push_back(byte);
    literal >>= 8;
  }
}

int Assembler::getRelativeOffsetToSymbolPool(){
  return sectionTable[currentSectionNum]->size + 4 - locationCounter - 4 + sectionTable[currentSectionNum]->pool.size()*sizeof(int);
}

int Assembler::getOffsetToSymbolPool(){
  return  sectionTable[currentSectionNum]->size +4  + sectionTable[currentSectionNum]->pool.size()*sizeof(int);
}

void Assembler::wordNum(int literal)
{
  if (!first)
    pushByByte(literal);
  
  locationCounter += 4;
}

bool Assembler:: isLocal(string symbol){
  for(Symbol* s : symbolTable){
    if(s->label == symbol){
      if(s->glocal == Glocal::local)
        return true;
      else
        return false;
    }
  }
  return false;
}

void Assembler::wordSym(string symbol)
{
  if(!first)
  {
    int row;
    if (isInSymbolTable(symbol, &row))
    {
      if(isLocal(symbol)){
        pushByByte(0);
        Relocation *r = new Relocation(currentSection, locationCounter, REL, getSymbolNum(currentSection), symbolTable[row]->offset);
        relocationTable.push_back(r); 
      }
      else{
        pushByByte(0);
        Relocation *r = new Relocation(currentSection, locationCounter, ABS, getSymbolNum(symbol), 0);
        relocationTable.push_back(r);
      }
    }
    else
    {
      std::cout << "simbol ne postoji u tabeli";
    }
  }
  locationCounter += 4;
}

void Assembler::skip(int num)
{
  if(!first)
  {
    for(int i = 0; i < num; i++)
      sectionTable[currentSectionNum]->content->push_back(0);
  }
  locationCounter += num;
}

void Assembler::label(string label)
{
  if (first)
  {
    int row;

    if (isInSymbolTable(label, &row))
    {
      symbolTable[row]->offset = locationCounter;
      symbolTable[row]->section = currentSection;
    }
    else
    {
      Symbol *s = new Symbol(label, Assembler::currentSection, Assembler::locationCounter, local, symbolTable.size() + 1, false);
      Assembler::symbolTable.push_back(s);
    }
  }
}

void Assembler::halt() 
{
  if (!first)
    pushByByte(0);
    
  locationCounter += 4;
}

void Assembler::intF()
{
  if(!first)
    pushByByte(0x10);
  
  locationCounter += 4;
}

void Assembler::iret()
{
  if (!first)
  {
    pushInstructionsByByte(9,1,sp,sp,0,8);
    pushInstructionsByByte(9,6,status,sp,0,-4);
    pushInstructionsByByte(9,2,pc,sp,0,-8);
  }
  locationCounter += 12;
}

void Assembler::callNum(int num)
{
  if (!first)
  {
    if(isLargerThan12Bits(num)){
      int offset = getRelativeOffsetToSymbolPool();
      num = rotateBytes(num);
      sectionTable[currentSectionNum]->pool.push_back(num);
      pushInstructionsByByte(2,1,pc,0,0,offset); 
    }
    else{
      pushInstructionsByByte(2,0,0,0,0,num);
    }
  }
  locationCounter += 4;
}

unsigned int getSymOffset(string s){
  for(Symbol* sym : Assembler::symbolTable){
    if(sym->label == s)
      return sym->offset;
  }
  return 0;
}

void Assembler::callSym(string symbol)
{
  if(!first)
  {
    int offset = getRelativeOffsetToSymbolPool();
    int symOffset = getOffsetToSymbolPool();

    if(isLocal(symbol)){
      Relocation *r = new Relocation(currentSection, symOffset, REL, getSymbolNum(currentSection), getSymOffset(symbol));
      relocationTable.push_back(r);
    }
    else{
      Relocation* r = new Relocation(currentSection, symOffset, ABS, getSymbolNum(symbol), 0);
      relocationTable.push_back(r);
    }

    sectionTable[currentSectionNum]->pool.push_back(0);
    pushInstructionsByByte(2,1,pc,0,0,offset); 
  }
  locationCounter += 4;
}

void Assembler::ret()
{
  if (first)
  {
   locationCounter += 4;
  }
  else{
    pop(pc);
  }
   
}

int Assembler::rotateBytes(int value) {
    int result = 0;

    // Extract each byte and shift it to the appropriate position
    for (unsigned int i = 0; i < sizeof(value); i++) {
        int byte = (value >> (8 * i)) & 0xFF;
        result |= byte << (8 * (sizeof(value) - 1 - i));
    }
    return result;
}

void Assembler::jmpNum(int num)
{
  if (!first)
  {
    if(isLargerThan12Bits(num)){
      int offset = getRelativeOffsetToSymbolPool();
      num = rotateBytes(num);
      sectionTable[currentSectionNum]->pool.push_back(num);
      pushInstructionsByByte(3,8,pc,0,0,offset); 
    }
    else{
      pushInstructionsByByte(3,0,0,0,0,num);
    }
  }
  locationCounter += 4;
}

void Assembler::jmpSym(string symbol)
{
  if(!first)
  {
    int offset = getRelativeOffsetToSymbolPool();
    int symOffset = getOffsetToSymbolPool();

    if(isLocal(symbol)){
      Relocation *r = new Relocation(currentSection, symOffset, REL, getSymbolNum(currentSection), getSymOffset(symbol));
      relocationTable.push_back(r);
    }
    else{
      Relocation* r = new Relocation(currentSection, symOffset, ABS, getSymbolNum(symbol), 0);
      relocationTable.push_back(r);
    }
    sectionTable[currentSectionNum]->pool.push_back(0);
    pushInstructionsByByte(3,8,pc,0,0,offset);
  }
   locationCounter += 4;
}

void Assembler::beqNum(int reg1, int reg2, int num)
{
  if(!first)
  {
    if(isLargerThan12Bits(num)){
      int offset = getRelativeOffsetToSymbolPool();
      num = rotateBytes(num);
      sectionTable[currentSectionNum]->pool.push_back(num);
      pushInstructionsByByte(3,9,pc,reg1,reg2,offset); 
    }
    else{
      pushInstructionsByByte(3,1,0,reg1,reg2,num);
    }
  }
  locationCounter += 4;
}

void Assembler::beqSym(int reg1, int reg2, string symbol)
{
  if (!first)
  {
    int offset = getRelativeOffsetToSymbolPool();
    int symOffset = getOffsetToSymbolPool();


    if(isLocal(symbol)){
      Relocation *r = new Relocation(currentSection, symOffset, REL, getSymbolNum(currentSection), getSymOffset(symbol));
      relocationTable.push_back(r);
    }
    else{
      Relocation* r = new Relocation(currentSection, symOffset, ABS, getSymbolNum(symbol), 0);
      relocationTable.push_back(r);
    }

    sectionTable[currentSectionNum]->pool.push_back(0);
    pushInstructionsByByte(3,9,pc,reg1,reg2,offset);
  }
   locationCounter += 4;
}

void Assembler::bneNum(int reg1, int reg2, int num)
{
  if (!first)
  {
    if(isLargerThan12Bits(num)){
      int offset = getRelativeOffsetToSymbolPool();
      num = rotateBytes(num);
      sectionTable[currentSectionNum]->pool.push_back(num);
      pushInstructionsByByte(3,10,pc,reg1,reg2,offset); 
    }
    else{
      pushInstructionsByByte(3,2,0,reg1,reg2,num);
    }
  }
  locationCounter += 4;
}

void Assembler::bneSym(int reg1, int reg2, string symbol)
{
  if (!first)
  {
    int offset = getRelativeOffsetToSymbolPool();
    int symOffset = getOffsetToSymbolPool();

    if(isLocal(symbol)){
      Relocation *r = new Relocation(currentSection, symOffset, REL, getSymbolNum(currentSection), getSymOffset(symbol));
      relocationTable.push_back(r);
    }
    else{
      Relocation* r = new Relocation(currentSection, symOffset, ABS, getSymbolNum(symbol), 0);
      relocationTable.push_back(r);
    }

    sectionTable[currentSectionNum]->pool.push_back(0);
    pushInstructionsByByte(3,10,pc,reg1,reg2,offset);
  }
   locationCounter += 4;
}

void Assembler::bgtNum(int reg1, int reg2, int num)
{
  if (!first)
  {
    cout << "REG1: " << reg1 << ", REG2: "<<reg2 <<endl;
    if(isLargerThan12Bits(num)){
      int offset = getRelativeOffsetToSymbolPool();
      num = rotateBytes(num);
      sectionTable[currentSectionNum]->pool.push_back(num);
      pushInstructionsByByte(3,11,pc,reg1,reg2,offset); 
    }
    else{
      pushInstructionsByByte(3,3,0,reg1,reg2,num);
    }
  }
  locationCounter += 4;
}

void Assembler::bgtSym(int reg1, int reg2, string symbol)
{
  if(!first)
  {
    int offset = getRelativeOffsetToSymbolPool();
    int symOffset = getOffsetToSymbolPool();

    if(isLocal(symbol)){
      Relocation *r = new Relocation(currentSection, symOffset, REL, getSymbolNum(currentSection), getSymOffset(symbol));
      relocationTable.push_back(r);
    }
    else{
      Relocation* r = new Relocation(currentSection, symOffset, ABS, getSymbolNum(symbol), 0);
      relocationTable.push_back(r);
    }

    sectionTable[currentSectionNum]->pool.push_back(0);
    pushInstructionsByByte(3,11,pc,reg1,reg2,offset);
  }
   locationCounter += 4;
}

void Assembler::push(int reg)
{
  if (!first)
    pushInstructionsByByte(8,1,sp,0,reg,-4);

  locationCounter += 4;
}

void Assembler::pop(int reg)
{
  if(!first)
    pushInstructionsByByte(9,3,reg,sp,0,4);
  
  locationCounter += 4;
}

void Assembler::setInstructionContext(unsigned char op, int regS, int regD)
{
  unsigned char valD = (regD & 0x0F) << 4 | (regD & 0x0F);
  unsigned char valS = (regS & 0x0F) << 4;
  sectionTable[currentSectionNum]->content->push_back(op);
  sectionTable[currentSectionNum]->content->push_back(valD);
  sectionTable[currentSectionNum]->content->push_back(valS);
  sectionTable[currentSectionNum]->content->push_back(0);
}

void Assembler::xchg(int regS, int regD)
{
  if(!first)
  { 
    sectionTable[currentSectionNum]->content->push_back(0x40);
    sectionTable[currentSectionNum]->content->push_back(regS & 0x0F);
    sectionTable[currentSectionNum]->content->push_back((regD & 0x0F) << 4);
    sectionTable[currentSectionNum]->content->push_back(0);
  }
  locationCounter += 4;
}

void Assembler::add(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x50, regS, regD);
  
  locationCounter += 4;
}

void Assembler::sub(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x51, regS, regD);
  
  locationCounter += 4;
}

void Assembler::mul(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x52, regS, regD);
  
  locationCounter += 4;
}

void Assembler::div(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x53, regS, regD);
  
  locationCounter += 4;
}

void Assembler::notF(int reg)
{
  if(!first)
    setInstructionContext(0x60, 0, reg);
  
  locationCounter += 4;
}

void Assembler::andF(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x61, regS, regD);
  
  locationCounter += 4;
}

void Assembler::orF(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x62, regS, regD);
  
  locationCounter += 4;
}

void Assembler::xorF(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x63, regS, regD);
  
  locationCounter += 4;
}

void Assembler::shl(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x70, regS, regD);
  
  locationCounter += 4;
}

void Assembler::shr(int regS, int regD)
{
  if(!first)
    setInstructionContext(0x71, regS, regD);
  
  locationCounter += 4;
}

bool Assembler::isLargerThan12Bits(int value) {
  int maxValue = (1 << 11) - 1; // Maximum value that can be represented using 12 bits
  int minValue = -(1 << 11);    // Minimum value that can be represented using 12 bits
  return (value > maxValue) || (value < minValue);
}

void Assembler::ldNum(int num, int regD)
{
 if(!first){
    if(isLargerThan12Bits(num)){

      int offset = getRelativeOffsetToSymbolPool();
     
     num = rotateBytes(num);
      sectionTable[currentSectionNum]->pool.push_back(num);
      pushInstructionsByByte(9,2,regD,0,pc,offset);
    }
    else{
      pushInstructionsByByte(9,1,regD,0,0,num);
    }
  }
  locationCounter += 4;
}

void Assembler::ldSym(string symbol, int regD)
{
  if(!first){

    int offset = getRelativeOffsetToSymbolPool();
    int symOffset = getOffsetToSymbolPool();

    if(isLocal(symbol)){
      Relocation *r = new Relocation(currentSection, symOffset, REL, getSymbolNum(currentSection), getSymOffset(symbol));
      relocationTable.push_back(r);
    }
    else{
      Relocation* r = new Relocation(currentSection, symOffset, ABS, getSymbolNum(symbol), 0);
      relocationTable.push_back(r);
    }

    sectionTable[currentSectionNum]->pool.push_back(0);
    pushInstructionsByByte(9,2,regD,0,pc,offset);

  }
  locationCounter += 4;
}

void Assembler::ldMemNum(int num, int regD){

  if(!first){
    int offset = getRelativeOffsetToSymbolPool();
    num = rotateBytes(num);
    sectionTable[currentSectionNum]->pool.push_back(num);
    pushInstructionsByByte(9,2,regD,pc,0,offset);
    pushInstructionsByByte(9,2,regD,regD,0,0);
  }
  locationCounter += 8;
}

void Assembler::ldMemSym(string symbol, int regD){
  if(first){
    locationCounter += 8;
  }
  else{
    ldSym(symbol, regD);
    pushInstructionsByByte(9,2,regD,regD,0,0);
    locationCounter += 4;
  }
}

void Assembler::ldReg(int regS, int regD)
{
  if(!first)
    pushInstructionsByByte(9,1,regD,regS,0,0);
  
  locationCounter += 4;
 
}

void Assembler::ldMemReg(int regS, int regD)
{
  if(!first)
    pushInstructionsByByte(9,2,regD,regS,0,0);
  
  locationCounter += 4;
}

void Assembler::ldMemRegDispNum(int regS, int numDisp, int regD)
{
  if(!first){
    if(isLargerThan12Bits(numDisp)){
      int offset = getRelativeOffsetToSymbolPool();
      numDisp = rotateBytes(numDisp);
      sectionTable[currentSectionNum]->pool.push_back(numDisp);
      pushInstructionsByByte(9,2,regD,regS,pc,offset);
    }
    else{
      pushInstructionsByByte(9,2,regD,regS,0,numDisp);
    }
  }
  locationCounter += 4;
}

void Assembler::stMemNum(int regS, int num)
{
  if(!first)
  {
      if(isLargerThan12Bits(num)){
        int offset = getRelativeOffsetToSymbolPool();
        num = rotateBytes(num);
        sectionTable[currentSectionNum]->pool.push_back(num);
        pushInstructionsByByte(8,2,pc,0,regS, offset);
      }
      else{
        pushInstructionsByByte(8,0,0,0,regS,num);
      }
 
  }
  locationCounter += 4;
}

void Assembler::stMemSym(int regS, string symbol)
{
  if(!first){
    int offset = getRelativeOffsetToSymbolPool();
    int symOffset = getOffsetToSymbolPool();

    if(isLocal(symbol)){
      Relocation *r = new Relocation(currentSection, symOffset, REL, getSymbolNum(currentSection), getSymOffset(symbol));
      relocationTable.push_back(r);
    }
    else{
      Relocation* r = new Relocation(currentSection, symOffset, ABS, getSymbolNum(symbol), 0);
      relocationTable.push_back(r);
    }
    sectionTable[currentSectionNum]->pool.push_back(0);
    pushInstructionsByByte(8,2,pc,0,regS, offset);
  }
  locationCounter += 4;
}

void Assembler::stMemReg(int regS, int regD)
{
  if(!first)
    pushInstructionsByByte(8, 0,0,regD, regS,0);
  
   locationCounter += 4;
}

void Assembler::stMemRegDispNum(int regS, int regD, int dispNum)
{
  if(!first){
    if(isLargerThan12Bits(dispNum))
        cout << "Error dispatch larger than 12 bits" << endl;
    else
      pushInstructionsByByte(8, 0,0,regD, regS,dispNum);
  }
  locationCounter += 4;
}

void Assembler::csrrd(int csr, int reg)
{
  if(!first)
    pushInstructionsByByte(9,0,reg,csr,0,0);

  locationCounter += 4;
}

void Assembler::csrwr(int csr, int reg)
{
  if(!first)
    pushInstructionsByByte(9,4,csr,reg,0,0);

  locationCounter += 4;
}

void Assembler::end()
{
  if(!first){
    
    if(currentSectionNum != -1){
      if(!sectionTable[currentSectionNum]->pool.empty()){
        
        int poolSize = sectionTable[currentSectionNum]->pool.size()*4;
        
        pushInstructionsByByte(3,0,pc,0,0,poolSize);

        sectionTable[currentSectionNum]->size += 4;
      }
    }
    
  }
}

void yyerror(const char *s)
{
  cout << "EEK, parse error!  Message: " << s << endl;
  // might as well halt now:
  exit(-1);
}