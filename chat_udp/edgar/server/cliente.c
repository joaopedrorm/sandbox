#include <stdlib.h>
#include <string.h>

#include "cliente.h"


Cliente cliente_new()
{
    return malloc(sizeof(struct cliente));
}


void cliente_init(Cliente cliente, const struct sockaddr_in *fromaddr)
{
    cliente->next = NULL;
    cliente->nome[0] = '\0';
    if(fromaddr != NULL)
        memcpy(&cliente->fromaddr, fromaddr, sizeof(struct sockaddr_in));
}

Cliente cliente_encontra_nome(const Cliente cliente_head, const char* nome)
{
    Cliente cliente_it;
    for( cliente_it = cliente_head->next; cliente_it != NULL && strcmp(cliente_it->nome, nome) != 0; cliente_it = cliente_it->next);
    return cliente_it;
}

Cliente cliente_encontra_sessao(const Cliente cliente_head, const struct sockaddr_in *fromaddr)
{
    Cliente cliente_it;
    for( cliente_it = cliente_head->next; cliente_it != NULL && cliente_it->fromaddr.sin_port != fromaddr->sin_port; cliente_it = cliente_it->next);
    return cliente_it;
}

void cliente_ip(Cliente cliente, char* IP, struct sockaddr_in *fromaddr)
{
    inet_ntop(AF_INET, &fromaddr->sin_addr, IP, 20);
}
