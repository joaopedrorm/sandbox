//  include para fila
#include "servidorChatUDP.h"
#include "fila.h"

void inicializar(void)
{
//  variáveis locais
    struct addrinfo     hints;
    struct addrinfo     *serverinfo, *p;
    struct sockaddr     sa;
    char                *myport;
    int                 status, port;
    void                *addr;
    char                *ipver;
    char                ip[INET6_ADDRSTRLEN];

    printf("Servidor de chat UDP V1.1\n");

//  porta pre-definida
    myport = PORT;
//  printf("Porta default: %s\n", myport);

    /* organização da estrutura de dados
     zerar memoria da estrutuda de dados
     AF_UNSPEC, ipv4 ou ipv6
     SOCK_DGRAM, UDP
     AI_PASSIVE, IP automatico*/
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

//  chamada getaddrinfo e teste
    status = getaddrinfo(NULL, myport, &hints, &serverinfo);
    if (status != 0)
    {
        printf("getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

// loop para tentar se conectar em algum socket disponivel
    for(p = serverinfo; p != NULL; p = p->ai_next) 
    {
        //  chamada para socket
        socket_master = socket(serverinfo->ai_family, serverinfo->ai_socktype
                    , serverinfo->ai_protocol);
        if (socket_master == -1) 
        {
            perror("cliente: socket");
            continue;
        }

        //  chamada bind
        if (bind(socket_master, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1) 
        {
            close(socket_master);
            perror("cliente: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "cliente: falha de conexao.\n");
        exit(2);
    }
    //  free addrinfo
    freeaddrinfo(serverinfo); // free the linked-list

//  imprimir configuracao da conexao
   if (p->ai_family == AF_INET) 
    { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr = &(ipv4->sin_addr);
        port = ntohs(ipv4->sin_port);
        ipver = "IPv4";
    } 
    else 
    { // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
        addr = &(ipv6->sin6_addr);
        port = ntohs(ipv6->sin6_port);
        ipver = "IPv6";
    }
//TODO: getifconfig, mostrar todos os endereços ips da maquina
    // converte ip para string e imprime
    //inet_ntop(sa.ai_family, sa.ai_addr, ip, sizeof ip);
    printf("Associado ao endereco %s: %s - Porta: %u\n", ipver, ip, port);
    printf("Digite q e pressione Enter para encerrar!\n");

}

void receptor(void)
{
    //      variaveis locais
    int                 size;
    int                 status;
    char                rxbuffer[RX_BUFFER_SIZE + 1];
    struct requisicao   *R;

    while (continuar)
    {
        R = NovaRequisicao();
        if (R == NULL)
        {
            printf("ERRO: alocação de estrutura sockaddr_in falhou. \n");
            exit(1);
        }

    //      variavel size de clienteaddr
        size = sizeof (struct sockaddr_in);

    //      chamada recvfrom
        status = recvfrom(socket_master, rxbuffer, RX_BUFFER_SIZE, 0,
                          (struct sockaddr *)R->addr, (socklen_t *)&size);

    //      verificação de erro para chamada recfrom()
        if (status < 0)
        {
            perror("Erro na chamda recvfrom: ");
            exit(1);
        }
        else if (status == 0)
        {
            printf("Receptor: conexao terminada.");
        }
        else
        {
            //      impressão da msg recebida
            printf("\nMensagem recebida de: %d\n", ntohs(R->addr->sin_port));
            printf("->%s\n", rxbuffer);
            printf("%d bytes recebidos.\n", status);
            //      rotina guardar na fila
            strcpy(R->buffer, rxbuffer);
            InserirFila(Fila, R);
        }
    }
}


int	insereCliente(struct sockaddr_in *addr, char *nome)
{
    int i, j;
    for (i = 0; i < CLIENT_LIMIT; i++)
    {
        if(clientes[i].isFree == 1)
        {
            memcpy(&(clientes[i].addr), addr, sizeof(*addr));
            memset(clientes[i].nome, 0, USERNAMESIZE + 1);
            strncpy(clientes[i].nome, nome, USERNAMESIZE);
            for (j = 0; j < USERNAMESIZE; j++)
            {
                if(clientes[i].nome[j] == '\0')
                {
                    clientes[i].nome[j] = ' ';
                }
            }
            clientes[i].isFree = 0;
            return 1;
        }
    }
    return 0;
}

int procuraCliente(struct sockaddr_in *addr)
{
    int i;
    for (i = 0; i < CLIENT_LIMIT; i++)
    {
        if (clientes[i].addr.sin_port == addr->sin_port && clientes[i].isFree == 0)
        {
                return i;
        }
    }
    return -1;
}

int removeCliente(struct sockaddr_in *addr)
{
    int i;
    i = procuraCliente(addr);
    if (i == -1)
    {
        return 0;
    }
    clientes[i].isFree = 1;
    return 1;
}

void incializarClientes(void)
{
    int i;
    for (i = 0; i < CLIENT_LIMIT; i++)
    {
        clientes[i].isFree = 1;
    }
}

void enviarMsg(char *txbuffer, struct sockaddr_in *addr)
{
    int status;
//  chamada sendto()
    status = sendto( socket_master, txbuffer, strlen(txbuffer)+1, 0,
                     (struct sockaddr *) addr, sizeof (struct sockaddr));

//	    verificação de erro chamada sendto()
    if (status <= 0)
    {
        perror("Erro na chamada sendto:");
    }
    else
    {
        printf("%d bytes enviados.\n", status);
    }
    memset(txbuffer, 0, TX_BUFFER_SIZE);

}

void escreveMsg(char *cmd, char *msg, int msg_size, char *buffer)
{
    memset(buffer, 0, TX_BUFFER_SIZE);
//	montando o cabeçalho e mensagem
	strcpy(buffer, cmd);
	strncat(buffer, msg, msg_size);
}

void recebeMsg(char *cmd, char *msg, int msg_size, char *buffer)
{
    int     i;
    memset(cmd, 0, CMDSIZE + 1);
    memset(msg, 0, MSGSIZE + 1);

//  Retirando o cabeçalho
    strncpy(cmd, buffer, CMDSIZE);
    for (i = 0; buffer[CMDSIZE+i] != '\0' && i < (msg_size); i++)
    {
        msg[i] = buffer[CMDSIZE+i];
    }
}

void broadcastMsg(char *txbuffer)
{
    int status, i;
    for (i = 0; i < CLIENT_LIMIT; i++)
    {
        if (clientes[i].isFree == 0)
        {
//          Chamada sendto()
            status = sendto( socket_master, txbuffer, strlen(txbuffer)+1, 0,
                (struct sockaddr *)&(clientes[i].addr), sizeof (struct sockaddr));

//	        verificação de erro chamada sendto()
            if (status <= 0)
            {
                perror("Erro na chamada sendto:");
            }
            else
            {
                printf("%d bytes enviados (broadcast).\n", status);
            }
        }

    }

}
char * escreveNome(char *msg, int n_cliente)
{
    char    *temp;
    temp = (char *) malloc(sizeof(char)*(MSGSIZE + USERNAMESIZE + CMDSIZE + 1));
    strcpy(temp, clientes[n_cliente].nome);
    strcat(temp, msg);
    return temp;
}

void processaMsg(void)
{
//	declaração da estrutura de dados
    char					txbuffer[TX_BUFFER_SIZE + 1];
    char					msg[MSGSIZE + 1];
    char					cmd[CMDSIZE + 1];
    int						n_cliente;
    struct requisicao   	*R;

//  laço principal de tratamento
    while (continuar || !FilaVazia(Fila))
    {
        if (FilaVazia(Fila))
        {
            sleep(1);
        }
        else
        {
    //      retirar requisição da fila
            R = RetirarFila(Fila);
    //		quebrar msg
            recebeMsg(cmd, msg, MSGSIZE, R->buffer);
            printf("\nProcessando msg...\n");
    //		resolver clientes
            n_cliente = procuraCliente(R->addr);
            printf("Cliente #%d\n", n_cliente);
            if (n_cliente == -1)
                printf("Novo cliente\n");
            if (strcmp(cmd, CMD_USER) == 0)
            {
                if (insereCliente(R->addr, msg) == 1)
                {
    //				cliente inserido, devolver OKOK
                    escreveMsg(CMD_OKOK, "\0", 2, txbuffer);
                    printf("Cliente inserido\nmsg a enviar: %s\n", txbuffer);
                }
                else
                {
    //				erro, limite de clientes atingido
                    escreveMsg(CMD_BUSY, "\0", 2, txbuffer);
                    printf("Erro: limite de clientes do servidor atingido.\n");
                    printf("msg a enviar: %s\n", txbuffer);
                }
                enviarMsg(txbuffer, R->addr);
            }
            else if (n_cliente != -1)
            {
                if (strcmp(cmd, CMD_UP) == 0)
                {
    //				msg para broadcast

                    escreveMsg(CMD_DOWN, escreveNome(msg, n_cliente), strlen(msg), txbuffer);
                    printf("Comando CMD_UP\n");
                    printf("TXBUFFER: %s - %d\n", txbuffer, strlen(txbuffer));
                    printf("msg: %s - %d\n", msg, strlen(msg));
                    printf("cliente: %s\n", clientes[n_cliente].nome);
                    broadcastMsg(txbuffer);
                }
                else if (strcmp(cmd, CMD_EXIT) == 0)
                {
    //				msg exit para saida
                    printf("Removendo cliente...\n");
                    if(removeCliente(R->addr))
                        printf("Cliente removido!\n");
                    else
                        printf("Erro ao remover cliente!\n");
                    escreveMsg(CMD_EXIT, "\0", MSGSIZE, txbuffer);
                    printf("TXbuffer: %s - %d\n", txbuffer, strlen(txbuffer));
                    enviarMsg(txbuffer, R->addr);
                }
            }
            else
            {
                escreveMsg(CMD_OKOK, "\0", MSGSIZE, txbuffer);
                printf("Msg nao reconhecida, enviando CMD_OKOK\n");
                enviarMsg(txbuffer, R->addr);
            }
    //      broadcast test
    //        escreveMsg(CMD_TEST, "\0", MSGSIZE, txbuffer);
    //        broadcastMsg(txbuffer);
        }
    // fim do else
    }
//	fim do laço while
}

int main()
{
    int status;
//	criação de threads
    pthread_t receptor1;
    pthread_t proc_msg1;

//  inicialização e criação de threads
    continuar  = TRUE;
    inicializar();
    incializarClientes();
    Fila = InitFila();
    pthread_create(&receptor1, NULL, (void*)receptor, (void*)1);
    pthread_create(&proc_msg1, NULL, (void*)processaMsg, (void*)2);

//  espere por q para finalizar
    while (getchar() != 'q');
    printf("\n\nFinalizando, aguarde...\n");
    continuar = FALSE;
//  close socket, mas envie dados pendentes
    status = shutdown(socket_master, SHUT_RD);
//  verificação da chamada close
    if (status == -1)
    {
        perror("Erro na chamada shutdown");
    }

    sleep(3);
    status = close(socket_master);
//  termino do laço principal, aguardar threads
    pthread_join(receptor1, NULL);
    pthread_join(proc_msg1, NULL);

    putchar('\n');

    return 0;
}