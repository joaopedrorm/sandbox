#include <iostream>

int main(int argc, char *argv[], char *envp[]) {

	std::cout << "TESTE" << std::endl;
	std::cout << "argc=" << argc << std::endl;
	for (int i = 0; i < argc; i++) {
		std::cout << "argv[" << i << "]=" << argv[i] << std::endl;
	}	
	for (int j = 0; envp[j] != NULL; j++){
		std::cout << "envp[" << j << "]=" << envp[j] << std::endl;
	}
	return 0;
}
