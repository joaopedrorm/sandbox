#include "bit_wise_xor.h"

int main (int argc, char *argv[])
{
	if (argc != 3){ 
		// argc should be 3 for correct execution
		// We print argv[0] assuming it is the program name
		std::cout<<"\nusage: "<< argv[0] <<" <option> <filename>\n";
		std::cout<<"\nthis program do a bitwise xor for multiples inputs.\n";
		std::cout<<"\n -b input in ascii coded binary format;";
		std::cout<<"\n -h input in ascii coded hexadecimal format;";
		std::cout<<"\n -t input is pure ascii text;\n\n";
	}
	else {
		// We assume argv[2] is a filename to open
		std::ifstream file (argv[2]);
		// Always check to see if file opening succeeded
		if (!file.is_open()){
			std::cout<<"Could not open file\n";
		}
		else {
			inputType type = defineInput(argv);
			// first test input file, if is valid proceed
			if (test(&file, type)){
				// capture data and call xor()
				process(&file, type);
			}
			else {
				// file is not valid, output example file
				printExample();
			}
			file.close();
		}
	}
	return 0;
}


void process (std::istream *input, inputType type){
	// input file assumed to have ascii coded numbers
	char c;
	// int variable to serve as temporary accumulator
	std::vector<std::vector <unsigned char> > data_matrix;
	std::vector<unsigned char> data_line;
	input->get(c) ;
	// get all data in input file
	while (input->good()){
		
		// ignore comments
		if (c == '#'){
			while ((c != '\n') && input->good()){
				input->get(c);
			}
			input->get(c);
		}
		
		if (type == asciihexadecimal && c == '$'){
			int i = 0;
			bool first = true;
			data_line.push_back(0);
			input->get(c);
			// loop finish only when there are no more data to import
			// (reaches $) or when i reaches a predetermined limit
			while (i < 0xfffffff0 && c != '$'){
				// ignore ' ', '-', '.' and ','
				while (c == ' ' || c == '-' || c == ',' || c == '.'){
					input->get(c);
				}
				// if eof is aquired throw exception
				if (!input->good()){
					throw LOL2;
				}
				if (first && c != '$'){
					data_line[i] = ahex2bin(c) << 4;
					first = false;
				}
				else if (c != '$'){
					data_line[i] += ahex2bin(c);
					i++;
					first = true;
					data_line.push_back(0);
				}
				input->get(c);
			}
			data_matrix.push_back(data_line);
			data_line.clear();
		}

		if (type == asciibinary && c == '$'){
			int i = 0;
			int j = 0;
			data_line.push_back(0);
			input->get(c);
			//loop finish only when there are no more data to import
			// (reaches $) or when i reaches a predetermined limit
			while (i < 0xfffffff0 && c != '$'){
				// ignore ' ', '-', '.' and ','
				while (c == ' ' || c == '-' || c == ','|| c == '.'){
					input->get(c);
				}
									// if eof is aquired throw exception
				if (!input->good()){
					throw LOL2;
				}
				if (c == '0' && c != '$'){
					data_line[i] += 0 << j;
					j++;
				}
				else if(c != '$'){
					data_line[i] += 0 << j;
					j++;
				}
				if (j >= 8){
					i++;
					j = 0;
					data_line.push_back(0);
				}
				input->get(c);
			}
			// loop finished, next line
			data_matrix.push_back(data_line);
			data_line.clear();
		}
		if (type == normaltext){
			int i = 0;
			data_line.push_back(0);
			//loop finish only when there are no more data to import
			while (i != -1){
				// catch $ delimiter to stop
				if (c == '\n'){
					i = -1;
				}
				// if eof is aquired throw exception
				if (!input->good()){
					throw LOL2;
				}
				data_line[i] = (unsigned char)(c);
				i++;
				data_line.push_back(0);
				input->get(c);
			}
			// loop finished, next line
			data_matrix.push_back(data_line);
			data_line.clear();
		}
		input->get(c);
	}

	// data is now in data_matrix
	// bitwise xor now
	int size_matrix_column = data_matrix.size();
	for (int i = 0; i < size_matrix_column; i++){
		for (int j = i + 1; j < size_matrix_column; j++){
			data_matrix.push_back(bitwiseXor(data_matrix[i], data_matrix[j]));
		}
	}
	// output file in txt normal txt and hex code
	std::fstream arquivotxt("output.txt", std::fstream::out);
	if (!arquivotxt){
		std::cout << "\noutput.txt could not be open!\n";
		return;
	}
	int size_matrix_column_new = data_matrix.size();
	int size_matrix_line = data_matrix[0].size();
	for (int i = 0; i < size_matrix_column; i++){
		arquivotxt << "# index " << i << " from input.\n";
		for (int j = 0; j < size_matrix_line; j++){
			arquivotxt << data_matrix[i][j];
		}
		arquivotxt << "\n\n";
	}
	for (int i = size_matrix_column; i < size_matrix_column_new; i++){
		arquivotxt << "# index (" << (i/size_matrix_column) << ",";
		arquivotxt << (i%size_matrix_column) << ") output.\n";
		for (int j = 0; j < size_matrix_line; j++){
			arquivotxt << data_matrix[i][j];
		}
		arquivotxt << "\n\n";
	}
	arquivotxt.close();
}

unsigned char ahex2bin (char c){
	if ('0' <= c && c <= '9') { return c      - '0'; }
    if ('a' <= c && c <= 'f') { return c + 10 - 'a'; }
    if ('A' <= c && c <= 'F') { return c + 10 - 'A'; }
    throw LOL1;
}

inputType defineInput (char **type){
	if (type[1][0] == '-'){
		switch (type[1][1]) {
			case 'b' :
				return asciibinary;
			case 'h' :
				return asciihexadecimal;
			case 't' :
				return normaltext;
		}
	}
	return undefined;
}

bool test(std::istream *input, inputType type){
	// not implemented yet
	// returns true
	return true;
}

void printExample(void){
	// not implemented yet
}

std::vector<unsigned char> bitwiseXor(std::vector<unsigned char> &input1, std::vector<unsigned char> &input2){
	std::vector<unsigned char> aux;
	if (input1.size() != input2.size()){
		throw LOL3;
	}
	for (int i = 0; i < input1.size(); i++){
		aux.push_back(input1[i]^input2[i]);
	}
	return aux;
}