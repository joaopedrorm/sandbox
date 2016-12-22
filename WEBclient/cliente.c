#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define BUFFERSIZE 500

// socket do cliente
int sd;
char stringIP[50];

int main(){

	char rxbuffer[BUFFERSIZE];
	char txbuffer[BUFFERSIZE];
	int status;
	char lixo;

	struct sockaddr_in server_addr;
	unsigned short serverport;
	printf("IP:");
	scanf("%s", stringIP);

	printf("Porta:");
	scanf("%hu", &serverport);
	scanf("%c", &lixo);

	// inicializa socket e verifica erro
	sd = socket(PF_INET, SOCK_STREAM, 6);
	if (sd == -1)
		perror("Erro na chamada socket");

	server_addr.sin_family		= AF_INET;
	server_addr.sin_port		= htons (serverport);

	status = inet_pton(AF_INET,stringIP, &server_addr.sin_addr);
	if(status <= 0)
		perror("Erro na conversao do endereco IP");

	// chamada connect
	status = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if(status != 0){
		perror("Erro na chamada connect");
		exit(1);
	}
	printf("-Fez connect\n");
	
	// chamada write
	printf("Mensagem:\n");
	fgets(txbuffer, BUFFERSIZE, stdin);
	status = write(sd, txbuffer, strlen(txbuffer)+1);
	if (status == -1)
		perror("Erro na chamada write");

	printf("-Fez write\n");

	// chamada read
	status = read(sd, rxbuffer, BUFFERSIZE);
	if(status == -1)
		perror("Erro na chamada read");
	printf("%s", rxbuffer);

	printf("-Fez read\n");


	// chamada close
	if(status == -1)
		perror("Erro na chamada close do servidor");

	printf("-Fez close\n");

	return 0;
}
