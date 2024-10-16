#include <iostream>
using namespace std;

#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include<unordered_map>
#include "../inc/common.hpp"



struct LinkerArguments {
    std::string outputFileName;
    std::unordered_map<std::string, unsigned int> specialSections;
    std::vector<std::string> inputFiles;
    bool isHex;
};


class Linker{
public:

  static int startAddr;
  static int symNum;

  static void printGlobalTables(const char*);

  static bool sortByValue(const std::pair<std::string, unsigned int>& a, const std::pair<std::string, unsigned int>& b);
  static void insertSpecialSections(unordered_map<string, unsigned int>* map);
  static void changeOffsetInRelocations(string section, int size);
  static void changeSymbolNumInRelocations(unsigned int newNum, unsigned int oldNum);
  static bool isInSectionTable(string section, unsigned int*row);
  static bool isInSymbolTable(string symbol, int* row);
  static void mergeSections();
  static void mergeSymbolTable();
  static void mergeRelocations();

  static void manageRelocations();

  static void makeOutput(const char* filename);
  static void printOutputFile(vector<char>* content, int);
  static vector<Symbol*> symbolTableLocal;
  static vector<Relocation*> relocationTableLocal;
  static vector<Section*> sectionTableLocal;

  static void clearVectors();
  static vector<Symbol*> symbolTableGlobal;
  static vector<Relocation*> relocationTableGlobal;
  static vector<Section*> sectionTableGlobal;

  static void parseInputFile(const std::string& filename);
  
};








