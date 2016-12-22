#include <fstream>
#include <iostream>
#include <vector>

// enumerable for input control
typedef enum inputType{
	asciibinary = 0,
	asciihexadecimal,
	normaltext,
	undefined
} inputType;

// enumerable for errors
typedef enum error{
	LOL1 = 1,
	LOL2,
	LOL3
} error;

// define input type
inputType defineInput (char **type);

// run do xor bitwise algorithm
void process(std::istream *input, inputType type);

// test if file is valid
bool test(std::istream *input, inputType type);

// print a valid output to a file named input.txt
void printExample(void);

// return a unsigned char 4bits number, input is ascii char hexadecimal code
// unsigned char is 1byte wide, but here only fist 4bits are used
unsigned char ahex2bin (char c);

// bitwise xor
std::vector<unsigned char> bitwiseXor(std::vector<unsigned char> &input1, std::vector<unsigned char> &input2);