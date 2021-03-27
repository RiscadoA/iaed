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
	const char* user;
	/* Activity where the task is placed */
	const char* activity;
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

/* Gets a pointer to a user on a board */
const char* find_user(struct kanban* board, const char* name) {
	int i;

	for (i = 0; board->users[i][0] != '\0' && i < MAX_USER_COUNT; ++i) {
		if (strncmp(board->users[i], name, USER_NAME_SZ) == 0) {
			return board->users[i];
		}
	}

	return NULL;
}

/* Gets a pointer to an activity on a board */
const char* find_activity(struct kanban* board, const char* activity) {
	int i;

	for (i = 0; board->activities[i][0] != '\0' && i < MAX_ACTIVITY_COUNT; ++i) {
		if (strncmp(board->activities[i], activity, ACTIVITY_DESC_SZ) == 0) {
			return board->activities[i];
		}
	}

	return NULL;
}

/* Adds a task to a kanban board */
int add_task(struct kanban* board, int duration, const char* desc) {
	int i;
	struct task* task;

	/* Check if there are too many tasks */
	if (board->task_count == MAX_TASK_COUNT) {
		puts("too many tasks");
		return 0;
	}
	
	/* Check if there is already a task with this description */
	for (i = 0; i < board->task_count; ++i) {
		if (strncmp(board->tasks[i].desc, desc, TASK_DESC_SZ) == 0) {
			puts("duplicate description");
			return 0;
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
	task->user = NULL;
	task->activity = find_activity(board, "TO DO");
	task->start = 0;
	task->duration = duration;

	return task->id;
}

/* Adds a user to a kanban board */
int add_user(struct kanban* board, const char* name) {
	int i;

	/* Search for an empty slot and check if the name is duplicated */
	for (i = 0; board->users[i][0] != '\0' && i < MAX_USER_COUNT; ++i) {
		if (strncmp(board->users[i], name, USER_NAME_SZ) == 0) {
			puts("user already exists");
			return 0;
		}
	}

	if (i >= MAX_USER_COUNT) {
		puts("too many users");
		return 0;
	}

	strncpy(board->users[i], name, USER_NAME_SZ);
	return 1;
}

/* Adds an activity to a kanban board */
int add_activity(struct kanban* board, const char* desc) {
	int i;

	/* Search for an empty slot and check if the name is duplicated */
	for (i = 0; board->activities[i][0] != '\0' && i < MAX_ACTIVITY_COUNT; ++i) {
		if (strncmp(board->activities[i], desc, ACTIVITY_DESC_SZ) == 0) {
			puts("duplicate activity");
			return 0;
		}
	}

	if (i >= MAX_ACTIVITY_COUNT) {
		puts("too many activities");
		return 0;
	}

	for (i = 0; i < ACTIVITY_DESC_SZ && desc[i] != '\0'; ++i) {
		if ('a' <= desc[i] && desc[i] <= 'z') {
			puts("invalid description");
			return 0;
		}
	}

	strncpy(board->activities[i], desc, ACTIVITY_DESC_SZ);
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

/* Moves a task on a kanban board */
int move_task(struct kanban* board, int id, const char* usr, const char* act) {
	int diff;
	struct task* task = find_task(board, id);

	if (task == NULL) {
		puts("no such task");
		return 0;
	}

	if (strncmp("TO DO", act, ACTIVITY_DESC_SZ) == 0) {
		puts("task already started");
		return 0;	
	}

	/* Check if user exists and get pointer to data */
	if ((usr = find_user(board, usr)) == NULL) {
		puts("no such user");
		return 0;	
	}

	/* Check if activity exists and get pointer to data */
	if ((act = find_activity(board, act)) == NULL) {
		puts("no such activity");
		return 0;	
	}

	if (strncmp(task->activity, "TO DO", ACTIVITY_DESC_SZ) == 0) {
		task->start = board->time;
	}

	task->user = usr;
	task->activity = act;

	if (strncmp(task->activity, "DONE", ACTIVITY_DESC_SZ) == 0) {
		diff = board->time - task->start;
		printf("duration=%d slack=%d\n", diff, diff - task->duration);
	}

	return 1;
}

/* Prints a task with l command format */
void print_task_l(struct task* task) {
	/* Format: <id> <activity> #<duration> <description> */
	printf(
		"%d %.*s #%d %.*s\n",
		task->id,
		ACTIVITY_DESC_SZ, task->activity,
		task->duration,
		TASK_DESC_SZ, task->desc
	);
}

/* Prints a task with d command format */
void print_task_d(struct task* task) {
	/* Format: <id> <activity> #<duration> <description> */
	printf(
		"%d %d %.*s\n",
		task->id,
		task->start,
		TASK_DESC_SZ, task->desc
	);
}

/* Print all of the tasks in lexicographical order */
void list_tasks(struct kanban* board) {
	int i;
	for (i = 0; i < board->task_count; ++i) {
		print_task_l(&board->tasks[i]);
	}
}

/* Print all of the tasks in lexicographical order */
int list_activity_tasks(struct kanban* board, const char* activity) {
	int i, changed, temp;
	struct task* lhs;
	struct task* rhs;
	/*
		Indexes of the tasks in this activity, must be static to prevent stack
		overflow. It could also be declared globally but I decided that I
		didn't want to pollute the global scope. Since this isn't meant to be
		thread safe, using static variables here isn't a problem.
	 */
	static int act_tasks[MAX_ACTIVITY_COUNT], count = 0;

	if (find_activity(board, activity) == NULL) {
		puts("no such activity");
		return 0;
	}

	for (i = 0; i < board->task_count; ++i) {
		if (strncmp(board->tasks[i].activity, activity, ACTIVITY_DESC_SZ) == 0) {
			act_tasks[count++] = i;
		}
	}

	/* Bubble sort tasks */
	do {
		changed = 0;
		for (i = 1; i < count; ++i) {
			lhs = &board->tasks[act_tasks[i - 1]];
			rhs = &board->tasks[act_tasks[i]];
			if (lhs->start > rhs->start ||
				(lhs->start == rhs->start && act_tasks[i - 1] > act_tasks[i])) {
				/* Swap tasks */
				temp = act_tasks[i];
				act_tasks[i] = act_tasks[i - 1];
				act_tasks[i - 1] = temp;
				changed = 1;
			}
		}
	} while (changed);
	
	for (i = 0; i < count; ++i) {
		print_task_d(&board->tasks[act_tasks[i]]);
	}

	return 1;
}

/* Print all of the users in creation order */
void list_users(struct kanban* board) {
	int i;
	for (i = 0; board->users[i][0] != '\0' && i < MAX_USER_COUNT; ++i) {
		printf("%.*s\n", USER_NAME_SZ, board->users[i]);
	}
}

/* Print all of the activities in creation order */
void list_activities(struct kanban* board) {
	int i;
	for (i = 0; board->activities[i][0] != '\0' && i < MAX_ACTIVITY_COUNT; ++i) {
		printf("%.*s\n", ACTIVITY_DESC_SZ, board->activities[i]);
	}
}

/* Reads either a task or activity description from stdin */
int read_desc(char* desc, int size) {
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

	if (index == -1) {
		return 0;
	}

	/* Trim whitespace at the end of the stream */
	if (last_nws < size - 1) {
		desc[last_nws + 1] = '\0';
	}

	return 1;
}

/* Tries to read a username from stdin */
int read_username(char* name, int size) {
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
	if (!id) {
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
			print_task_l(task);
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
		print_task_l(task);
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

/* Reads and executes a 'm' command from stdin */
int read_m_command(struct kanban* board) {
	int id;
	char user[USER_NAME_SZ], activity[ACTIVITY_DESC_SZ];

	scanf("%d", &id);
	read_username(user, USER_NAME_SZ);
	read_desc(activity, ACTIVITY_DESC_SZ);

	if (!move_task(board, id, user, activity)) {
		return STATUS_ERR;
	}

	return STATUS_OK;
}

/* Reads and executes a 'd' command from stdin */
int read_d_command(struct kanban* board) {
	char activity[ACTIVITY_DESC_SZ];
	read_desc(activity, ACTIVITY_DESC_SZ);

	if (!list_activity_tasks(board, activity)) {
		return STATUS_ERR;
	}

	return STATUS_OK;
}

/* Reads and executes a 'a' command from stdin */
int read_a_command(struct kanban* board) {
	char desc[ACTIVITY_DESC_SZ];

	if (read_desc(desc, ACTIVITY_DESC_SZ)) {
		/* Add activity to board */
		if (!add_activity(board, desc)) {
			return STATUS_ERR;
		}
	}
	else {
		/* Print list of activities */
		list_activities(board);
	}

	return STATUS_OK;
}

/* TODO */
int main() {
	int c, status = STATUS_OK;
	/*
		Kanban board, must be static to prevent stack overflow. It could also
		be declared globally but I decided that I didn't want to pollute the
		global scope.
	*/
	static struct kanban board;

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
		case 'm':
			/* Move task */
			status = read_m_command(&board);
			break;
		case 'd':
			/* List tasks in activity */
			status = read_d_command(&board);
			break;
		case 'a':
			/* Add activity / list actvities */
			status = read_a_command(&board);
			break;

		default:
			/* Ignore unknown character / whitespace */
			break;
		}
	}

	return status;
}