#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "cliente.h"

#define DBG(x, args...) do{ if(debug == 1) printf(x, ## args); } while(0)
#define LST(x, args...) do{ if(lst == 1) printf(x, ## args); } while(0)

#define LISTEN_PORT 10000
#define CMD_LENGTH 5
#define NOME_LENGTH 10
#define MESG_LENGTH 80
#define BUFFER_TAM (CMD_LENGTH+MESG_LENGTH+1)

#define CABECALHO_NOME_LISTA_USUARIO "USER:"
#define CABECALHO_MENSAGEM "MESG:"
#define CABECALHO_COMANDO_DEBUG "DEBG:"
#define CABECALHO_COMANDO_LISTEN "LSTN:"
#define CABECALHO_COMANDO_ALTERA_LIMITE "ULMT:"
#define CABECALHO_COMANDO_CLIENTE_SAIR "EXIT:"

#define MSG_ID_ERRO_NULA "ERR0:"
#define MSG_ID_OK "OKOK:"
#define MSG_ID_CLEAR "UCLR:"
#define MSG_MESG_CLIENTE_CONECTADO "MESG:--%s entrou no chat"
#define MSG_MESG_CLIENTE_MUDOU_NOME "MESG:--%s agora é %s"
#define MSG_MESG_ID_EM_USO "MESG:--o nome solicitado já estava em uso!"
#define MSG_ID_ERRO_EMUSO "ERR1:"
#define MSG_MESG_ERRO_ID "ERR2:"
#define MSG_CMD_DESCONHECIDO "ERR4:"
#define MSG_BUSY "BUSY:"

void envia_dados(const char* mensagem);
int muda_nome_cliente(char* nome);
void envia_lista_usuarios();
unsigned int conta_usuarios();

static struct sockaddr fromaddr;
static struct sockaddr_in serversocketaddr;
static int status, socket_descriptor;
static Cliente cliente_head;
static Cliente cliente;
static int debug = 0;
static int lst = 1;
static unsigned int user_limit = 3;

