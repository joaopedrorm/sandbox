#define _POSIX_C_SOURCE 199309L

#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>

#define NOME_PROG "Chat UDP"
#define LISTEN_PORT 10000
#define MSG_MAX 90
#define NOME_MAX 11
#define RECEIVER_TIMEOUT 3
#define LOGIN_OK 1
#define LOGIN_USER_EXISTS 2
#define LOGIN_RECV_ERROR 3
#define LOGIN_USER_LIMIT 4

#define CABECALHO_NOME_LISTA_USUARIO "USER:"
#define CABECALHO_MENSAGEM "MESG:"
#define CABECALHO_COMANDO_DEBUG "DEBG:"
#define CABECALHO_COMANDO_LISTEN "LSTN:"
#define CABECALHO_COMANDO_ALTERA_LIMITE "ULMT:"
#define CABECALHO_COMANDO_CLIENTE_SAIR "EXIT:"
#define CABECALHO_LEN 5

#define CMD_QUIT "\\quit"
#define CMD_DEBUG "\\debug"
#define CMD_NAME "\\name "
#define CMD_SRV_DBG "\\serverdebug"
#define CMD_SRV_LST "\\serverlisten"
#define CMD_SRV_USR_LMT "\\serveruserlimit "
#define CMD_HELP "\\help"

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

#define POS_INICIO do{ printf("\033[H"); fflush(stdout); } while(0)
#define CUR_CIMA do{ printf("\033[A"); fflush(stdout); } while(0)
#define CUR_BAIXO do{ printf("\033[B"); fflush(stdout); } while(0)
#define CUR_DIR do{ printf("\033[C"); fflush(stdout); } while(0)
#define CUR_ESQ do{ printf("\033[D"); fflush(stdout); } while(0)
#define CUR_SALVA do{ printf("\0337"); fflush(stdout); } while(0)
#define CUR_RESTAURA do{ printf("\0338"); fflush(stdout); } while(0)
#define POS_IMPRESSAO do{ POS_INICIO; for(int i = 0; i < linha-1; ++i) CUR_BAIXO; CUR_DIR; fflush(stdout); } while(0)
#define POS_LISTA_USR do{                                           \
    POS_INICIO;                                                     \
    for(int i = 0; i < linha-1; ++i)                                \
        CUR_BAIXO;                                                  \
    for(int i = 0; i < w.ws_col-NOME_MAX; ++i)                      \
        CUR_DIR;                                                    \
    } while(0)
#define POS_CAMPO do{ POS_INICIO; for(int i = 0; i < linhas_max-2; ++i) CUR_BAIXO; CUR_DIR; } while(0)
#define DBG(x, args...) do{ if(debug == 1) escreve(x, ## args); } while(0)
#define APAGA_TELA_CUR do{ printf("\033[J"); } while(0)

typedef struct usr {
    char nome[11];
    struct usr *next;
} *UsrNome;


static struct sockaddr_in serversockeaddr;
static int socket_descriptor;
static int linha = 2;
static struct winsize w;
static char** historico = 0;
static unsigned int linhas_max = 0;
static unsigned int linha_tam = 0;
static UsrNome usrNome_head;
static int login = 0;
static int debug = 0;

void desenha_janela();
void receiver_func(void* unused);
void envia_dados(const char* msg_source);
char* usrNome_at(int pos);
void usrNome_escreve_lista();
void scroll();

static pthread_mutex_t imprimindo;
static pthread_cond_t  receiver_message;
static pthread_mutex_t receiver_message_mutex;

