#include <stdio.h>

int func2(void) {
	static int ret = 0;
	return ret++;
}

int func1(void) { 
	        return func2(); 
} 

int main(int argc, char * argv[]) {
	int i;
        for(i = 0; i<10; i++) {
		printf("%d - t=%d\n", i, func1());
	}
	return 0;
}
