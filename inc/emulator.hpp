#include <vector>
using namespace std;
#include <unordered_map>

class Emulator{

public:

  static unordered_map<unsigned int,  char> memory;
  static  int reg[16];

  static  int csr[3];
 
  static int temp;

  static bool end;

  static unsigned char instruction[4];
  static void initRegisters();

  static void decodeInstructions();

  static void push(int val);

  static void storeToMemory(char A, char B, char C, int D, int val);
  static int loadFromMemory(char A, char B, char C, int D);
  static void halt();
  static void intF();
  static void call(char M, char A, char B, int D);
  static void jmp(char M, char A, char B, char C, int D);
  static void xchg(char B, char C);
  static void aritmetic(char M, char A, char B, char C);
  static void logic(char M, char A, char B, char C);
  static void shift(char M, char A, char B, char C);
  static void load(char M, char A, char B, char C, int D);
  static void store(char M, char A, char B, char C, int D);
  

};