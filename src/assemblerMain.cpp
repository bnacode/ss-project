#include "../inc/assembler.hpp"
#include <iostream>
#include <fstream>


using namespace std;

extern FILE *yyin;

int main(int argc, char* argv[]) {

  if (argc < 4 || std::string(argv[1]) != "-o") {
    std::cout << "Usage: ./assembler -o output_file input_file" << std::endl;
    return 1;
  }

  const char* outputFileName = argv[2];
  const char* inputFileName = argv[3];

  FILE* myfile = fopen(inputFileName, "r");
 
  if (!myfile) {
    std::cout << "Failed to open input file: " << inputFileName << std::endl;
    return 1;
  }

  // Set flex to read from it instead of defaulting to STDIN:
  yyin = myfile;

  // Parse through the input:
  yyparse();

  //Reset and prepare for second pass
  if (Assembler::first) {
    Assembler::setCurrentSectionSize();
    Assembler::first = false;
    Assembler::currentSection = "undefined";
    Assembler::locationCounter = 0;
    Assembler::currentSectionNum = -1;

    fseek(myfile, 0, SEEK_SET);
    yyin = myfile;
    yyparse();
  

    // Open the output file for writing:
    std::ofstream outputFile(outputFileName);
    // Make sure it's valid:
    if (!outputFile) {
      std::cout << "Failed to open output file: " << outputFileName << std::endl;
      return -1;
    }
    // Redirect the output to the file:
    std::streambuf* originalBuffer = std::cout.rdbuf(outputFile.rdbuf());

    
    printSymbolTable(&Assembler::symbolTable);
    std::cout << std::endl;
    // Assembler::printSectionTable(&Assembler::sectionTable);
    // std::cout << std::endl;
 
    printRealocationTable(&Assembler::relocationTable);
    std::cout << std::endl;

    for (Section* section : Assembler::sectionTable) {
      std::cout << "Section:" << section->section << std::endl;
      printContent(section->content);
      bool endLine;
      if (section->size % 8 != 0) {
        endLine = true;
      } else {
        endLine = false;
        std::cout << std::endl;
      }
      printPool(&section->pool, endLine);
      std::cout << std::endl << std::endl;
    }

    // Restore the original cout buffer
    std::cout.rdbuf(originalBuffer);

    // Close the output file
    outputFile.close();
  }
}
