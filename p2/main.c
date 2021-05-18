/*
 * File: 		main.c
 * Author: 		Ricardo Antunes
 * Description: Main source file, where commands are read, parsed and executed.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"
#include "adt.h"

/*
 * Removes beginning and trailing whitespaces from a string and returns it.
 * Since the returned address differs from the one passed to the function, the
 * resulting string must be freed using the original address.
 */
static char* trim_whitespaces(char* str) {
	int l;

	str += strspn(str, WHITESPACE_CHARS); /* Skip beginning whitespaces */
	l = strlen(str);
	/* Remove trailing whitespaces */
	for (; l > 0 && strchr(WHITESPACE_CHARS, str[l - 1]); --l)
		str[l - 1] = '\0';

	return str;
}

/* Auxiliar function to parse_instruction, parses a quit instruction */
static int parse_quit_instruction() {
	return QUIT_CODE;
}

/* Auxiliar function to parse_instruction, parses a help instruction */
static int parse_help_instruction() {
	puts(HELP_MESSAGE);
	return SUCCESS_CODE;
}

/* Auxiliar function to parse_instruction, parses a set instruction */
static int parse_set_instruction(struct fs* fs) {
	char* path = strtok(NULL, WHITESPACE_CHARS);
	char* value = trim_whitespaces(strtok(NULL, ""));

	return file_set(fs, path, value) ? SUCCESS_CODE : NO_MEMORY_CODE;
}

/* Auxiliar function to parse_instruction, parses a print instruction */
static int parse_print_instruction(struct fs* fs) {
	file_print(fs);
	return SUCCESS_CODE;
}

/* Auxiliar function to parse_instruction, parses a find instruction */
static int parse_find_instruction(struct fs* fs) {
	char* path = strtok(NULL, WHITESPACE_CHARS);
	struct file* file = file_find(fs, path);

	if (file == NULL)
		puts(NOT_FOUND_ERROR);
	else if (file_value(file) == NULL)
		puts(NO_DATA_ERROR);
	else
		puts(file_value(file));
	return SUCCESS_CODE;
}

/* Auxiliar function to parse_instruction, parses a list instruction */
static int parse_list_instruction(struct fs* fs) {
	char* path = strtok(NULL, WHITESPACE_CHARS);
	struct file* file = file_find(fs, path);

	if (file == NULL)
		puts(NOT_FOUND_ERROR);
	else
		file_list(file);
	return SUCCESS_CODE;
}

/* Auxiliar function to parse_instruction, parses a search instruction */
static int parse_search_instruction(struct fs* fs) {
	char* value = trim_whitespaces(strtok(NULL, ""));
	struct file* file = file_search(fs, value);

	if (file == NULL)
		puts(NOT_FOUND_ERROR);
	else {
		file_print_path(file);
		putchar('\n');
	}
	return SUCCESS_CODE;
}

/* Auxiliar function to parse_instruction, parses a delete instruction */
static int parse_delete_instruction(struct fs* fs) {
	char* path = strtok(NULL, WHITESPACE_CHARS);
	struct file* file;
	
	if (path == NULL)
		file_delete(fs, NULL); /* Delete every path except root */
	else if ((file = file_find(fs, path)) == NULL)
		puts(NOT_FOUND_ERROR); 
	else
		file_delete(fs, file);
	return SUCCESS_CODE;
}

/* Parses and executes an instruction. */
static int parse_instruction(char* instruction, struct fs* fs) {
	char* command = strtok(instruction, WHITESPACE_CHARS); /* Get command */

	/* Execute function which corresponds to the command read */
	if (strcmp(command, QUIT_COMMAND) == 0)
		return parse_quit_instruction();
	else if (strcmp(command, HELP_COMMAND) == 0)
		return parse_help_instruction();
	else if (strcmp(command, SET_COMMAND) == 0)
		return parse_set_instruction(fs);
	else if (strcmp(command, PRINT_COMMAND) == 0)
		return parse_print_instruction(fs);
	else if (strcmp(command, FIND_COMMAND) == 0)
		return parse_find_instruction(fs);
	else if (strcmp(command, LIST_COMMAND) == 0)
		return parse_list_instruction(fs);
	else if (strcmp(command, SEARCH_COMMAND) == 0)
		return parse_search_instruction(fs);
	else if (strcmp(command, DELETE_COMMAND) == 0)
		return parse_delete_instruction(fs);
	else
		return QUIT_CODE; /* Unknown function, unreachable in test conditions */
}

/* Reads instructions from stdin line by line and executes them. */
int main() {
	int code;
	char instruction[MAX_INSTRUCTION_SIZE];
	struct fs* fs = filesystem_create(); /* Initialize filesystem */

	/* Parse instructions */
	do {
		fgets(instruction, MAX_INSTRUCTION_SIZE, stdin);
		code = parse_instruction(instruction, fs);
	} while(code == SUCCESS_CODE);

	/* Program run out of memory */
	if (code == NO_MEMORY_CODE)
		puts(NO_MEMORY_ERROR);

	/* Cleanup */
	filesystem_destroy(fs);
	return 0;
}