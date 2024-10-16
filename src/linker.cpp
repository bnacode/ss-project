
#include "../inc/linker.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip> 
#include <unordered_map>
#include <algorithm>

int Linker:: symNum = 1;
int Linker:: startAddr = 0x40000000;

vector<Symbol*> Linker::symbolTableLocal = vector<Symbol*>();
vector<Relocation*> Linker:: relocationTableLocal = vector<Relocation*>();
vector<Section*> Linker::sectionTableLocal = vector<Section*>();

vector<Symbol*> Linker::symbolTableGlobal = vector<Symbol*>();
vector<Relocation*> Linker::relocationTableGlobal = vector<Relocation*>();
vector<Section*> Linker::sectionTableGlobal = vector<Section*>();

void Linker::parseInputFile(const string& filename) {

    ifstream inputFile(filename);
    if (!inputFile) {
        cout << "Failed to open input file from parser." << endl;
        return;
    }

    string line;
    while (getline(inputFile, line)) {
        if (line == "Symbol Table:") {
            getline(inputFile, line);
            
            while (getline(inputFile, line) && !line.empty()) {
                istringstream iss(line);
                string label, sectionName, offsetStr, glocalStr, numStr, externStr, sectionStr;
                iss >> label >> sectionName >> offsetStr >> glocalStr >> numStr >> externStr >> sectionStr;

                unsigned int offset = stoi(offsetStr);
                
                Glocal glocal = (glocalStr == "global") ? Glocal::global : Glocal::local;
                int num = stoi(numStr);

                bool isExtern = (externStr == "1");
                bool isSection = (sectionStr == "1");

                if(label == "my_start"){
                    //cout << "### " << offset << endl;
                }
                Symbol *s = new Symbol(label, sectionName, offset, glocal, num, isExtern, isSection);
                Linker::symbolTableLocal.push_back(s);
            }
        
        }
        else if (line == "Relocation Table:") {
            getline(inputFile, line);
            
            while (getline(inputFile, line) && !line.empty()) {
                istringstream iss(line);
                string sectionName, offsetStr, relocationStr, symNumStr, addendStr;
                iss >> sectionName >> offsetStr >> relocationStr >> symNumStr >> addendStr;

                int offset = stoi(offsetStr);
                RealocationType relocation = (relocationStr == "ABS") ? ABS : REL;
                int symNum = stoi(symNumStr);
                int addend = stoi(addendStr);

                Relocation *r = new Relocation(sectionName, offset, relocation, symNum, addend);
                relocationTableLocal.push_back(r);
            }
        }
        else if (line.substr(0, 8) == "Section:") {
          
            string sectionName = line.substr(8);

            Section *section = new Section(sectionName);

            while (getline(inputFile, line) && !line.empty()) {
                istringstream contentIss(line);
                string byteStr;
                while (contentIss >> byteStr) {
                    try {
                        int byte = stoi(byteStr, nullptr, 16);
                        section->content->push_back(static_cast<char>(byte));
                    } catch (const invalid_argument& e) {
                        cerr << "Invalid argument: " << e.what() << endl;
                        // Handle the error accordingly
                    } catch (const out_of_range& e) {
                        cerr << "Out of range: " << e.what() << endl;
                        // Handle the error accordingly
                    }
                }
            }
        

    sectionTableLocal.push_back(section);
}

    }

    inputFile.close();
}

bool Linker::isInSymbolTable(string symbol, int* row){
    int iteration = 0;
    for (Symbol* s : symbolTableGlobal) {
            if (s->label == symbol) {
                *row = iteration;
                return true; // Symbol found in the symbol table
            }
        iteration++;
    }
    return false; // Symbol not found in the symbol table
}


