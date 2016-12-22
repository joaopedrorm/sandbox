#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define BUFFERSIZE 1000000

int main()
{
//	declaração da estrutura de dados
	int			sd;
	int			status;
	struct sockaddr_in	serveraddr;
	unsigned short		serverport;
	char			hostnamep[BUFFERSIZE];
	char			txbuffer[BUFFERSIZE];
	char			rxbuffer[BUFFERSIZE];
	char			*palavra;

//	pedido de dados ao usuário
	printf("Entre com endereço ip: ");
	scanf("%s", hostnamep);

	printf("Entre com a porta: ");
	scanf("%d", &serverport);

//	printf("Entre com a mensagem: ");
//	getchar();
//	fgets(txbuffer, BUFFERSIZE, stdin); 
	palavra = "GET / HTTP/1.0\r\n\r\n";
//	scanf("%s", txbuffer);

//	chamada socket para tcp
	sd = socket(PF_INET, SOCK_STREAM, 6);

//	definição de endereço de destino
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port   = htons(serverport);
	
//	chamada de conversão de dados
	status = inet_pton(AF_INET, hostnamep, &serveraddr.sin_addr);

//	verificação de erro de conversão de dados
	if (status <= 0)
		perror("Erro na conversão do endereço IP");

//	chamada connect
	status = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

//	teste de conexão
	if (status != 0)
		perror("Erro na chamada connect");

//	chamada write()
	status = write(sd, palavra, strlen(palavra)+1);

//	teste de envio
	if (status == -1)
		perror("Erro na chamada write");

//	chamda read()
	status = read(sd, rxbuffer, sizeof(rxbuffer));

//	teste de recebimento
	if (status == -1)
		perror("Erro na chamada read");

//	impressão da mensagem recebida
	printf("Mensagem do servidor: %s\n", rxbuffer);

//	chamada close
	status = close(sd);

//	verificação da chamada close
	if (status == -1)
		perror("Erro na chamada close");

	return 0;
}
