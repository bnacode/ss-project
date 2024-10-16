#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <queue>
using namespace std;
#include "../inc/common.hpp"

void printRealocationTable(vector<Relocation*> *relocationTable)
{
  std::cout << "Relocation Table:" << std::endl;
  std::cout << std::setw(25) << setfill(' ') <<std::left << "Section"
            << std::setw(25) << setfill(' ') <<std::left << "Offset"
            << std::setw(25) << setfill(' ') <<std::left << "Relocation"
            << std::setw(25) << setfill(' ') <<std::left << "SymNum"
            << std::setw(25) << setfill(' ') <<std::left << "Addend"
            << std::endl;

  for (const Relocation* relocation : *relocationTable)
  {
    string relType;
    if (relocation->realocation == REL)
      relType = "REL";
    else
      relType = "ABS";

    std::cout << std::setw(25) << setfill(' ') << std::left << relocation->section
              << std::setw(25) << setfill(' ') << std::left << relocation->offset
              << std::setw(25) << setfill(' ') << std::left << relType
              << std::setw(25) << setfill(' ') << std::left << relocation->symNum
              << std::setw(25) << setfill(' ') << std::left << relocation->addend
              << std::endl;
  }
}

void printSymbolTable(vector<Symbol*> *symbolTable)
{
  std::cout << "Symbol Table:" << std::endl;
  std::cout << std::setw(25) << setfill(' ') << std::left << "Label"
            << std::setw(25) << setfill(' ') << std::left << "Section"
            << std::setw(25) << setfill(' ') << std::left << "Offset"
            << std::setw(25) << setfill(' ') << std::left << "Glocal"
            << std::setw(10) << setfill(' ') << std::left << "Num"
            << std::setw(10) << setfill(' ') << std::left << "Extern"
            << std::setw(10) << setfill(' ') << std::left << "IsSection"
            << std::endl;

  for (Symbol *symbol : *symbolTable)
  {
    if(symbol->glocal == global){
      string glocal;
    if (symbol->glocal == Glocal::global)
      glocal = "global";
    else
      glocal = "local";

    int isSection;
    if(symbol->isSection)
      isSection = 1;
    else
      isSection =  0;

    int externs;
    if (symbol->isExtern)
      externs = 1;
    else
      externs = 0;

    std::cout << std::setw(25) << setfill(' ') << std::left << symbol->label
              << std::setw(25) << setfill(' ') << std::left << symbol->section
              << std::setw(30) << setfill(' ') << std::left << symbol->offset
              << std::setw(25) << setfill(' ') << std::left << glocal
              << std::setw(10) << setfill(' ') << std::left << symbol->num
              << std::setw(10) << setfill(' ') << std::left << externs
              << std::setw(10) << setfill(' ') << std::left << isSection
              << std::endl;
  }
    }
    
}

void printPool(vector<int>* pool, bool endLine) {
    const int intsPerRow = 2;

    if (endLine && !pool->empty()) {
        int firstValue = (*pool)[0];
        for (int j = sizeof(int) - 1; j >= 0; j--) {
            unsigned char byte = static_cast<unsigned char>((firstValue >> (j * 8)) & 0xFF);
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(byte);
            if (j > 0) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }

    for (std::size_t i = endLine ? 1 : 0; i < pool->size(); i += intsPerRow) {
        for (int k = 0; k < intsPerRow; k++) {
            if (i + k >= pool->size()) {
                break;  // Break if no more integers remaining in the pool
            }

            int value = (*pool)[i + k];
            for (int j = sizeof(int) - 1; j >= 0; j--) {
                unsigned char byte = static_cast<unsigned char>((value >> (j * 8)) & 0xFF);
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(byte);
                if (j > 0) {
                    std::cout << " ";
                }
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}


void printContent(vector<char>* content) {
  int bytesPerRow = 8;
  int contentSize = content->size();
  for (int i = 0; i < contentSize; i++) {
    if (i % bytesPerRow == 0 && i != 0)
      std::cout << std::endl;
    std::cout << std::hex << std::setw(2) << std::setfill('0') << right
              << static_cast<int>(static_cast<unsigned char>((*content)[i]) & 0xFF) << " ";
  }
}

void printSectionTable(vector<Section*> *sectionTable)
{
  std::cout << "Section Table:" << std::endl;
  std::cout << std::left << std::setw(10) << "Section"
            << std::setw(10) << "Offset"
            << std::setw(10) << "Size"
            << std::endl;

  for (Section *section : *sectionTable)
  {
    std::cout << std::left << std::setw(10) << section->section
              << std::setw(10)<<setfill(' ') << section->offset
              << std::setw(10)<<setfill(' ') << section->size
              << endl;
  }
   
}
  