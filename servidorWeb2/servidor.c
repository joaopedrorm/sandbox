#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "fila_reentrante.h"
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#define PATHSIZE    80
#define QLEN 10
#define BUFFERSIZE 1024
#define min(x,y)   (((x)<(y))?(x):(y))
#define LINESIZE        100
#define LISTBUFFERSIZE  1000

// socket do servidor
int sd;
struct fila F;
char root[PATHSIZE];

void initserver();
void aceita_req();
void processa();
void responde(int socket, char* rxbuffer);
int transferfile(char *path,int output_fd);
void listagem(char *path,int socket);
void append(char *dest,int buffersize, char *src);
void lista_diretorio(char *path,char *buffer,int buffersize);

int main(){

	int status;
	pthread_t aceita;
	pthread_t proc1;
	pthread_t proc2;
	pthread_t proc3;
	pthread_t proc4;

	InitFila(&F);
	initserver();

	//dispara os threads de processamento

	pthread_create(&proc1, NULL, (void*) processa, NULL);
	pthread_create(&proc2, NULL, (void*) processa, NULL);
	pthread_create(&proc3, NULL, (void*) processa, NULL);
	pthread_create(&proc4, NULL, (void*) processa, NULL);
	pthread_create(&aceita, NULL, (void*) aceita_req, NULL);

	pthread_join(proc1,NULL);
	pthread_join(proc2,NULL);
	pthread_join(proc3,NULL);
	pthread_join(proc4,NULL);
	pthread_join(aceita,NULL);

	// chamada close no socket do servidor
	status = close(sd);
	if(status == -1)
		perror("Erro na chamada close do servidor");

	printf("-Fez close no sd\n");

	sem_destroy(&(F.itens));
	sem_destroy(&(F.slots));
	sem_destroy(&(F.mutex));

	return 0;
}

void initserver(){

	int status;
	struct sockaddr_in local_addr;
	unsigned short myport;

	//Realiza abertura e leitura do arquivo de configuracao
	FILE *config;

	config = fopen("server_config.txt", "r");
	if(config == NULL){
		printf("Erro ao abrir arquivo de configuracao!\n");
		exit(1);
	}
	fscanf(config, "%hu", &myport);
	fscanf(config, "%s", root);
	fclose(config);
	printf("Configuracao:\nPorta: %hu\nRoot Directory: %s\n", myport, root);

	// inicializa socket e verifica erro
	sd = socket(PF_INET, SOCK_STREAM, 6);
	if (sd == -1)
		perror("Erro na chamada socket");

	local_addr.sin_family		= AF_INET;
	local_addr.sin_addr.s_addr	= INADDR_ANY;
	local_addr.sin_port		= htons (myport);

	// chamada bind com o socket do servidor
	status = bind(sd, (struct sockaddr *) &local_addr, sizeof(struct sockaddr_in));
	if (status == -1){
		perror("Erro na chamada bind");
		exit(1);
	}
	printf("-Fez bind\n");
	// chamada listen
	status = listen(sd, QLEN);
	if(status != 0){
		perror("Erro na chamada listen");
		exit(1);
	}

	printf("-Fez listen\n");

}

void aceita_req(){

	struct sockaddr_in client_addr;
	int newsd;
	int size;

	size = sizeof(client_addr);

	while(1){
		// chamada accept com o socket do cliente newsd

		newsd = accept(sd, (struct sockaddr *) &client_addr, (socklen_t *) &size);

		if (newsd < 0){
			perror("Erro na chamada accept");
			exit(1);
		}

		printf("-Fez accept\n");

		InserirFila(&F, newsd);
	}
}

void processa(){

	int status;
	int socket;
	char rxbuffer[BUFFERSIZE] = "\0";

	while(1){
	socket = RetirarFila(&F);

	// chamada read no socket do cliente newsd
	status = read(socket, rxbuffer, BUFFERSIZE);
	if(status == -1)
		perror("Erro na chamada read");
	printf("-Leu\n");

	responde(socket, rxbuffer);

	printf("-Respondeu\n");

	// chamada close no socket do cliente newsd
	status = close(socket);
	if(status == -1)
		perror("Erro na chamada close do cliente");
	printf("-Finalizou o socket do cliente\n");
	}
}

void responde(int socket, char* rxbuffer){
	char metodo[10] = "\0";
	char URL[PATHSIZE]  ="\0";
	char versao[20] = "\0";
	char txbuffer[BUFFERSIZE] = "\0";
	char path[PATHSIZE] = "\0";
	char newpath[PATHSIZE] = "\0";
	struct stat propr;

/*    printf("\n");
    fputs(rxbuffer, stdout);
    printf("\n");
*/
	strcpy(path, root);
	sscanf(rxbuffer, "%s %s %s", metodo, URL, versao);
	if(strcmp(metodo,"GET") == 0){
		if(strcmp(versao,"HTTP/1.0") == 0){

			strcat(path,URL);
			printf("%s\n", path);
			//procura e identifica se arquivo ou diretorio
			if(stat(path, &propr) == -1){
			//Nao encontrou o path, logo erro 404 Not Found
				strcpy(path, root);
				sprintf(txbuffer, "HTTP/1.0 404 Not Found\n");
				write(socket, txbuffer, strlen(txbuffer));
				strcat(path, "/erro/NotFound.html");
				printf("%s\n", path);
				transferfile(path, socket);
			}
			else{
				sprintf(txbuffer, "HTTP/1.0 200 OK\n");
				write(socket, txbuffer, strlen(txbuffer));
				switch (propr.st_mode & S_IFMT){
					//se diretorio gera listagem ou passa pagina index.html
					case S_IFDIR:
						if(path[strlen(path)-1] != '/')
							strcat(path,"/");
                        strcpy(newpath, path);
						strcat(newpath,"index.html");
						//procura index.html no diretorio, senao faz listagem
						if(stat(newpath, &propr) == -1){
						//faz listagem
							listagem(path, socket);
						}
						else{
							transferfile(newpath, socket);
						}
						break;

					//se arquivo transfere
					case S_IFREG:
						transferfile(path, socket);
						break;
				}
			}
		}
		else{
			//erro HTTP Version Not Supported
			strcpy(path, root);
			sprintf(txbuffer, "HTTP/1.0 505 HTTP Version Not Supported\n");
			write(socket, txbuffer, strlen(txbuffer));
			strcat(path, "/erro/noHTTP.html");
			transferfile(path, socket);
		}
	}
	else{
		//erro Bad Request
		strcpy(path, root);
		sprintf(txbuffer, "HTTP/1.0 400 Bad Request\n");
		write(socket, txbuffer, strlen(txbuffer));
		strcat(path,"/erro/BadRequest.html");
		transferfile(path, socket);
	}


}

