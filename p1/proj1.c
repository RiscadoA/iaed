/*
 * File: 		proj1.c
 * Author: 		Ricardo Antunes
 * Description: IAED project 1.
 */

#include <stdio.h>
#include <string.h>

/* The maximum number of characters in a task description */
#define TASK_DESC_SZ 50
/* The maximum number of tasks supported */
#define MAX_TASK_COUNT 10000

/* The maximum number of characters used in an username */
#define USER_NAME_SZ 20
/* The maximum number of users supported */
#define MAX_USER_COUNT 50

/* The maximum number of characters used to describe an activity */
#define ACTIVITY_DESC_SZ 20
/* The maximum number of activities supported */
#define MAX_ACTIVITY_COUNT 10

/* Status codes used by the main and read command functions */
#define STATUS_OK 0
#define STATUS_ERR -1

/* Contains information about a task */
struct task {
	/* Task identifier */
	int id;
	/* Task description */
	char desc[TASK_DESC_SZ];
	/* User who is responsible for the task */
	char user[USER_NAME_SZ];
	/* Activity where the task is placed */
	char activity[ACTIVITY_DESC_SZ];
	/* Predicted task completion duration */
	int duration;
	/* The time the task started being executed */
	int start;
};

/* Contains information about a kanban board */
struct kanban {
	/* Tasks in the kanban board */
	struct task tasks[MAX_TASK_COUNT];
	/* Number of tasks in the kanban board */
	int task_count;
	/* Users in the kanban board */
	char users[MAX_USER_COUNT][USER_NAME_SZ];
	/* Activities in the kanban board */
	char activities[MAX_ACTIVITY_COUNT][ACTIVITY_DESC_SZ];
	/* The current time */
	int time;
};

/* Initializes a kanban board */
void init_kanban(struct kanban* board) {
	int i;
	
	board->task_count = 0;
	for (i = 0; i < MAX_USER_COUNT; ++i) {
		board->users[i][0] = '\0';
	}

	strncpy(board->activities[0], "TO DO", ACTIVITY_DESC_SZ);
	strncpy(board->activities[1], "IN PROGRESS", ACTIVITY_DESC_SZ);
	strncpy(board->activities[2], "DONE", ACTIVITY_DESC_SZ);
	for (i = 3; i < MAX_ACTIVITY_COUNT; ++i) {
		board->activities[i][0] = '\0';
	}

	board->time = 0;
}

/* Adds a task to a kanban board */
int add_task(struct kanban* board, int duration, char desc[]) {
	int i;
	struct task* task;

	/* Check if there are too many tasks */
	if (board->task_count == MAX_TASK_COUNT) {
		puts("too many tasks");
		return -1;
	}
	
	/* Check if there is already a task with this description */
	for (i = 0; i < board->task_count; ++i) {
		if (strncmp(board->tasks[i].desc, desc, TASK_DESC_SZ) == 0) {
			puts("duplicate description");
			return -1;
		}
	}

	/*
		The task array is always sorted, so we need to find where to insert a
		new element. This could be done using binary search, but since we're
		going to have to move the array forward it will be O(N) anyway.
		(assuming memmove is O(N)) So, a simple linear search will suffice.
	*/

	for (i = 0; i < board->task_count; ++i) {
		if (strncmp(board->tasks[i].desc, desc, TASK_DESC_SZ) > 0) {
			/* If the stored task is 'larger', insert the new one before it. */
			break;
		}
	}

	/* Move array forward to make space for new task */
	memmove(
		&board->tasks[i + 1],
		&board->tasks[i],
		board->task_count * sizeof(struct task)
	);

	/* Add task to board */
	task = &board->tasks[i];
	task->id = ++board->task_count;
	strncpy(task->desc, desc, TASK_DESC_SZ);
	/* No user assigned yet */
	task->user[0] = '\0';
	strncpy(task->activity, "TO DO", ACTIVITY_DESC_SZ);
	task->start = 0;
	task->duration = duration;

	return task->id;
}


/* Adds a user to a kanban board */
int add_user(struct kanban* board, char name[]) {
	int i;

	/* Search for an empty slot and check if the name is duplicated */
	for (i = 0; board->users[i][0] != '\0' && i < MAX_USER_COUNT; ++i) {
		if (strncmp(board->users[i], name, USER_NAME_SZ) == 0) {
			printf("user already exists\n");
			return 0;
		}
	}

	if (i >= MAX_USER_COUNT) {
		printf("too many users\n");
		return 0;
	}

	strncpy(board->users[i], name, USER_NAME_SZ);
	return 1;
}

/* Gets a pointer to a task from its id */
struct task* find_task(struct kanban* board, int id) {
	int i;
	
	for (i = 0; i < board->task_count; ++i) {
		if (board->tasks[i].id == id) {
			return &board->tasks[i];
		}
	}

	return NULL;
}

