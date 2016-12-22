#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
//	include para socket()
#include <sys/socket.h>

//	include para connect()
#include <netdb.h>
#include <arpa/inet.h>

//	include para threads
#include <pthread.h>

/*////////////////////////////////////////////////////////////////////////////////
// BUFFERSIZE       95 = (80 bytes msg + 5 bytes CMD + 10 bytes USERNAMESIZE)   //
// LOGONBUFFERSIZE  15 = (10 bytes username + 5 bytes CMD)                      //
// LOGOFFBUFFERSIZE  5 bytes CMD                                                //
// USERNAMESIZE     10 bytes username                                           //
// CMDSIZE           5 bytes CMD                                                //
// MSGSIZE          80 bytes                                                    //
// TX_BUFFER_SIZE = RX_BUFFER_SIZE = BUFFERSIZE                                 //
// 1byte \0  -> levado em consideração na implementação                         //
////////////////////////////////////////////////////////////////////////////////*/

#define BUFFERSIZE 	    95
#define LOGONBUFFERSIZE 15
#define LOGOFFBUFFERSIZE 5
#define USERNAMESIZE 	10
#define CMDSIZE		     5
#define HOSTNAMESIZE    80
#define MSGSIZE         80

//	comandos do cabeçalho
#define CMD_USER 	"USER:"
#define CMD_UP      "UP  :"
#define CMD_EXIT 	"EXIT:"
#define CMD_OKOK 	"OKOK:"
#define CMD_DOWN 	"DOWN:"
#define CMD_BUSY 	"BUSY:"
#define CMD_TEST 	"TEST:"

//  variáveis globais
//  declaração do descritor do socket
int socketDescriptor;
FILE *terminal;

void conectar(void);
void logon();
void escreveMsg(char *cmd, char *msg, char *buffer);
void recebeMsg(char *cmd, char *msg, int msg_size, char *buffer);
void removeStrEnter(char *buffer, char c);
void limparStr(char *buffer);
void transmite();       // função na qual ficará o thread responsável pela transmissão das mensagens
void recebe();          // função na qual ficará o thread responsável pelo recebimento das mensagens
void logoff();

void conectar(void)
{
//	Chamada socket
//	int socket(int domain, int type, int protocol)

//	declaração de estrutura de dados para socket
    int		        	status;
    unsigned short		serverport;
    struct sockaddr_in	serversockaddr;

//	declaração de variáveis para resolução de nomes
    char		    	hostnamep[HOSTNAMESIZE+1];
    struct hostent		*hostentryp;

//	pedido de nome do host
    printf("Entre com o nome da máquina ou endereço IP: ");
    fgets(hostnamep, HOSTNAMESIZE, stdin);
//    scanf("%s", hostnamep);
    removeStrEnter(hostnamep, '\0');

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
//		verifica se endereco ip
        status = inet_pton(AF_INET, hostnamep, &serversockaddr.sin_addr);

//		verificação de erro
        if (status <=0)
        {
            perror("ERRO: inet_pton()");
            printf("Nome de host ou endereço IP inválido \n");
            exit(-1);
        }
    }

//	pedido do endereço de porta e IP
    printf("Usando porta padrão: 10000 \n");
    serverport = 10000;

//  se porta
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
    {
        perror("Erro na chamada connect");
        exit(1);
    }

}

void logon()
{
    char		txbuffer[LOGONBUFFERSIZE+1];
    char		rxbuffer[LOGONBUFFERSIZE+1];
    char        username[USERNAMESIZE+1];
    int         status;

//  Terminal
    char terminalName[80];
    printf("Entre com o nome do novo terminal: ");
    scanf("%s", terminalName);
//  eliminar \n
    getchar();
    terminal = fopen(terminalName, "a+");
    if (terminal == NULL)
    {
        perror("Abertura do terminal");
        exit(1);
    }


//	Mensagem
    printf("Entre com um nome de usuário: ");
    fgets(username, USERNAMESIZE, stdin);
    removeStrEnter(username, ' ');

//	contruir msg txbuffer
    escreveMsg(CMD_USER, username, txbuffer);

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

//  verificação de msg do servidor
//  se OKOK:\0 proseguir
//  se BUSY apresentar msg de servidor cheio para usuario
//  // depois será feito: se OKOK:DUPLICATED_NAME\0 pedir novo nome de usuário

    if(memcmp(txbuffer, CMD_BUSY, CMDSIZE) == 0)
    {
        printf("Servidor está cheio no momento \n");
    }
    else
    {
        printf("Conectado...\n");
    }

}

void escreveMsg(char *cmd, char *msg, char *buffer)
{
    memset(buffer, 0, BUFFERSIZE);
//	montando o cabeçalho
    strcpy(buffer, cmd);

//	montando a mensagem
    strcat(buffer, msg);
}