void escreve(char* x, ...)
{
    va_list args;

    //Trava o mutex, só 1 thread pode imprimir de cada vez
    int pegou_o_mutex = !pthread_mutex_trylock(&imprimindo);
    //Adquire as características do terminal na struct w
    ioctl(0, TIOCGWINSZ, &w);

    //Caso o terminal tenha sido redimensionado, redesenha a tela
    if(linhas_max != w.ws_row || linha_tam != w.ws_col+1) {
        desenha_janela();
        linha = 2;
    }

    //Imprime o texto na variável historico[linha]
    va_start(args, x);
    vsprintf(historico[linha],x, args);
    va_end(args);

    //Caso o texo não caiba na janela, quebra em várias linhas
    while(strlen(historico[linha]) > linha_tam-NOME_MAX-3) {
        int i;

        //Realiza o scroll se necessário
        if(linha >= linhas_max-3) {
            scroll();
            --linha;
        }

        for(i = linha_tam-NOME_MAX-3; i > 0 ; --i)
            if(historico[linha][i] == ' ') {
                strcpy(historico[linha+1],&historico[linha][i+1]);
                break;
            }
        if(i == 0) {
            i = linha_tam-NOME_MAX-3;
            strcpy(historico[linha+1],&historico[linha][i]);
        }

        historico[linha][i] = '\n';
        historico[linha][i+1] = '\0';

        POS_IMPRESSAO;
        printf("%s", historico[linha]);
        ++linha;
    }

    //Posiciona o cursor na área de mensagens recebidas e imprime o texto
    POS_IMPRESSAO;
    printf("%s", historico[linha]);

    //Imprime o usuário correspondente na posição dos nomes de usuário
    if(usrNome_at(linha) != NULL) {
        POS_LISTA_USR;
        printf("%s", usrNome_at(linha));
    }
    ++linha;
    fflush(stdout);

    //Realiza o scroll se necessário
    if(linha >= linhas_max-2)
        scroll();

    //Destrava o mutex para outros threads poderem imprimir
    if(pegou_o_mutex)
        pthread_mutex_unlock(&imprimindo);
}