void Linker::mergeSymbolTable(){
    int row;
    for(Symbol* s : symbolTableLocal){
        if(isInSymbolTable(s->label, &row)){
            if(s->isSection){
                changeSymbolNumInRelocations(s->num, symbolTableGlobal[row]->num);
            }
            else{
                Symbol* symbolFromTable = symbolTableGlobal[row];

                if(symbolFromTable->isExtern && !s->isExtern){
                   symbolFromTable->isExtern = false;
                   symbolFromTable->offset = s->offset;
                   symbolFromTable->section = s->section;

                   changeSymbolNumInRelocations( s->num,  symbolFromTable->num);
                }
                else if(!symbolFromTable->isExtern && !s->isExtern){
                    {
                        cout << "Dvostruko definisan simbol: " << s->label << endl;
                        return;
                    }
                }
                else if(!symbolFromTable->isExtern && s->isExtern){
                    //cout << "pronadjen simbol num " << s->num  << s->label << " na poziciji " << symbolFromTable->num << endl;
                    changeSymbolNumInRelocations( s->num,  symbolFromTable->num);
                }
                else if(symbolFromTable->isExtern && s->isExtern){
                    changeSymbolNumInRelocations( s->num,  symbolFromTable->num);
                }
            }
        }
        else{
            changeSymbolNumInRelocations(s->num, symNum);
            s->num = symNum++;
            symbolTableGlobal.push_back(s);
        }
    }

}

void Linker::mergeRelocations(){
    for(Relocation* r : relocationTableLocal){
        relocationTableGlobal.push_back(r);
    }
}

bool Linker::isInSectionTable(string section, unsigned int*row){
    int i = 0;
    for(Section* s : sectionTableGlobal){
        if(s->section == section){
            *row = i;
            return true;
        }
        i++;
    }
    return false;
}

void Linker::changeOffsetInRelocations(string section, int size){
    for(Relocation *r : relocationTableLocal){
        if(r->section == section){
            r->offset += size;
        }
    }
}

void Linker::changeSymbolNumInRelocations(unsigned int oldNum, unsigned int newNum){

    for(Relocation* r : relocationTableLocal){
        if(r->symNum == oldNum && !r->changed){
            r->symNum = newNum;
            r->changed = true;
        }
    }
}

void changeOffsetInSymbolTable(string section, int size){
    for(Symbol* s : Linker::symbolTableLocal){
        if(s->section == section)
            s->offset += size;
    }
}

void Linker:: mergeSections(){
    unsigned int row;
    for(Section* s : sectionTableLocal){
        if(isInSectionTable(s->section, &row)){
            if(sectionTableGlobal.size() > row + 1){
                if(sectionTableGlobal[row+1]->special){
                    //Ako ne moze da se uglavi pre sledece specijalne prijavi gresku
                    if(sectionTableGlobal[row]->content->size()+ sectionTableGlobal[row]->offset + s->content->size() >  sectionTableGlobal[row+1]->offset){
                        cout << "Greska specijalne sekcije " << s->section << endl;
                        return;
                    } 
                }
    
            }
            changeOffsetInRelocations(s->section, sectionTableGlobal[row]->content->size());
            changeOffsetInSymbolTable(s->section, sectionTableGlobal[row]->content->size());
            sectionTableGlobal[row]->content->insert(sectionTableGlobal[row]->content->end(), s->content->begin(), s->content->end());
            sectionTableGlobal[row]->size += s->content->size();
        }
        else{
            if(sectionTableGlobal.size()==0){
                s->offset = startAddr;
                s->size = s->content->size();               
            }
            else{
                int sizeOfSectionBefore = sectionTableGlobal.back()->content->size() + sectionTableGlobal.back()->offset;
                s->offset = sizeOfSectionBefore;
                //changeOffsetInSymbolTable(s->section, sizeOfSectionBefore);
            }
            sectionTableGlobal.push_back(s);      
        }

    }
}

void Linker::printGlobalTables(const char* outputFileName){

    std::ofstream outputFile(outputFileName);
    
    if (!outputFile) {
        std::cout << "Failed to open output file: " << outputFileName << std::endl;
        return;
    }

    // Redirect the output to the file:
    std::streambuf* originalBuffer = std::cout.rdbuf(outputFile.rdbuf());

    //printSymbolTable(&Linker::symbolTableGlobal);
    std::cout << std::endl;
    // Assembler::printSectionTable();
    // std::cout << std::endl;

   // printRealocationTable(&Linker::relocationTableGlobal);
    std::cout << std::endl;

    for (Section* section : Linker::sectionTableGlobal) {
        std::cout << section->section << ": " << section->offset << std::endl;
        printContent(section->content);
        cout << endl<< endl;
    
    }
    

  std::cout.rdbuf(originalBuffer);
  outputFile.close();
}