/* Prints a task */
void print_task(struct task* task) {
	/* Format: <id> <activity> #<duration> <description> */
	printf(
		"%d %.*s #%d %.*s\n",
		task->id,
		ACTIVITY_DESC_SZ, task->activity,
		task->duration,
		TASK_DESC_SZ, task->desc
	);
}

/* Print all of the tasks in lexicographical order */
void list_tasks(struct kanban* board) {
	int i;
	for (i = 0; i < board->task_count; ++i) {
		print_task(&board->tasks[i]);
	}
}

/* Print all of the users in creation order */
void list_users(struct kanban* board) {
	int i;
	for (i = 0; board->users[i][0] != '\0' && i < MAX_USER_COUNT; ++i) {
		printf("%.*s\n", USER_NAME_SZ, board->users[i]);
	}
}

/* Reads either a task or activity description from stdin */
void read_desc(char desc[], int size) {
	/* Index of the last character read */
	int index = -1;
	/* Index of the last non whitespace character */
	int last_nws = 0;
	int c;

	while ((c = getchar()) != '\n') {
		if (c == ' ' || c == '\t') {
			/* Trim whitespace at the beginning of the stream */
			if (index != -1 && index < size - 1) {
				desc[++index] = c;
			}
		}
		else if (index < size - 1) {
			desc[++index] = c;
			last_nws = index;
		}
	}

	/* Trim whitespace at the end of the stream */
	if (last_nws < size - 1) {
		desc[last_nws + 1] = '\0';
	}
}

/* Tries to read a username from stdin */
int read_username(char name[], int size) {
	/* Index of the last character read */
	int index = -1;
	int c;

	while ((c = getchar()) != '\n') {
		if ((c == ' ' || c == '\t') && index != -1) {
			break;
		}
		else if (c != ' ' && c != '\t' && index < size - 1) {
			name[++index] = c;
		}
	}

	if (index < size - 1) {
		name[index + 1] = '\0';
	}

	return index != -1;
}

/* Reads and executes a 't' command from stdin */
int read_t_command(struct kanban* board) {
	int duration, id;
	char desc[TASK_DESC_SZ];

	scanf("%d", &duration);
	read_desc(desc, TASK_DESC_SZ);
	id = add_task(board, duration, desc);
	if (id == -1) {
		return STATUS_ERR;
	}

	printf("task %d\n", id);
	return STATUS_OK;
}

/* Reads and executes a 'l' command from stdin */
int read_l_command(struct kanban* board) {
	int c, empty = 1, id = -1;

	while ((c = getchar()) != '\n') {
		if ((c == ' ' || c == '\t') && id != -1) {
			/* Search for task and print it */
			struct task* task = find_task(board, id);
			if (task == NULL) {
				printf("%d: no such task\n", id);
				return STATUS_ERR;
			}
			print_task(task);
			empty = 0;
			id = -1;
		}
		else if (c >= '0' && c <= '9') {
			if (id == -1) {
				id = 0;
			}

			id = 10 * id + c - '0';
		}
	}

	if (id != -1) {
		/* Search for task and print it */
		struct task* task = find_task(board, id);
		if (task == NULL) {
			printf("%d: no such task\n", id);
			return STATUS_ERR;
		}
		print_task(task);
		empty = 0;
	}

	/* If no IDs are provided, list all tasks */
	if (empty) {
		list_tasks(board);
	}

	return STATUS_OK;
}

/* Reads and executes a 'n' command from stdin */
int read_n_command(struct kanban* board) {
	int duration;
	scanf("%d", &duration);
	if (duration < 0) {
		puts("invalid time");
		return STATUS_ERR;
	}

	board->time += duration;
	printf("%d\n", board->time);
	return STATUS_OK;
}

/* Reads and executes a 'u' command from stdin */
int read_u_command(struct kanban* board) {
	char name[USER_NAME_SZ];

	if (read_username(name, USER_NAME_SZ)) {
		/* Add user to board */
		if (!add_user(board, name)) {
			return STATUS_ERR;
		}
	}
	else {
		/* Print list of users */
		list_users(board);
	}

	return STATUS_OK;
}

/* TODO */
int main() {
	int c, status;
	/* Kanban board */
	struct kanban board;

	init_kanban(&board);

	/* While q isn't entered and the program doesn't fail */
	while ((c = getchar()) != 'q' && status == STATUS_OK) {
		switch (c) {
		case 't':
			/* Adds a task to a board */
			status = read_t_command(&board);
			break;
		case 'l':
			/* Lists tasks */
			status = read_l_command(&board);
			break;
		case 'n':
			/* Advances time */
			status = read_n_command(&board);
			break;
		case 'u':
			/* Add user / list users */
			status = read_u_command(&board);
			break;

		default:
			/* Ignore unknown character/whitespace */
			break;
		}
	}

	return status;
}