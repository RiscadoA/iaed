/*
 * File: 		proj2.c
 * Author: 		Ricardo Antunes
 * Description: IAED project 2.
 */

#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "tad.h"

/*
 * Parses a path from a string, ignoring beginning and trailing whitespaces.
 * *path is set to a malloc'ed string containing the parsed path. A pointer to
 * the first character after the parsed path in the original string is returned.
 */
const char* parse_path(const char* str, char** path) {
	return NULL;
}

/*
 * Tries to parse a command from a string, ignoring beginning and trailing
 * whitespaces. If the command is found, a pointer to the first character after
 * the parsed command in the original string is returned. Otherwise, NULL is
 * returned.
 */
const char* parse_command(const char* str, const char* command) {
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

	if (parse_command(instruction, QUIT_COMMAND))
		return 0;
	else if (parse_command(instruction, HELP_COMMAND))
		puts(HELP_MESSAGE);

	return 1;
}

/* Reads commands from stdin line by line and executes them. */
int main() {
	char instruction[MAX_INSTRUCTION_SIZE];

	do {
		/* Read instruction from stdin */
		fgets(instruction, MAX_INSTRUCTION_SIZE - 1, stdin);
		if (instruction[strlen(instruction) - 1] != '\n') {
			/* This happens if the line doesn't fit inside the buffer */
			puts(NO_MEMORY_ERROR);
			return 1;
		}
	} while(parse_instruction(instruction)); /* Parse instruction */

	return 0;
}