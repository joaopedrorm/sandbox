#!/bin/bash
#

gcc -std=c99 -Wall -pipe -O2 cliente.c -o cliente -lpthread -lrt
