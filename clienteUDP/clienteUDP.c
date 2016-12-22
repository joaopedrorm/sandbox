#include <stdlib.h>
#include <stdio.h>

//	include para socket()
#include <sys/socket.h>

//	include para connect()
#include <netdb.h>

int main()
{
//	Chamada socket
//	int socket(int domain, int type, int protocol)
//	declaração do descritor de socket e buffer de dados
	int 			socketDescriptor;
	char			txbuffer[80];
	char			rxbuffer[80];

//	declaração de estrutura de dados para socket
	int			status;
	unsigned short		serverport;
	struct sockaddr_in	serversockaddr;

//	declaração de variáveis para resolução de nomes
	char			hostnamep[80];
	struct hostent		*hostentryp;
	
//	pedido de nome do host
	printf("Entre com o nome da máquina ou endereço IP: ");
	scanf("%s", hostnamep);
	
//	conversão de nome para endereço
	hostentryp = gethostbyname(hostnamep);
	
//	verificação de erro, endereço IP ou nome
	if (hostentryp != NULL)
	{
// 		conseguiu converter, copia os 4 bytes do endereço IP
		bcopy(hostentryp->h_addr, (char *)&serversockaddr.sin_addr, hostentryp->h_length);
		
//		imprime endereço ip correspondente
		printf("Endereço = %d.%d.%d.%d \n",
			((unsigned char *) hostentryp->h_addr) [0],
			((unsigned char *) hostentryp->h_addr) [1],
			((unsigned char *) hostentryp->h_addr) [2],
			((unsigned char *) hostentryp->h_addr) [3]);
	}
	else
	{
//		Não conseguiu converter nome para nendereço IP
//		verifica se hostname
		status = inet_pton(AF_INET, hostnamep, &serversockaddr.sin_addr);
		
//		verificação de erro		
		if (status <=0)
		{
			perror(" ERRO: intet_pton()");
			printf("Nome de host ou endereço IP inválido \n");
			return(-1);
		}
	}

//	pedido do endereço de porta e IP
	printf("Entre com o endereço da porta: ");
	scanf("%d", &serverport);
	
//	PF_INET = IPV4; SOCK_DGRAM = tipo de dado datagrama; 17 = protocolo UDP
	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 17);

//	Verificação de erro
	if (socketDescriptor == -1)
	{
		perror("Erro na chamada socket.");
		exit(1);
	}

//	Mensagem para sinalizar que socket() foi concluido com sucesso	
	printf("Descritor do socket = %d\n", socketDescriptor);	

//	definição de dados para estrutura
	serversockaddr.sin_family = AF_INET;
	serversockaddr.sin_port = htons(serverport);

//	chamada int connect(int sd, struct sockaddr *serversockaddr, int size) (include netdb.h)
	status = connect(socketDescriptor, (struct sockaddr *)&serversockaddr, sizeof(serversockaddr));

//	teste de conexão
	if (status != 0)
		perror("Erro na chamada connect");

//	Mensagem
	printf("Entre com a mensagem: ");
	scanf("%s", txbuffer);

//	chamada int send(int sd, void * txbuffer, int msgsize, int flags)
	status = send (socketDescriptor, txbuffer, strlen(txbuffer)+1, 0);

//	verificação de erro	
	if (status < 0)
		perror("Erro na chamada send");

//	chamada int recv(int sd, void * rxbuffer, int rxbuffersize, int flags)
	status = recv(socketDescriptor, rxbuffer, sizeof(rxbuffer), 0);

//	verificação de erro
	if (status < 0)
		perror("Erro na chamada recv");

//	impressão da mensagem recebida
	printf("Mensagem recebida: %s\n", rxbuffer);

//	chamada close
	status = close(socketDescriptor);

//	verificação da chamada close
	if (status == -1)
		perror("Erro na chamada close");

	return 0;
}
