/*
 * File: 		proj2.c
 * Author: 		Ricardo Antunes
 * Description: IAED project 2.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_INSTRUCTION_SIZE 65537

#define QUIT_COMMAND "quit"
#define HELP_COMMAND "help"


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

/*
 * Parses a path from a string, ignoring beginning and trailing whitespaces.
 * *path is set to a malloc'ed string containing the parsed path. A pointer to
 * the first character after the parsed path in the original string is returned.
 */
/*const char* parse_path(const char* str, char** path) {
	return NULL;
}*/

/*
 * Tries to parse a command from a string, ignoring beginning and trailing
 * whitespaces. If the command is found, a pointer to the first character after
 * the parsed command in the original string is returned. Otherwise, NULL is
 * returned.
 */
const char* read_command(const char* str, const char* command) {
	/* Skip whitespaces */
	for (; isspace(*str); ++str);
	/* Match the two strings until one ends */
	for (; *command == *str && *command != '\0'; ++command, ++str);
	/* Only succeed if the whole command string matches the input */
	return *command == '\0' ? str : NULL;
}

/*void read_value(const char* str, char** value) {
	return NULL;
}*/

int parse_instruction(const char* instruction) {
	/*const char* it;*/

	if (read_command(instruction, QUIT_COMMAND))
		return 0;
	else if (read_command(instruction, HELP_COMMAND))
		puts(HELP_MESSAGE);

	return 1;
}

/* Reads commands from stdin line by line and executes them. */
int main() {
	char instruction[MAX_INSTRUCTION_SIZE];

	do {
		/* Read instruction from stdin */
		fgets(instruction, MAX_INSTRUCTION_SIZE, stdin);
		if (instruction[strlen(instruction) - 1] != '\n') {
			puts(NO_MEMORY_ERROR);
			return 1;
		}
	} while(parse_instruction(instruction)); /* Parse instruction */

	return 0;
}