#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define QLEN 10
#define BUFFERSIZE 500

int main()
{
//	declaração da estrutura de dados
	int			sd;
	int			newsd;
	int			size;
	int			status;
	struct sockaddr_in	clientaddr;
	struct sockaddr_in	mylocal_addr;
	unsigned short		myport;
	char			buffer[BUFFERSIZE];
	char			rxbuffer[80];

//	pedido de dados ao usuário
	printf("Entre com a porta: ");
	scanf("%d", &myport);

//	chamada socket para tcp
	sd = socket(PF_INET, SOCK_STREAM, 6);

//	definição de endereço de destino
	mylocal_addr.sin_family = AF_INET;
	mylocal_addr.sin_addr.s_addr = INADDR_ANY;
	mylocal_addr.sin_port   = htons(myport);
	
//	chamada bind
	status = bind(sd, (struct sockaddr *) &mylocal_addr, sizeof(struct sockaddr_in));

//	teste de conexão bind
	if (status == -1)
		perror("Erro na chamada connect");

//	chamada listen()
	status = listen(sd, QLEN);

//	verificação de erro para chamada listen()	
	if (status != 0)
	{
		perror("Erro na chamda listen()");
		exit(1);
	}

//	preparação para chamada accept
	size = sizeof(clientaddr);

//	chamada accept()
	newsd = accept( sd, (struct sockaddr *) &clientaddr, (socklen_t *) &size);

//	verificação de erro na chamada accept
	if (newsd < 0)
	{
		perror("Erro na chamada accept()");
		exit(1);
	}

//	chamada read()
	status = read(newsd, buffer, BUFFERSIZE);

//	impressão da msg recebida
	printf("%s\n", buffer);

//	envio de resposta
//	status = write(newsd, buffer, strlen(buffer)+1);

//	chamada close
	status = close(sd);

//	verificação da chamada close
	if (status == -1)
		perror("Erro na chamada close");

	return 0;
}