int main()
{
    char stringIP[20];
    char rxbuffer[BUFFER_TAM];
    unsigned int size = sizeof(struct sockaddr_in);

    cliente_head = cliente_new();
    cliente_init(cliente_head, NULL);

    //Configura o socket
    serversocketaddr.sin_family = AF_INET;
    serversocketaddr.sin_addr.s_addr = INADDR_ANY;
    serversocketaddr.sin_port = htons(LISTEN_PORT);

    //Cria o socket
    socket_descriptor = socket(PF_INET, SOCK_DGRAM, 17);
    if (socket_descriptor == -1) {
        perror("Erro na chamada socket");
        exit(1);
    }

    //Espera por dados de clientes
    status = bind(socket_descriptor, (struct sockaddr *)&serversocketaddr, sizeof(struct sockaddr));
    if (status == -1) {
        perror("Erro na chamada bind");
        exit(1);
    }
    printf("---Servidor echo iniciado na porta %d\n", LISTEN_PORT);

    //loop principal do servidor
    while(1) {
        cliente = NULL;

        //Aguarda receber dados
        status = recvfrom(socket_descriptor, rxbuffer, sizeof(rxbuffer), 0, (struct sockaddr *)&fromaddr, &size);
        if (status == -1) {
            perror("Erro na chamada recvfrom");
            exit(1);
        }

        //Identifica o cliente e o adiciona a lista de clientes ativos, se necessário
        cliente = cliente_encontra_sessao(cliente_head, (struct sockaddr_in *)&fromaddr);
        if(cliente == NULL) {
            //Se o cliente ainda não tem uma sessão estabelecida e está tentando fazer algo que não é
            //definir o nome do usuário
            if( memcmp(CABECALHO_NOME_LISTA_USUARIO, rxbuffer, CMD_LENGTH) != 0 ) {
                envia_dados(MSG_MESG_ERRO_ID);
                DBG("Cliente tentou enviar comando sem definir o nome! %s\n", rxbuffer);
                continue;
            }
            else if(muda_nome_cliente(&rxbuffer[CMD_LENGTH]) != 1)
                continue;
            else if(conta_usuarios() >= user_limit && user_limit != 0) {
                envia_dados(MSG_BUSY);
                continue;
            }
            //Insere o cliente na lista de clientes com sessão ativa
            Cliente cliente_it;
            cliente = cliente_new();                                                                    //Cria o cliente,
            cliente_init(cliente, (const struct sockaddr_in*)&fromaddr);                                //o inicializa
            for(cliente_it = cliente_head; cliente_it->next != NULL; cliente_it = cliente_it->next);    //e o insere na
            cliente_it->next = cliente;                                                                 //lista ligada.

            cliente_ip(cliente, stringIP, &cliente->fromaddr);
            DBG("Nova mensagem recebida de %s, porta remota %hu\n", stringIP, ntohs(cliente->fromaddr.sin_port));
        }

        //Trata comandos de definição do nome do usuário
        if( memcmp(CABECALHO_NOME_LISTA_USUARIO, rxbuffer, CMD_LENGTH) == 0 )
            muda_nome_cliente(&rxbuffer[CMD_LENGTH]);
        //Trata comandos de envio de mensagem
        else if( memcmp(CABECALHO_MENSAGEM, rxbuffer, CMD_LENGTH) == 0 ) {
            char mensagem_formatada[BUFFER_TAM + NOME_LENGTH +3];
            //Envia a mensagem para todos os clientes ativos
            for ( Cliente cliente_it = cliente_head->next; cliente_it != NULL; cliente_it = cliente_it->next ) {
                memcpy(&fromaddr,&cliente_it->fromaddr,sizeof(fromaddr));
                memcpy(mensagem_formatada, rxbuffer, CMD_LENGTH);
                sprintf(&mensagem_formatada[CMD_LENGTH], "%s> %s\n", cliente->nome, &rxbuffer[CMD_LENGTH]);
                envia_dados(mensagem_formatada);
            }
            LST("%s> %s", cliente->nome, &rxbuffer[CMD_LENGTH]);
        }
        //Trata comandos ligar ou desligar o debug
        else if( memcmp(CABECALHO_COMANDO_DEBUG, rxbuffer, CMD_LENGTH) == 0 ) {
            debug = !debug;
            if(debug) {
                printf("--debug ligado\n");
                envia_dados("MESG:--debug do servidor ligado\n");
            }
            else {
                printf("--debug desligado\n");
                envia_dados("MESG:--debug do servidor desligado\n");
            }
        }
        //Trata comandos ligar ou desligar a escuta da conversa
        else if( memcmp(CABECALHO_COMANDO_LISTEN, rxbuffer, CMD_LENGTH) == 0 ) {
            lst = !lst;
            if(lst) {
                printf("--escuta da conversa ligada\n");
                envia_dados("MESG:--escuta da conversa do servidor ligada\n");
            }
            else {
                printf("--escuta da conversa desligada\n");
                envia_dados("MESG:--escuta da conversa do servidor desligada\n");
            }
        }
        //Trata comandos de mudança no limite de usuários simultâneos conectados
        else if( memcmp(CABECALHO_COMANDO_ALTERA_LIMITE, rxbuffer, CMD_LENGTH) == 0 ) {
            user_limit = (unsigned int)atoi(&rxbuffer[CMD_LENGTH]);
            if(user_limit != 0) {
                sprintf(rxbuffer,"MESG:--número limite de usuários do servidor alterado para %d\n", user_limit);
                LST("--Número limite de usuários alterado para %d\n", user_limit);
            }
            else {
                sprintf(rxbuffer,"MESG:--removido o limite no número de usuários do servidor\n");
                LST("--Número de usuários alterado para ilimitado\n");
            }
            envia_dados(rxbuffer);
        }
        //Trata comandos de desconexão
        else if(memcmp(CABECALHO_COMANDO_CLIENTE_SAIR, rxbuffer, CMD_LENGTH) == 0 ) {
            Cliente cliente_it;

            envia_dados(MSG_ID_OK);

            //Cria a mensagem de aviso de que o cliente saiu
            char mensagem_com_nome[BUFFER_TAM];
            sprintf(mensagem_com_nome, "MESG:--%s saiu do bate-papo.", cliente->nome);

            //Envia mensagem para os clientes
            for ( cliente_it = cliente_head->next; cliente_it != NULL; cliente_it = cliente_it->next ) {
                memcpy(&fromaddr, &cliente_it->fromaddr, sizeof(fromaddr));
                envia_dados(mensagem_com_nome);
            }
            LST("%s\n",&mensagem_com_nome[strlen("MESG:")]);

            //Remove o cliente da lista ligada
            for ( cliente_it = cliente_head; cliente_it->next != cliente; cliente_it = cliente_it->next );
            if(cliente_it == NULL) continue;
            cliente_it->next = cliente->next;
            free(cliente); //Apaga o cliente

            envia_lista_usuarios();
        }
        //Trata comandos desconhecidos
        else {
            envia_dados(MSG_CMD_DESCONHECIDO);
            DBG("Cliente TODO enviou um comando desconhecido: '%s'\n", rxbuffer);
        }
    }
}

