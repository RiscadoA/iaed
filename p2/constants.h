/*
 * File: 		constants.h
 * Author: 		Ricardo Antunes
 * Description: All constants are defined in this file.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* The maximum number of characters in a instruction. */
#define MAX_INSTRUCTION_SIZE 65536

/* Number of cells in the value to file hash table, must be a prime. */
#define HASH_TABLE_SIZE 65537

/* Whitespace characters */
#define WHITESPACE_CHARS " \t\n"

/* Error codes for parse_instruction */
#define SUCCESS_CODE 0
#define QUIT_CODE 1
#define NO_MEMORY_CODE 2

/* Command names */
#define QUIT_COMMAND "quit"
#define HELP_COMMAND "help"
#define SET_COMMAND "set"
#define PRINT_COMMAND "print"
#define FIND_COMMAND "find"
#define LIST_COMMAND "list"
#define SEARCH_COMMAND "search"
#define DELETE_COMMAND "delete"

/* Error strings */
#define NO_MEMORY_ERROR "No memory."
#define NOT_FOUND_ERROR "not found"
#define NO_DATA_ERROR "no data"

/* Message written to stdin when HELP_COMMAND is executed */
#define HELP_MESSAGE \
	HELP_COMMAND	": Imprime os comandos disponíveis.\n"\
	QUIT_COMMAND	": Termina o programa.\n"\
	SET_COMMAND 	": Adiciona ou modifica o valor a armazenar.\n"\
	PRINT_COMMAND	": Imprime todos os caminhos e valores.\n"\
	FIND_COMMAND	": Imprime o valor armazenado.\n"\
	LIST_COMMAND 	": Lista todos os componentes imediatos de um sub-caminho."\
					"\n"\
	SEARCH_COMMAND	": Procura o caminho dado um valor.\n"\
	DELETE_COMMAND	": Apaga um caminho e todos os subcaminhos."

#endif