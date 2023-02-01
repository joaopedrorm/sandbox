#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]) {
	char arr[30];
	char * ptr;
	ptr = &arr[1];
	printf("sizeof(arr)=%d, sizeof(ptr)=%d, sizeof(*ptr)=%d, strlen(ptr)=%d", sizeof(arr),sizeof(ptr), sizeof(*ptr), strlen(ptr));
	return 0;
}
