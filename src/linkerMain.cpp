#include "../inc/linker.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


LinkerArguments parseLinkerCommand(int argc, char* argv[]) {
    LinkerArguments linkerArgs;
    linkerArgs.isHex = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-o") {
            // Extract the output file name
            if (i + 1 < argc) {
                linkerArgs.outputFileName = argv[i + 1];
                i++; // Skip the next argument
            }
        } else if (arg.find("-place=") == 0) {
            // Extract section name and address
            std::string placeOption = arg.substr(7);
            size_t atPos = placeOption.find('@');

            if (atPos != std::string::npos) {
                std::string sectionName = placeOption.substr(0, atPos);
                std::string addressStr = placeOption.substr(atPos + 1);

                // Check if the address is in hexadecimal format
                bool isHex = (addressStr.compare(0, 2, "0x") == 0);

                // Remove the "0x" prefix if present
                if (isHex) {
                    addressStr = addressStr.substr(2);
                }

                // Convert address string to integer
                int address;
                if (isHex) {
                    address = std::stoul(addressStr, nullptr, 16);
                } else {
                    address = std::stoul(addressStr);
                }

                // Store section name and address in the hashmap
                linkerArgs.specialSections[sectionName] = address;
            }
        } else if (arg == "-hex") {
            linkerArgs.isHex = true;
        } else {
            // Assume it's an input file
            linkerArgs.inputFiles.push_back(arg);
        }
    }

    return linkerArgs;
}



int main(int argc, char* argv[]) {


  LinkerArguments linkerArgs = parseLinkerCommand(argc, argv);
    
  string outputFileName = linkerArgs.outputFileName;
    
  Linker::insertSpecialSections(&linkerArgs.specialSections);
   
  for (const std::string& inputFile : linkerArgs.inputFiles) {
      
          Linker::parseInputFile(inputFile);

          Linker::mergeSections();

          Linker::mergeSymbolTable();

          Linker:: mergeRelocations();

      
          Linker::symbolTableLocal.clear();
          Linker::relocationTableLocal.clear();
          Linker::sectionTableLocal.clear();     
    
  }
    

  Linker::manageRelocations();
  //Linker::printGlobalTables(linkerArgs.outputFileName.c_str());

  Linker::makeOutput(outputFileName.c_str());

}