int main()
{
    pthread_t receiver = NULL;
    int status;
    char entrada_texto[MSG_MAX];
    char ip[16];

    pthread_mutex_init(&imprimindo,NULL);
    pthread_mutex_init(&receiver_message_mutex,NULL);
    pthread_cond_init(&receiver_message,NULL);

    ioctl(0, TIOCGWINSZ, &w);
    desenha_janela();

    usrNome_head = malloc(sizeof(struct usr));
    usrNome_head->next = NULL;

    //Boas vindas
    escreve("--Bem-vindo ao cliente de chat UDP--\n");

    //Cria o socket
    socket_descriptor = socket(PF_INET, SOCK_DGRAM, 17);
    if(socket_descriptor == -1) {
        escreve("Erro na chamada socket");
        exit(EXIT_FAILURE);
    }

    //Conecta-se ao servidor
    do {
        struct timespec ts;

        //Endereço do servidor
        escreve("-Qual é o endereço IP do servidor? (ENTER para loopback)\n");
        escreve("IP: ");
        fgets(entrada_texto, 16, stdin);
        if(entrada_texto[0] != '\n') {
            strcpy(ip,entrada_texto);
            ip[strlen(ip)-1] = '\0';
        }
        else
            memcpy(ip, "127.0.0.1", 16);

        //Configura o socket
        serversockeaddr.sin_family = AF_INET;
        serversockeaddr.sin_port = htons(LISTEN_PORT);
        if(inet_pton(AF_INET,ip, &serversockeaddr.sin_addr) != 1) {
            escreve("Endereço IP inválido! Tente de novo.");
            continue;
        }

        //Conecta ao servidor
        status = connect( socket_descriptor, (struct sockaddr *)&serversockeaddr, sizeof(serversockeaddr));

        //Cria o thread que recebe as mensagens enviadas pelo servidor
        pthread_create(&receiver, NULL, (void*)receiver_func, NULL);

        //Obtém o nome do usuário
        escreve("-Que nome gostaria de usar no chat?\n");
        escreve("Nome: ");
        fgets(&entrada_texto[CABECALHO_LEN], NOME_MAX-1, stdin);
        if(entrada_texto[CABECALHO_LEN] == '\n' || entrada_texto[CABECALHO_LEN] == ' ') {
            escreve("O nome não pode começar com ENTER ou espaço. Tente novamente.");
            continue;
        }
        entrada_texto[CABECALHO_LEN+strlen(&entrada_texto[CABECALHO_LEN])-1] = '\0';
        memcpy(entrada_texto,CABECALHO_NOME_LISTA_USUARIO,CABECALHO_LEN);

        //Envia o nome do usuário para o servidor
        envia_dados(entrada_texto);

        //Aguarda a resposta do servidor por até RECEIVER_TIMEOUT segundos
        ts.tv_sec = time(NULL) + 3L;//RECEIVER_TIMEOUT;
        pthread_cond_timedwait(&receiver_message, &receiver_message_mutex, &ts);

        //O servidor não respondeu a tempo. Pode ser que não haja um servidor ouvindo no endereço especificado...
        if(login == 0)
            escreve("O servidor não respondeu a tempo. Por favor, verifique o endereço digitado de tente de novo.\n");
        //O nome solicitado já está em uso no servidor
        else if(login == LOGIN_USER_EXISTS)
            escreve("O nome de usuário solicitado já está em uso no servidor! Tente de novo.\n");
        //O número de usuários no servidor foi excedido
        else if(login == LOGIN_USER_LIMIT)
            escreve("O número de usuários no servidor está excedido! Tente de novo mais tarde.\n");
        //Erro ao tentar ouvir no socket
        else if(login == LOGIN_RECV_ERROR)
            escreve("Erro na conexão. Por favor, verifique o endereço digitado e tente de novo.\n");
        if(login != LOGIN_OK) {
            pthread_mutex_unlock(&receiver_message_mutex);
            pthread_cancel(receiver);
        }
    } while(login != LOGIN_OK);

    //Posiciona o cursor no campo de texto
    pthread_mutex_lock(&imprimindo);
    POS_CAMPO;
    pthread_mutex_unlock(&imprimindo);
    //Lê o que o usuário digita e transmite para o servidor
    while(status >= 0) {
        char txbuffer[MSG_MAX];
        char input[MSG_MAX];

        //Lê a entrada do usuário
        fgets(input, 80, stdin);

        //Limpa o campo de texto
        pthread_mutex_lock(&imprimindo);
        POS_CAMPO;
        CUR_ESQ;
        printf("|");
        for(int i = 0; i < w.ws_col-2; ++i) printf(" ");
        printf("|\n");
        pthread_mutex_unlock(&imprimindo);

        if( memcmp(CMD_QUIT, input, strlen(CMD_QUIT)) == 0 ) {
            //Envia o comando de saída para o servidor
            strcpy(txbuffer, CABECALHO_COMANDO_CLIENTE_SAIR);
            envia_dados(txbuffer);
            pthread_mutex_lock(&imprimindo);
            APAGA_TELA_CUR;
            fflush(stdout);
            printf("Obrigado por usar o Chat UDP!\n");
            exit(EXIT_SUCCESS);
        }
        else if( memcmp(CMD_DEBUG, input, strlen(CMD_DEBUG)) == 0 ) {
            if(debug) {
                escreve("--Modo debug desativado\n");
                debug = 0;
            }
            else {
                escreve("--Modo debug ativado\n");
                debug = 1;
            }
            pthread_mutex_lock(&imprimindo);
            POS_CAMPO;
            pthread_mutex_unlock(&imprimindo);
            continue;
        }
        else if( memcmp(CMD_NAME, input, strlen(CMD_NAME)) == 0 ) {
            strcpy(txbuffer, CABECALHO_NOME_LISTA_USUARIO);
            strcpy(&txbuffer[strlen(CABECALHO_NOME_LISTA_USUARIO)], &input[strlen(CMD_NAME)]);
            txbuffer[strlen(txbuffer)-1] = '\0';

            //Remove o \n final
            txbuffer[strlen(txbuffer)] = '\0';
        }
        else if( memcmp(CMD_SRV_DBG, input, strlen(CMD_SRV_DBG)) == 0 )
            strcpy(txbuffer, CABECALHO_COMANDO_DEBUG);
        else if( memcmp(CMD_SRV_LST, input, strlen(CMD_SRV_LST)) == 0 )
            strcpy(txbuffer, CABECALHO_COMANDO_LISTEN);
        else if( memcmp(CMD_SRV_USR_LMT, input, strlen(CMD_SRV_USR_LMT)) == 0 ) {
            int number = atoi(&input[strlen(CMD_SRV_USR_LMT)]);
            if(number < 0) {
                pthread_mutex_lock(&imprimindo);
                escreve("--Erro: número não suportado!\n");
                POS_CAMPO;
                pthread_mutex_unlock(&imprimindo);
                continue;
            }
            sprintf(txbuffer, CABECALHO_COMANDO_ALTERA_LIMITE "%d", number);
        }
        else if( memcmp(CMD_HELP, input, strlen(CMD_HELP)) == 0 ) {
            pthread_mutex_lock(&imprimindo);
            escreve("--Lista de comandos\n");
            escreve("- " CMD_NAME "<novo_nome> - trocar nome do usuário\n");
            escreve("- " CMD_HELP " - mostrar essa ajuda\n");
            escreve("- " CMD_DEBUG " - habilitar mensagens de debug\n");
            escreve("- " CMD_SRV_DBG " - habilitar mensagens de debug no servidor\n");
            escreve("- " CMD_SRV_LST " - habilitar escuta da conversa no servidor\n");
            escreve("- " CMD_SRV_USR_LMT "<#> - altera o limite de usuários no servidor\n");
            escreve("- " CMD_QUIT " - sair do chat\n");
            escreve("--");
            POS_CAMPO;
            pthread_mutex_unlock(&imprimindo);
            continue;
        }
        else if( input[0] == '\\') {
            pthread_mutex_lock(&imprimindo);
            escreve("--Erro: comando não reconhecido\n");
            POS_CAMPO;
            pthread_mutex_unlock(&imprimindo);
        }
        else {
            strcpy(txbuffer, CABECALHO_MENSAGEM);
            strcpy(&txbuffer[strlen(CABECALHO_MENSAGEM)], input);

            //Remove o \n final
            txbuffer[strlen(txbuffer)] = '\0';
        }
        //Reposiciona o cursor
        POS_CAMPO;
        //Envia a mensagem
        envia_dados(txbuffer);
    }

    status = close(socket_descriptor);
    if (status == -1)
        perror("Erro na chamada close");
}

