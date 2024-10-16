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

int Emulator:: reg[16];
int Emulator:: csr[3];
int Emulator:: temp;

#define pc reg[15]
#define sp reg[14]
#define status  csr[0]
#define handler csr[1]
#define cause   csr[2]

unsigned char Emulator:: instruction[4];


std::unordered_map<unsigned int,  char> Emulator::memory;

void Emulator:: initRegisters(){
    for(int i = 0; i < 15; i++)
        reg[i] = 0;

    pc = 0x40000000;

    status = 0;
    cause = 0;
    handler = 0;

    temp = 0;
}

void Emulator::push(int val){
    sp = sp - 4;
    storeToMemory(0,0,0, sp, val);
}

void Emulator::halt(){
    end = true;
}

void Emulator::intF(){
    push(status);
    push(pc);
    cause = 4;
    status = status&(~0x1);
    pc = handler;
}

void Emulator::call(char M, char A, char B, int D){

    if(M == 1){
        push(pc);
        pc = loadFromMemory(A, B,0,D);  
    }
    else{
        push(pc);
        pc = reg[A] + reg[B] + D;
    }
}

void Emulator::jmp(char M, char A,char B, char C, int D){

    switch(M){
        case 0: 
        {
            pc = reg[A] + D;
            break;
        }
        case 1: 
        {
            if(reg[B] == reg[C])
                pc = reg[A] + D;
            break;
        }
        case 2: 
        {
            if(reg[B] != reg[C])
                pc = reg[A] + D;
            break;
        }
        case 3: 
        {
            if(reg[B] > reg[C])
                pc = reg[A] + D;
            break;
        }
        case 8: 
        {
            pc = loadFromMemory(A,0,0,D);
            break;
        }
        case 9: 
        {
            if(reg[B] == reg[C])
                pc = loadFromMemory(A,0,0,D);
            break;
        }
        case 10: 
        {
            if(reg[B] != reg[C])
                pc = loadFromMemory(A,0,0,D);
            break;
        }
         case 11: 
        {
            if(reg[B] > reg[C])
                pc = loadFromMemory(A,0,0,D);
            break;
        }

    }
}

void Emulator::xchg(char B, char C){
    temp = reg[B];
    reg[B] = reg[C];
    reg[C] = temp;
}

void Emulator::aritmetic(char M, char A, char B, char C){
    
    switch(M){

        case 0:
        {
            reg[A] = reg[B] + reg[C];
            break;
        }
        case 1:
        {
            reg[A] = reg[B] - reg[C];
            break;
        }
        case 2:
        {
            reg[A] = reg[B] * reg[C];
            break;
        }
        case 3:
        {
            reg[A] = reg[B] / reg[C];
            break;
        }
    }
}

void Emulator::logic(char M, char A, char B, char C){

    switch(M){
        case 0:
        {
            reg[A] = ~reg[B];
            break;
        }
        case 1:
        {
            reg[A] = reg[B] & reg[C];
            break;
        }
        case 2:
        {
            reg[A] = reg[B] | reg[C];
            break;
        }
        case 3:
        {
            reg[A] = reg[B] ^ reg[C];
            break;
        }
    }
}

void Emulator::shift(char M, char A, char B, char C){
    if(M == 0){
        reg[A] = reg[B] << reg[C];
    }
    else{
        reg[A] = reg[B] >> reg[C];
    }
}



void Emulator::load(char M, char A, char B, char C, int D){

       switch(M){
        case 0: 
        {
            reg[A] = csr[B];
            break;
        }
        case 1: 
        {
            reg[A] = reg[B] + D;
            break;
        }
        case 2: 
        {
            reg[A] = loadFromMemory(0,B,C,D);
            break;
        }
        case 3: 
        {
            reg[A] = loadFromMemory(0,B,0,0);
            reg[B] = reg[B] + D;
            break;
        }
        case 4: 
        {
            csr[A] = reg[B];
            break;
        }
        case 5: 
        {
            csr[A] = csr[B] | D;
            break;
        }
        case 6: 
        {
            csr[A] = loadFromMemory(0,B,C,D);
            break;
        }
         case 7: 
        {
            csr[A] = loadFromMemory(0,B,0,0);
            reg[B] = reg[B] + D;
            break;
        }
    }
}



void Emulator::store(char M, char A, char B, char C, int D){

        switch(M){
        case 0:
        {
            storeToMemory(A,B,0,D,reg[C]);
            break;
        }
        case 2:
        {
            int address = loadFromMemory(A,B,0,D);
            storeToMemory(0,0,0,address, reg[C]);
            break;
        }
        case 1:
        {
            reg[A] = reg[A] + D;
            storeToMemory(A,0,0,0, reg[C]);
            break;
        }
    }
}

int Emulator:: loadFromMemory(char A, char B, char C, int D){
    int val = 0;

    for(int i = 0; i < 4; i++){
        unsigned char byte = memory[reg[A] + reg[B] + reg[C] + D + i];
       val |= (static_cast<int>(byte) << (i * 8));
        
    }
    return val;
}

void Emulator:: storeToMemory(char A, char B, char C, int D, int val){
    int a = val;
    for(int i = 0; i < 4; i++){
        memory[ reg[A] + reg[B] + reg[C] +  D + i ] = (val & 0xFF);
        val >>= 8;
    }
}

bool Emulator:: end = false;

int getD(unsigned int val){
    
    int twelfthBit = (val >> 11) & 1;
    if(twelfthBit){
        //negative
        return static_cast<int>(val | 0xFFFFF000);  
    }
    else{
        //positive  
        return static_cast<int>(val);   
    }
}

void Emulator::decodeInstructions(){

    while(!end){
        for(int i = 0; i < 4; i++)
            instruction[i] = memory[pc+i];

        pc += 4;

        char op = ((instruction[0] & 0xF0) >> 4);
        char M = instruction[0] & 0x0F;
        char A = (instruction[1] & 0xF0) >> 4;
        char B = instruction[1] & 0x0F;
        char C = (instruction[2] & 0xF0) >> 4;
        
        unsigned int val = ((((instruction[2] & 0x0F) << 8 ) | instruction[3] & 0xFF));
        int D = getD(val);   

        switch(op)
        {
            case 0x0 :  
            {
                halt();
                break;
            }
            case 0x1 :  
            {
                intF();
                break;
            }   
            case 0x2 :  
            {
                call(M,A,B,D);
                break;
            }
            case 0x3 :  
            {
                jmp(M,A,B,C,D);
                break;
            }
            case 0x4 :  
            {
                xchg(B,C);
                break;
            }
            case 0x5:
            {
                aritmetic(M,A,B,C);
                break;
            }
            case 0x6:
            {
                logic(M,A,B,C);
                break;
            }
            case 0x7:
            {
                shift(M,A,B,C);
                break;
            }
            case 0x8:
            {
                store(M,A,B,C,D);
                break;
            }
            case 0x9:
            {
                load(M,A,B,C,D);            
                break;
            }
        }
    }
}

