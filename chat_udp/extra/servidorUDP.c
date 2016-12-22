#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


#define UDP 17
#define QLEN 10
#define BUFFERSIZE 500
#define RX_BUFFER_SIZE 90
#define TX_BUFFER_SIZE 90

void processo_cliente()
{
	
}

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
	char			rxbuffer[RX_BUFFER_SIZE];
	char			txbuffer[TX_BUFFER_SIZE];


//	pedido de dados ao usuário
	printf("Entre com a porta: ");
	scanf("%d", &myport);

//	chamada socket para tcp
	sd = socket(PF_INET, SOCK_DGRAM, UDP);

//	definição de endereço de destino
	mylocal_addr.sin_family = AF_INET;
	mylocal_addr.sin_addr.s_addr = INADDR_ANY;
	mylocal_addr.sin_port   = htons(myport);
	
//	chamada bind
	status = bind(sd, (struct sockaddr *) &mylocal_addr, sizeof(struct sockaddr_in));

//	teste de conexão bind
	if (status == -1)
		perror("Erro na chamada connect");

//	variavel size de clienteaddr
	size = sizeof (struct sockaddr_in);
	

//	chamada recvfrom
	status = recvfrom(sd, rxbuffer, RX_BUFFER_SIZE, 0, (struct sockaddr *) &clientaddr, &size);

//	verificação de erro para chamada listen()	
	if (status <= 0)
	{
		perror("Erro na chamda recvfrom: ");
		exit(1);
	}

//	copia de rxbuffer para txbuffer
	strcpy(txbuffer, rxbuffer);

//	chamada sendto()
	status = sendto( sd, txbuffer, strlen(txbuffer)+1, 0, (struct sockaddr *) &clientaddr, sizeof (struct sockaddr));

//	verificação de erro chamada sendto()
	if (status <= 0)
	{
		perror("Erro na chamada sendto:");
	}

//	impressão da msg recebida
	printf("%s\n", rxbuffer);

//	chamada close
	status = close(sd);

//	verificação da chamada close
	if (status == -1)
		perror("Erro na chamada close");

	return 0;
}