void desenha_janela()
{
    int pegou_o_mutex = !pthread_mutex_trylock(&imprimindo);

    if(historico != 0) {
        for (int i = 0; i < linhas_max; ++i) {
            free(historico[i]);
        }
        free(historico);
    }
    linhas_max = w.ws_row;
    linha_tam = w.ws_col+1;
    historico = malloc(2*linhas_max*sizeof(char*));
    for(int i = 0; i < linhas_max; ++i) {
        historico[i] = malloc((linha_tam+2)*sizeof(char));
        historico[i][0] = '\0';
    }

    APAGA_TELA_CUR;
    for(int i = 0; i < w.ws_row -3; ++i) {
        printf("|");
        for(int j = 0; j < w.ws_col - NOME_MAX-2; ++j) printf(" ");
        printf("|");
        for(int j = 0; j < NOME_MAX -1; ++j) printf(" ");
        printf("|\n");
    }
    printf("|");
    for(int j = 0; j < w.ws_col - NOME_MAX-2; ++j) printf("_");
    printf("|");
    for(int j = 0; j < NOME_MAX -1; ++j) printf("_");
    printf("|\n");
    printf("|");
    for(int i = 0; i < w.ws_col-2; ++i) printf(" ");
    printf("|\n");
    printf("|");
    for(int i = 0; i < w.ws_col-2; ++i) printf("_");
    printf("|");
    POS_INICIO;
    //Barra superior
    printf("__"NOME_PROG);
    for(int i = 0; i < w.ws_col -strlen("__"NOME_PROG); ++i) printf("_");
    printf("\n");

    if(pegou_o_mutex)
        pthread_mutex_unlock(&imprimindo);
}

//Realiza o scroll das mensagens na tela
void scroll()
{
    linha = 2;
    for(int i = 2; i < linhas_max-2;++i) {
        strcpy(historico[i-1],historico[i]);
        linha = i;
        POS_IMPRESSAO;
        CUR_ESQ;
        printf("|");
        for(int j = 0; j < w.ws_col - NOME_MAX-2; ++j) printf(" ");
        printf("|");
        for(int j = 0; j < NOME_MAX -1; ++j) printf(" ");
        printf("|\n");
        POS_IMPRESSAO;
        printf("%s", historico[i-1]);
        if(usrNome_at(linha) != NULL) {
            POS_LISTA_USR;
            printf("%s", usrNome_at(linha++));
        }
    }
}