Section* findSection(string name){
    for(Section* s : Linker::sectionTableGlobal){
        if(s->section == name){
            return s;
        }
    }
    return 0;
}

Symbol* findSymbol(unsigned int num){
    for(Symbol* s : Linker::symbolTableGlobal){
        if(s->num == num)
            return s;
    }
    return 0;
}

void Linker::manageRelocations(){

    //Fixing section offset
    for (std::size_t i = 0; i < sectionTableGlobal.size() - 1; ++i) {
        Section* currentSection = sectionTableGlobal[i];
        Section* nextSection = sectionTableGlobal[i + 1];
        
        if (currentSection->offset + currentSection->content->size() > nextSection->offset) {
            nextSection->offset = currentSection->offset + currentSection->content->size();
        }  
    }
    
    for(Section* sec : sectionTableGlobal){
        for(Symbol* sym : symbolTableGlobal){
            if(sym->section == sec->section){
               sym->offset += sec->offset;
            }
        }
    }
    
    for(Relocation* r : relocationTableGlobal){
        int symNum = r->symNum;
        Symbol* s = findSymbol(symNum);
        unsigned int value = s->offset + r->addend;

        Section* sec = findSection(r->section);

        unsigned char* bytePtr = reinterpret_cast<unsigned char*>(&value);

        for (unsigned int i = 0; i < sizeof(unsigned int); i++) {
            sec->content->at(r->offset + i) = bytePtr[i];
        }
    } 
}


void Linker::printOutputFile(std::vector<char>* content, int offset) {
    int bytesPerRow = 8;
    int contentSize = content->size();

    for (int i = 0; i < contentSize; i++) {
        if (i % bytesPerRow == 0) {
            std::cout << std::hex << std::setw(8) << std::setfill('0') << offset << " : ";
            offset += bytesPerRow;
        }

        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>((*content)[i]) & 0xFF) << " ";

        if ((i + 1) % bytesPerRow == 0) {
            std::cout << std::endl;
        }
    }
}

bool Linker::sortByValue(const std::pair<std::string, unsigned int>& a, const std::pair<std::string, unsigned int>& b) {
    return a.second < b.second;
}

void Linker:: insertSpecialSections(unordered_map<string,unsigned int>* map){

    std::vector<std::pair<std::string, int>> vector(map->begin(), map->end());

    // Sort the vector by value using the custom comparison function
    std::sort(vector.begin(), vector.end(), sortByValue);

    for (const auto& entry : vector) {
        Section *s= new Section(entry.first);
        s->offset = entry.second;
        s->special = true;
        sectionTableGlobal.push_back(s);
    }

    startAddr = vector.back().second;

}

void Linker::clearVectors(){
    for (std::size_t i = 0; i < symbolTableLocal.size(); i++) {
        delete symbolTableLocal[i];
    }
        for (std::size_t i = 0; i < relocationTableLocal.size(); i++) {
        delete relocationTableLocal[i];
    }
        for (std::size_t i = 0; i < sectionTableLocal.size(); i++) {
        delete sectionTableLocal[i];
    }
    
}

void Linker::makeOutput(const char* outputFileName){
    
    std::ofstream outputFile(outputFileName);
    
    if (!outputFile) {
        std::cout << "Failed to open output file: " << outputFileName << std::endl;
        return;
    }

    // Redirect the output to the file:
    std::streambuf* originalBuffer = std::cout.rdbuf(outputFile.rdbuf());

    unsigned int size = 0;
    for(Section* s : Linker::sectionTableGlobal){
        if(size % 8 != 0){
            if(size == s->offset){
                for(int i = 0; i < 4; i ++){
                    std::cout << std::hex << std::setw(2) << std::setfill('0')<< static_cast<int>(static_cast<unsigned char>((s->content->at(i))))<< " ";
                }
                s->content->erase(s->content->begin(), s->content->begin() +4);
                s->offset += 4;
                cout <<endl;
            }
            else{
                std::cout << std::hex << std::setw(8) << std::setfill('0') << 0 ;
            }
        }
        printOutputFile(s->content, s->offset);
        size = s->offset + s->content->size();
    }

    std::cout.rdbuf(originalBuffer);
    outputFile.close();
}

