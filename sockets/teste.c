#include <stdio.h>
#include <netdb.h>

int main()
{
	struct hostent *host = malloc(sizeof(hostent));
	char *query = malloc(20 * sizeof(char));
	host = gethostbyname(query);
//	host = gethostbyaddr();
	printf("%s", host->);
	return 0;
}