//Função principal do thread que recebe as mensagens do servidor
void receiver_func(void* unused)
{
    int status, old_type;
    char rxbuffer[MSG_MAX];

    (void)unused; //evita que o compilador reclame que a variável não é usada
    login = 0;    //Não estabelecemos ainda uma sessão com o servidor

    //Esse thread pode ser destruído em qualquer ponto da sua execução
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old_type);

    while(1) {
        //Aguarda mensagens do servidor
        status = recv(socket_descriptor, rxbuffer, sizeof(rxbuffer), 0);
        if (status < 0) { //Não conseguimos ouvir no socket...
            DBG("Erro na chamada recv\n");
            login = LOGIN_RECV_ERROR;
            continue;
        }

        //Posiciona o cursor na área das mensagens recebidas
        pthread_mutex_lock(&imprimindo);

        //Salva a posição do cursor
        CUR_SALVA;
        POS_IMPRESSAO;

        //--Trata as mensagens recebidas
        //Nome do usuário modificado com sucesso
        if(memcmp(MSG_ID_OK, rxbuffer, strlen(MSG_ID_OK)) == 0)
            login = LOGIN_OK;
        //Erro: o nome de usuário já estava em uso
        else if(memcmp(MSG_ID_ERRO_EMUSO, rxbuffer, strlen(MSG_ID_ERRO_EMUSO)) == 0)
            login = LOGIN_USER_EXISTS;
        //Erro: número de usuários excedido no servidor
        else if(memcmp(MSG_BUSY, rxbuffer, strlen(MSG_BUSY)) == 0)
            login = LOGIN_USER_LIMIT;
        //Mensagem do chat recebida
        else if(memcmp(CABECALHO_MENSAGEM, rxbuffer, strlen(CABECALHO_MENSAGEM)) == 0)
            escreve("%s\n", &rxbuffer[strlen(CABECALHO_MENSAGEM)]);
        //Apagar lista local de usuários
        else if(memcmp(MSG_ID_CLEAR, rxbuffer, strlen(MSG_ID_CLEAR)) == 0) {
            DBG(MSG_ID_CLEAR "\n");
            for(UsrNome usrNome_it = usrNome_head->next; usrNome_it != NULL; ) {
                UsrNome next = usrNome_it->next;
                free(usrNome_it);
                usrNome_it = next;
            }
            usrNome_head->next = NULL;
        }
        //Adicionar o usuário à lista local de usuários
        else if(memcmp(CABECALHO_NOME_LISTA_USUARIO, rxbuffer, strlen(CABECALHO_NOME_LISTA_USUARIO)) == 0) {
            UsrNome usrNome_last;
            DBG(CABECALHO_NOME_LISTA_USUARIO "%s\n", &rxbuffer[strlen(CABECALHO_NOME_LISTA_USUARIO)]);
            for(usrNome_last = usrNome_head; usrNome_last->next != NULL; usrNome_last = usrNome_last->next);
            usrNome_last->next = malloc(sizeof(struct usr));
            usrNome_last->next->next = NULL;
            strcpy( usrNome_last->next->nome, &rxbuffer[strlen(CABECALHO_NOME_LISTA_USUARIO)] );
            usrNome_escreve_lista();
        }
        else
            //Comando não reconhecido recebidp
            DBG("Comando desconhecido recebido: %s\n", rxbuffer);

        //Volta o cursor para a posição anterior
        CUR_RESTAURA;
        pthread_mutex_unlock(&imprimindo);

        //Sinaliza que chegou uma nova mensagem. Usado pelo thread principal para verificar a variável login
        pthread_cond_signal(&receiver_message);
    }
}

//Envia uma mensagem. Os 4 primeiros bytes da mensagem são reservados para o número da sessão.
void envia_dados(const char* msg_source)
{
    int status;

    status = send(socket_descriptor, msg_source, strlen(msg_source)+1, 0);
    if (status == -1) {
        perror("Erro na chamada send!");
        exit(1);
    }
}

char* usrNome_at(int pos)
{
    int i = 0;
    UsrNome usrNome_it;
    for(usrNome_it = usrNome_head->next; i < pos-2 && usrNome_it != NULL; ++i)
        usrNome_it = usrNome_it->next;
    if(usrNome_it == NULL)
        return NULL;

    return usrNome_it->nome;
}

void usrNome_escreve_lista()
{
    int linha_ant = linha;
    UsrNome usrNome_it;

    //Apaga os nomes de todos os usuários da tela
    for(linha = 2; linha != linhas_max-2 ; ++linha) {
        POS_LISTA_USR;
        printf("          ");
    }

    //Escreve o nome dos usuários na tela
    linha = 2;
    for(usrNome_it = usrNome_head->next; usrNome_it != NULL; usrNome_it = usrNome_it->next) {
        POS_LISTA_USR;
        if(usrNome_at(linha) != NULL)
            printf("%s", usrNome_at(linha++));
    }
    linha = linha_ant;
}