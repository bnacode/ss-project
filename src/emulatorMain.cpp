#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <vector>
#include "../inc/emulator.hpp"
using namespace std;


void parseFile(const std::string& filename, std::unordered_map<unsigned int,  char>& parsedData) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line) && !line.empty()) {
        std::istringstream iss(line);
        unsigned int firstInt;
        unsigned char colon;
        std::vector<unsigned char> bytes(8);

        if (iss >> std::hex >> firstInt >> colon) {
            for (unsigned char& byte : bytes) {
                std::string byteStr;
                if (!(iss >> byteStr)) {
                    std::cerr << "Error parsing line: " << line << std::endl;
                    break;
                }

                // Convert the byte string to an integer
                std::stringstream ss(byteStr);
                int byteInt;
                ss >> std::hex >> byteInt;
                byte = static_cast<unsigned char>(byteInt);
            }

            for (unsigned int i = 0; i < 8; ++i) {
                parsedData[firstInt + i] = bytes[i];
            }
        }
    }

    file.close();
}



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Please provide the filename as an argument." << std::endl;
        return 1;
    }

    parseFile(argv[1], Emulator::memory);

    std::cout << "Memory size: " << Emulator::memory.size() << std::endl;
    
    const char* outputFileName = "emulator.txt";
    std::ofstream outputFile(outputFileName);
    
    if (!outputFile) {
        std::cout << "Failed to open output file: " << outputFileName << std::endl;
        return -1;
    }

     std::streambuf* originalBuffer = std::cout.rdbuf(outputFile.rdbuf());

    for (const auto& pair : Emulator::memory) {
    std::cout << "Key: " << std::hex << pair.first
              << ", Value: " << std::setw(2) << std::setfill('0')
              << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(pair.second))
              << std::endl;
    }

    std::cout.rdbuf(originalBuffer);

    outputFile.close();


    Emulator::initRegisters();

    Emulator::decodeInstructions();

    cout << "Emulated processor state:" << endl;

    for (int i = 0; i < 16; i++) {
        std::cout << "r" << i << "=" << "0x" << std::hex << std::setw(8) << std::setfill('0') << Emulator::reg[i] << " ";
        if (i % 4 == 3)
            std::cout << std::endl;
    }
    cout << endl;

    return 0;
}
