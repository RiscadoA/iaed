/*
 * File: 		proj2.c
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
char* trim_whitespaces(char* str) {
	int l;

	str += strspn(str, WHITESPACE_CHARS);
	l = strlen(str);
	for (; l > 0 && strchr(WHITESPACE_CHARS, str[l - 1]); --l)
		str[l - 1] = '\0';

	return str;
}

/* Parses and executes an instruction. */
int parse_instruction(char* instruction, struct fs* fs) {
	char* command, * path, * value;
	struct file* file;

	command = strtok(instruction, WHITESPACE_CHARS);

	if (strcmp(command, QUIT_COMMAND) == 0)
		return QUIT_CODE;
	else if (strcmp(command, HELP_COMMAND) == 0)
		puts(HELP_MESSAGE);
	else if (strcmp(command, SET_COMMAND) == 0) {
		path = strtok(NULL, WHITESPACE_CHARS);
		value = trim_whitespaces(strtok(NULL, ""));
		if (!file_set(fs, path, value))
			return NO_MEMORY_CODE;
	}
	else if (strcmp(command, PRINT_COMMAND) == 0)
		file_print(fs);
	else if (strcmp(command, FIND_COMMAND) == 0) {
		path = strtok(NULL, WHITESPACE_CHARS);
		if ((file = file_find(fs, path)) == NULL)
			puts(NOT_FOUND_ERROR);
		else
			puts(file_value(file) == NULL ? NO_DATA_ERROR : file_value(file));
	}
	else if (strcmp(command, LIST_COMMAND) == 0) {
		path = strtok(NULL, WHITESPACE_CHARS);
		if ((file = file_find(fs, path)) == NULL)
			puts(NOT_FOUND_ERROR);
		else
			file_list(file);
	}
	else if (strcmp(command, SEARCH_COMMAND) == 0) {
		value = trim_whitespaces(strtok(NULL, ""));
		if ((file = file_search(fs, value)) == NULL)
			puts(NOT_FOUND_ERROR);
		else {
			file_print_path(file);
			putchar('\n');
		}
	}
	else if (strcmp(command, DELETE_COMMAND) == 0) {
		path = strtok(NULL, WHITESPACE_CHARS);
		if (path == NULL)
			file_destroy(fs, NULL);
		else if ((file = file_find(fs, path)) == NULL)
			puts(NOT_FOUND_ERROR);
		else
			file_destroy(fs, file);
	}

	return SUCCESS_CODE;
}

/* Reads instructions from stdin line by line and executes them. */
int main() {
	int code;
	char instruction[MAX_INSTRUCTION_SIZE];
	struct fs* fs = filesystem_create();

	do {
		fgets(instruction, MAX_INSTRUCTION_SIZE, stdin);
		code = parse_instruction(instruction, fs);
	} while(code == SUCCESS_CODE);

	if (code == NO_MEMORY_CODE)
		puts(NO_MEMORY_ERROR);

	filesystem_destroy(fs);
	return 0;
}