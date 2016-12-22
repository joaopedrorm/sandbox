#ifndef CLIENTE_H
#define CLIENTE_H

#include <arpa/inet.h>
#include <pthread.h>

typedef struct cliente {
    struct sockaddr_in fromaddr;
    char nome[11];
    struct cliente *next;
} *Cliente;

//Aloca uma estrutura Cliente
Cliente cliente_new();

//Inicializa uma estrutura Cliente
void cliente_init(Cliente cliente, const struct sockaddr_in *fromaddr);

//Dada uma sessão, encontra um cliente numa lista ligada de clientes
Cliente cliente_encontra_sessao(const Cliente cliente_head, const struct sockaddr_in *fromaddr);

//Dado um nome, encontra um cliente numa lista ligada de clientes
Cliente cliente_encontra_nome(const Cliente cliente_head, const char* nome);

//Escreve o endereço IP do cliente na string IP
void cliente_ip(Cliente cliente, char* IP, struct sockaddr_in *fromaddr);

#endif //CLIENTE_H
