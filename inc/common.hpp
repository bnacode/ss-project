#include <iostream>
using namespace std;

#include <vector>
#include <list>
#include <queue>

#include<unordered_map>

typedef string SectionType;

enum Glocal{
  local,
  global
};

typedef string SectionType;

enum RealocationType{
  REL,
  ABS
};


struct Symbol{

  string label;
  SectionType section;
  unsigned int offset;
  Glocal glocal;
  bool isExtern;
  unsigned int num;
  bool isSection;



  Symbol(string label, SectionType section,unsigned int offset, Glocal g,unsigned int num, bool e = false, bool s = false){
      this->label = label;
      this->section = section;
      this-> offset = offset;
      this->glocal = g;
      this->num = num;
      this->isExtern = e;
      this->isSection = s;
  }

};

struct Relocation
{
  SectionType section;
  unsigned int offset;
  RealocationType realocation;
  unsigned int symNum;
  unsigned int addend;
  bool changed = false;

  Relocation(SectionType section,unsigned int offset, RealocationType realocation,unsigned int num,unsigned int addend){
    this->section = section;
    this->offset = offset;
    this->realocation = realocation;
    this->symNum = num;
    this->addend = addend;
  }
};

struct Section{

  SectionType section;
  unsigned int offset = 0;
  vector<char> *content;
  vector<int> pool;
  unsigned int size = 0;
  bool special = false;
  
  Section(SectionType section){
    this->section = section;
    offset = 0;
    content = new vector<char>();
    pool = vector<int>();
  }

};

void printAll();
void printContent(vector<char>* content);
void printPool(vector<int>* pool, bool);
void printSectionTable(vector<Section*> *sectionTable);
void printSymbolTable(vector<Symbol*> *symbolTable);
void printRealocationTable(vector<Relocation*> *relocationTable);

