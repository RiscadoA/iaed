/*
 * File: 		constants.h
 * Author: 		Ricardo Antunes
 * Description: All constants are defined in this file.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_INSTRUCTION_SIZE 65537

#define QUIT_COMMAND "quit"
#define HELP_COMMAND "help"
#define SET_COMMAND "set"
#define PRINT_COMMAND "print"
#define FIND_COMMAND "find"
#define LIST_COMMAND "list"
#define SEARCH_COMMAND "search"
#define DELETE_COMMAND "delete"

#define NO_MEMORY_ERROR "No memory."
#define HELP_MESSAGE \
	"help: Imprime os comandos disponíveis.\n"\
	"quit: Termina o programa.\n"\
	"set: Adiciona ou modifica o valor a armazenar.\n"\
	"print: Imprime todos os caminhos e valores.\n"\
	"find: Imprime o valor armazenado.\n"\
	"list: Lista todos os componentes imediatos de um sub-caminho.\n"\
	"search: Procura o caminho dado um valor.\n"\
	"delete: Apaga um caminho e todos os subcaminhos."

#endif