int transferfile(char *path,int output_fd)
   {
   int          input_fd;     // input file descriptor
   int          status;
   int          n;
   char         buffer[BUFFERSIZE];
   char         str[100];
   struct stat  statp;

   input_fd = open(path,O_RDONLY);
   if (input_fd < 0)
      {
      perror("ERRO chamada open(): Erro na abertura do arquivo: ");
      return(-1);
      }

   // obtem tamanho do arquivo
   status = fstat(input_fd,&statp);
   if (status != 0)
      {
      perror("ERRO chamada stat(): Erro no acesso ao arquivo: ");
      status = close(input_fd);
      return(-1);
      }
   sprintf(str,"Content-Length: %jd\n\n",statp.st_size);
   write(output_fd,str,strlen(str));

   // le arquivo , por partes
   do
      {
      n = read(input_fd,buffer,BUFFERSIZE);
      if (n<0)
	   {
           perror("ERRO: chamada read(): Erro na leitura do arquivo: ");
           status = close(input_fd);
           return(-1);
           }
      write(output_fd,buffer,n);
      }
      while(n>0);

   status = close(input_fd);
   if (status == -1)
	   {
	   perror("ERRO: chamada close(): Erro no fechamento do arquivo: " );
           return(-1);
           }
   return(0);
   }

void listagem(char *path,int socket)
{
    char buffer[BUFFERSIZE] = "\0";
    char newpath[LINESIZE] = "\0";
    char *p;
    // pegar caminho absoluto
    p = getcwd(newpath,LINESIZE);
    if (p==NULL)
      perror("Erro na chamada getcwd");
    // concatenar path
    strcat(p, "/");
    strcat(p, path);

    printf("\n");
    fputs(p, stdout);
    printf("\n");

    sprintf(buffer, "\n<!DOCTYPE html><html><head><title>Diretorio</title><style type=\"text/css\">");
    sprintf(buffer + strlen(buffer), "<!--A{text-decoration:none}--></style></head>\n<body><hr><h2>%s</h2><menu>\n", path);

    lista_diretorio(p, buffer, BUFFERSIZE);

    sprintf(buffer + strlen(buffer), "</menu><hr></body></html>");
    write(socket, buffer, strlen(buffer));
}

void append(char *dest,int buffersize, char *src)
{
    int d;
    int i;

    d = strlen(dest);
    for (i=0; i<min(strlen(src),buffersize-1-d); i++)
        dest[d+i] = src[i];
    dest[d+i] = '\0';
}

// Lista diretorio
//    path: diretorio a ser listado
//    buffer: buffer que contera' a string com a sequencia ASCII
//            resultado da listagem do diretorio (finalizada por '\0'
//    bufffersize: tamanho do buffer

void lista_diretorio(char *path,char *buffer,int buffersize)
{
    DIR           * dirp;
    struct dirent * direntry;
    char            linha[LINESIZE] = "\0";
    char          * str_atual;
    char            newpath[LINESIZE] = "\0";

    dirp = opendir(path);
    if (dirp ==NULL)
    {
        perror("ERRO: chamada opendir(): Erro na abertura do diretorio: ");
        snprintf(buffer,buffersize,"Erro na listagem diretorio!\n");
        return;
    }
//    comentado pois a linha seguinte apaga o que jah foi escrito no buffer
//    buffer[0]='\0';
    str_atual = strstr(path, root);
    str_atual = strtok(str_atual, "/");
    // para descartar root
    str_atual = strtok(NULL, "/");
    strcpy(newpath, "/\0");
    while (str_atual != NULL)
        {
            strcat(newpath, str_atual);
            strcat(newpath, "/");
            str_atual = strtok(NULL, "/");
        }

    printf("\n%s\n", newpath);
//    ptr = ptr + strlen(root) + 1;
    while (dirp)
    {
        // Enquanto nao chegar ao fim do diretorio, leia cada entrada
        direntry = readdir(dirp);
        if (direntry == NULL)
            // chegou ao fim
            break;
        else if (strcmp(direntry->d_name, ".") == 0 || strcmp(direntry->d_name, "..") == 0)
        {
            // ler entrada
            sprintf(linha, "<li><a href=\"%s%s\">%s</a></li>", newpath, direntry->d_name, direntry->d_name);
            append(buffer,buffersize,linha);
            append(buffer,buffersize,"\n");
        }
        else
        {
            // ler entrada
            sprintf(linha, "<li><a href=\"%s%s\">%s</a></li>", newpath, direntry->d_name, direntry->d_name);
            append(buffer,buffersize,linha);
            append(buffer,buffersize,"\n");
        }
    }
    closedir(dirp);
}
