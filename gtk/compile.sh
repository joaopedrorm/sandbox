#!/bin/sh
if [ $# -gt 0 ]; then
	F=`echo $1 | cut -d. -f 1`
	echo "Compilando $F..."
	gcc `pkg-config --cflags gtk+-3.0` -Wall -o $F $1 `pkg-config --libs gtk+-3.0`
else
	echo "Uso: $0 Arquivo.c"
fi