//Envia uma mensagem.
void envia_dados(const char* msg_source)
{
    status = sendto(socket_descriptor, msg_source, strlen(msg_source)+1, 0, &fromaddr,sizeof(struct sockaddr));
    if (status == -1) {
        perror("Erro na chamada send!");
        exit(1);
    }
}

int muda_nome_cliente(char* nome)
{
    if( nome[0] == '\0' ) {
        envia_dados(MSG_ID_ERRO_NULA);
        DBG("Cliente de TODO tentou usar nome de comprimento nulo\n");
        return 0;
    }
    else if( cliente_encontra_nome(cliente_head, nome) == NULL ) {
        if(cliente != NULL) {
            envia_dados(MSG_ID_OK);
            //Avisa a todos os clientes ativos que o cliente tem um novo nome
            for ( Cliente cliente_it = cliente_head->next; cliente_it != NULL; cliente_it = cliente_it->next ) {
                memcpy(&fromaddr,&cliente_it->fromaddr,sizeof(fromaddr));
                if( cliente->nome[0] == '\0' ) {
                    char mensagem_com_nome[BUFFER_TAM];
                    sprintf(mensagem_com_nome, MSG_MESG_CLIENTE_CONECTADO, nome);
                    envia_dados(mensagem_com_nome);
                }
                else {
                    char mensagem_com_nome[BUFFER_TAM];
                    sprintf(mensagem_com_nome, MSG_MESG_CLIENTE_MUDOU_NOME, cliente->nome, nome);
                    envia_dados(mensagem_com_nome);
                }
            }
            strcpy(cliente->nome, nome);
            envia_lista_usuarios();
            DBG("Cliente de TODO tem novo nome: %s\n", cliente->nome);
        }
        return 1;
    }
    else {
        envia_dados(MSG_ID_ERRO_EMUSO);
        if(cliente != NULL)
            envia_dados(MSG_MESG_ID_EM_USO);
        DBG("Cliente TODO tentou usar um nome que já estava em uso: %s\n", nome);
        return 0;
    }
}

void envia_lista_usuarios()
{
    //Envia mensagem para os clientes
    for ( Cliente cliente_it = cliente_head->next; cliente_it != NULL; cliente_it = cliente_it->next ) {
        memcpy(&fromaddr, &cliente_it->fromaddr, sizeof(fromaddr));
        envia_dados(MSG_ID_CLEAR);
        for ( Cliente cliente_nome_it = cliente_head->next; cliente_nome_it != NULL; cliente_nome_it = cliente_nome_it->next ) {
            char mensagem[BUFFER_TAM];
            sprintf(mensagem, "%s%s", CABECALHO_NOME_LISTA_USUARIO, cliente_nome_it->nome);
            envia_dados(mensagem);
        }
    }
}

unsigned int conta_usuarios()
{
    unsigned int i = 0;
    for(Cliente cliente_it = cliente_head->next; cliente_it != NULL ; cliente_it = cliente_it->next)
        ++i;
    return i;
}