void recebeMsg(char *cmd, char *msg, int msg_size, char *buffer)
{
    int     i;
//  Retirando o cabeçalho
    strncpy(cmd, buffer, CMDSIZE);
    for (i = 0; buffer[CMDSIZE+i] != '\0' && i < (msg_size); i++)
    {
        msg[i] = buffer[CMDSIZE+i];
    }
}

void removeStrEnter(char *buffer, char c)
{
    int i = 0;
    while ( (buffer[i] != '\n') && (buffer[i] != '\0'))
    {
        i++;
    }
    if (buffer[i] == '\n')
    {
        buffer[i] = c;
    }
}

void limparStr(char *buffer)
{
    int i = 0;
    while ( buffer[i] != '\0' && i < BUFFERSIZE)
    {
        buffer[i] = '\0';
        i++;
    }
}

void transmite()
{
//  Variáveis
    char    mensagem[MSGSIZE+1];
    char	txbuffer[BUFFERSIZE+1];
    int     status;

    while(1)
    {
        memset(mensagem, 0, MSGSIZE);
        memset(txbuffer, 0, BUFFERSIZE);
//	Mensagem
        printf("Mensagem>> ");
        fgets(mensagem, BUFFERSIZE, stdin);
        removeStrEnter(mensagem, ' ');

//  Testando o critério de logoff do chat -> usuário digitar EXIT
        if(memcmp(mensagem, CMD_EXIT, CMDSIZE) == 0)
        {
//  Sim, digitou, inicia o processo de logoff
            printf("Realizando logoff");
            logoff(socketDescriptor);
//  Finaliza a thread do transmissor
            pthread_exit(NULL);
        }
        if(memcmp(mensagem, CMD_USER, CMDSIZE) == 0)
        {
//  Sim, digitou, inicia o processo de logoff
            printf("Trocando nome de usuario...\n");
            escreveMsg(CMD_USER, mensagem, txbuffer);

        }
        else
        {
//  Escrevendo a mensagem em txbuffer
        limparStr(txbuffer);
        escreveMsg(CMD_UP, mensagem, txbuffer);
        }

//	chamada int send(int sd, void * txbuffer, int msgsize, int flags)
        status = send (socketDescriptor, txbuffer, strlen(txbuffer)+1, 0);

//	verificação de erro
        if (status < 0)
            perror("Erro na chamada send");
    }
}

void recebe()
{
//  Declaração de variáveis
    char	mensagem[MSGSIZE+1];
    char    comando[CMDSIZE+1];
    char	rxbuffer[BUFFERSIZE+1];
    int     status;
    fprintf(terminal, "\e[3JCliente de Chat UDP\n");
    while(1)
    {
        memset(mensagem, 0, MSGSIZE);
        memset(rxbuffer, 0, BUFFERSIZE);
//	chamada int recv(int sd, void * rxbuffer, int rxbuffersize, int flags)
    status = recv(socketDescriptor, rxbuffer, sizeof(rxbuffer), 0);

//	verificação de erro
    if (status < 0)
        perror("Erro na chamada recv");


//  Desconstrução da mensagem recebida
        recebeMsg(comando, mensagem, BUFFERSIZE, rxbuffer);

//  Testando se o comando recebido está conforme, i.e., se recebeu EXIT:
        if(memcmp(comando, CMD_EXIT, CMDSIZE) == 0)
        {
//  Recebeu, termina a thread do receptor
            pthread_exit(NULL);
        }

//  Impressão da mensagem recebida
        fprintf(terminal, "%s \n", mensagem);
    }
}

void logoff()
{
//  Declaração das variáveis
    int     status;
    char    txbuffer[BUFFERSIZE];

//  Monta a mensagem de logoff, EXIT:\0
    escreveMsg(CMD_EXIT, '\0', txbuffer);

//	chamada int send(int sd, void * txbuffer, int msgsize, int flags)
    status = send (socketDescriptor, txbuffer, strlen(txbuffer)+1, 0);

//	verificação de erro
    if (status < 0)
    {
        perror("Erro na chamada send");
        return;
    }
}

int main()
{
//	Declaração de estrutura de dados principal
    int			status;

//  Declaração das variáveis de thrads
    pthread_t   transmissor;
    pthread_t   receptor;

//	Chamada à função conectar(), realizando a conexão
    conectar();

//  Chamada à função logon(), para logar no servidor
    logon(socketDescriptor);

//  Disparando a thread do transmissor
    pthread_create(&transmissor, NULL, (void*) transmite,(void *)0);

//  Disparando a thread do receptor
    pthread_create(&receptor, NULL, (void*) recebe,(void *)1);

    pthread_join(transmissor, NULL);
    pthread_join(receptor, NULL);

//	chamada close()
    status = close(socketDescriptor);

//	verificação da chamada close
    if (status == -1)
        perror("Erro na chamada close");

    return 0;
}

