/*
 * File: 		proj1.c
 * Author: 		Ricardo Antunes
 * Description: IAED project 1.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* The maximum number of characters in a task description. */
#define TASK_DESC_SZ 50
/* The maximum number of tasks supported. */
#define MAX_TASK_COUNT 10000

/* The maximum number of characters used in an username. */
#define USER_NAME_SZ 20
/* The maximum number of users supported. */
#define MAX_USER_COUNT 50

/* The maximum number of characters used to describe an activity. */
#define ACTIVITY_DESC_SZ 20
/* The maximum number of activities supported. */
#define MAX_ACTIVITY_COUNT 10

/* Status codes used by the main and read command functions. */
#define STATUS_OK 0
#define STATUS_ERR -1

/* String literals used. */
#define TO_DO_STR "TO DO"
#define IN_PROGRESS_STR "IN PROGRESS"
#define DONE_STR "DONE"

/* Formatting string literals. */
#define TASK_ID_FORMAT "task %d\n"
#define TASK_DONE_FORMAT "duration=%d slack=%d\n"
#define TASK_1_FORMAT "%d %.*s #%d %.*s\n"
#define TASK_2_FORMAT "%d %d %.*s\n"
#define USER_FORMAT "%.*s\n"
#define ACTIVITY_FORMAT "%.*s\n"
#define TIME_FORMAT "%d\n"

/* Error string literals. */
#define TOO_MANY_TASKS_STR "too many tasks"
#define DUPLICATE_DESC_STR "duplicate description"
#define USER_ALREADY_EXISTS_STR "user already exists"
#define TOO_MANY_USERS_STR "too many users"
#define DUPLICATE_ACTIVITY_STR "duplicate activity"
#define TOO_MANY_ACTIVITIES_STR "too many activities"
#define INVALID_DESC_STR "invalid description"
#define NO_SUCH_TASK_STR "no such task"
#define NO_SUCH_USER_STR "no such user"
#define NO_SUCH_ACTIVITY_STR "no such activity"
#define TASK_ALREADY_STARTED_STR "task already started"
#define INVALID_TIME_STR "invalid time"
#define INVALID_DURATION_STR "invalid duration"

/* Error formatting string literals. */
#define NO_SUCH_TASK_FORMAT "%d: no such task\n"

/* Contains information about a task. */
struct task {
	/* Task identifier. */
	int id;
	/* Task description. */
	char desc[TASK_DESC_SZ];
	/* User who is responsible for the task. */
	const char* user;
	/* Activity where the task is placed. */
	const char* activity;
	/* Predicted task completion duration. */
	int duration;
	/* The time the task started being executed. */
	int start;
};

/* Contains information about a kanban board. */
struct kanban {
	/* Tasks in the kanban board. */
	struct task tasks[MAX_TASK_COUNT];
	/* Number of tasks in the kanban board. */
	int task_count;
	/* The current time. */
	int time;
	/* Users in the kanban board. */
	char users[MAX_USER_COUNT][USER_NAME_SZ];
	/* Activities in the kanban board. */
	char activities[MAX_ACTIVITY_COUNT][ACTIVITY_DESC_SZ];
	/* Array used to sort printed tasks in an activity. */
	int activity_order[MAX_TASK_COUNT];
};

/* Initializes a kanban board. */
void init_kanban(struct kanban* board) {
	int i;
	
	board->task_count = 0;
	board->time = 0;

	for (i = 0; i < MAX_USER_COUNT; ++i) {
		board->users[i][0] = '\0';
	}

	/* Set default activities. */
	strncpy(board->activities[0], TO_DO_STR, ACTIVITY_DESC_SZ);
	strncpy(board->activities[1], IN_PROGRESS_STR, ACTIVITY_DESC_SZ);
	strncpy(board->activities[2], DONE_STR, ACTIVITY_DESC_SZ);
	for (i = 3; i < MAX_ACTIVITY_COUNT; ++i) {
		board->activities[i][0] = '\0';
	}
}

/* Initializes a task. */
void init_task(struct task* task, int id, int duration, const char* desc) {
	task->id = id;
	strncpy(task->desc, desc, TASK_DESC_SZ);
	task->user = NULL;
	task->activity = TO_DO_STR;
	task->start = 0;
	task->duration = duration;
}

/*
 * Gets a pointer to a user string on a board.
 * Returns NULL if the user isn't found.
 */
const char* find_user(struct kanban* board, const char* name) {
	int i;

	for (i = 0; board->users[i][0] != '\0' && i < MAX_USER_COUNT; ++i) {
		if (strncmp(board->users[i], name, USER_NAME_SZ) == 0) {
			return board->users[i];
		}
	}

	return NULL;
}

/*
 * Gets a pointer to an activity on a board.
 * Returns NULL if the activity isn't found.
 */
const char* find_activity(struct kanban* board, const char* activity) {
	int i;

	for (i = 0; board->activities[i][0] != '\0' && i < MAX_ACTIVITY_COUNT; ++i) {
		if (strncmp(board->activities[i], activity, ACTIVITY_DESC_SZ) == 0) {
			return board->activities[i];
		}
	}

	return NULL;
}

/*
 * Gets a pointer to a task from its id.
 * Returns NULL if the task isn't found.
 */
struct task* find_task(struct kanban* board, int id) {
	int i;
	
	for (i = 0; i < board->task_count; ++i) {
		if (board->tasks[i].id == id) {
			return &board->tasks[i];
		}
	}

	return NULL;
}

/* Prints a task with the format TASK_1_FORMAT. */
void print_task_1(struct task* task) {
	printf(
		TASK_1_FORMAT,
		task->id,
		ACTIVITY_DESC_SZ, task->activity,
		task->duration,
		TASK_DESC_SZ, task->desc
	);
}

/* Prints a task with the format TASK_2_FORMAT. */
void print_task_2(struct task* task) {
	printf(
		TASK_2_FORMAT,
		task->id,
		task->start,
		TASK_DESC_SZ, task->desc
	);
}

/*
 * Print all of the tasks in the board in lexicographical order.
 * The tasks are printed with the format TASK_1_FORMAT.
 */
void list_all_tasks(struct kanban* board) {
	int i;

	/*
	 * Since the tasks are always sorted by their description, no sorting
	 * is necessary here. 
	 */
	for (i = 0; i < board->task_count; ++i) {
		print_task_1(&board->tasks[i]);
	}
}

/*
 * Sorts the indexes of the tasks stored on board->activity_order by starting
 * time. If two tasks have the same starting time, the task which is
 * lexicographically smaller appears first.
 */
void sort_activity_tasks(struct kanban* board, int count) {
	int i, changed, temp;
	int* order = board->activity_order;
	struct task* lhs;
	struct task* rhs;

	/* Bubble sort the indexes of the tasks. */
	do {
		changed = 0;
		for (i = 1; i < count; ++i) {
			lhs = &board->tasks[order[i - 1]];
			rhs = &board->tasks[order[i]];
			/* If the task indexes are unordered. */
			if (lhs->start > rhs->start ||
				(lhs->start == rhs->start && order[i - 1] > order[i])) {
				/* Swap task indexes. */
				temp = order[i];
				order[i] = order[i - 1];
				order[i - 1] = temp;
				changed = 1;
			}
		}
	} while (changed);
}

/*
 * Print all of the tasks in an activity sorted by starting time. If two tasks
 * have the same starting time, the task which is lexicographically smaller
 * appears first.
 * The tasks are printed with the format TASK_2_FORMAT.
 *
 * If the activity isn't on the board, NO_SUCH_ACTIVITY_STR is sent to stdout
 * and the operation is canceled.
 */
void list_activity_tasks(struct kanban* board, const char* activity) {
	int i, count = 0;
	int* order = board->activity_order;

	/* Check if the activity exists. */
	if (find_activity(board, activity) == NULL) {
		puts(NO_SUCH_ACTIVITY_STR);
		return;
	}

	/* Search for tasks which are in the activity. */
	for (i = 0; i < board->task_count; ++i) {
		if (strncmp(board->tasks[i].activity, activity, ACTIVITY_DESC_SZ) == 0) {
			order[count++] = i;
		}
	}

	/* Sort the indexes of the tasks in the activity. */
	sort_activity_tasks(board, count);
	
	/* Print tasks in the activity. */
	for (i = 0; i < count; ++i) {
		print_task_2(&board->tasks[order[i]]);
	}
}

/*
 * Print all of the users in creation order.
 * The users are printed with the format USER_FORMAT.
 */
void list_users(struct kanban* board) {
	int i;

	for (i = 0; board->users[i][0] != '\0' && i < MAX_USER_COUNT; ++i) {
		printf(USER_FORMAT, USER_NAME_SZ, board->users[i]);
	}
}

/*
 * Print all of the activities in creation order.
 * The activities are printed with the format ACTIVITY_FORMAT.
 */
void list_activities(struct kanban* board) {
	int i;
	
	for (i = 0; board->activities[i][0] != '\0' && i < MAX_ACTIVITY_COUNT; ++i) {
		printf(ACTIVITY_FORMAT, ACTIVITY_DESC_SZ, board->activities[i]);
	}
}

/*
 * Adds a task to a kanban board.
 *
 * If the board is already full of tasks, TOO_MANY_TASKS_STR is sent to stdout
 * and the operation is canceled.
 * Otherwise, if there is already a task with the same description,
 * DUPLICATE_DESC_STR is sent to stdout and the operation is canceled.
 * Otherwise, if the duration isn't a positive integer, INVALID_DURATION_STR is
 * sent to stdout and the operation is canceled.
 * Otherwise, TASK_ID_FORMAT is sent to stdout formatted with the new task ID.
 */
void add_task(struct kanban* board, int duration, const char* desc) {
	int i, cmp;

	/* Check if there are too many tasks. */
	if (board->task_count == MAX_TASK_COUNT) {
		puts(TOO_MANY_TASKS_STR);
		return;
	}

	/*
	 * The task array is always sorted, so we need to find where to insert a
	 * new element. This could be done using binary search, but since we're
	 * going to have to move the array forward it will be O(N) anyway (assuming
	 * memmove is O(N)). So, a simple linear search will suffice.
	 */

	for (i = 0, cmp = -1; i < board->task_count && cmp < 0; ++i) {
		cmp = strncmp(board->tasks[i].desc, desc, TASK_DESC_SZ);
		/* Check if there is already a task with this description. */
		if (cmp == 0) {
			puts(DUPLICATE_DESC_STR);
			return;
		}
	}

	/* Check it the duration is valid. */
	if (duration <= 0) {
		puts(INVALID_DURATION_STR);
		return;
	}

	/* Move the array forward to make space for the new task. */
	if (board->task_count > 0 && cmp > 0) {
		--i;
		memmove(&board->tasks[i + 1], &board->tasks[i],
				(board->task_count - i) * sizeof(struct task));
	}

	/* Add task to board. */
	init_task(&board->tasks[i], ++board->task_count, duration, desc);
	printf(TASK_ID_FORMAT, board->task_count);
}

/*
 * Adds a user to a kanban board.
 *
 * If the user is already on the board, USER_ALREADY_EXISTS_STR is sent to
 * stdout and the operation is canceled.
 * Otherwise, if the board is already full of users, TOO_MANY_USERS_STR is
 * sent to stdout and the operation is canceled.
 */
void add_user(struct kanban* board, const char* name) {
	int i;

	/* Search for an empty slot and check if the name is duplicated. */
	for (i = 0; board->users[i][0] != '\0' && i < MAX_USER_COUNT; ++i) {
		if (strncmp(board->users[i], name, USER_NAME_SZ) == 0) {
			puts(USER_ALREADY_EXISTS_STR);
			return;
		}
	}

	/* Check if an empty slot was found. */
	if (i >= MAX_USER_COUNT) {
		puts(TOO_MANY_USERS_STR);
		return;
	}

	strncpy(board->users[i], name, USER_NAME_SZ);
}

/*
 * Adds an activity to a kanban board.
 *
 * If the activity is already on the board, DUPLICATE_ACTIVITY_STR is sent to
 * stdout and the operation is canceled.
 * Otherwise, if the activity description contains lowercase characters,
 * INVALID_DESC_STR is sent to stdout and the operation is canceled.
 * Otherwise, if the board is already full of activities,
 * TOO_MANY_ACTIVITIES_STR is sent to stdout and the operation is canceled.
 */
void add_activity(struct kanban* board, const char* desc) {
	int i, j;

	/* Search for an empty slot and check if the name is duplicated. */
	for (i = 0; board->activities[i][0] != '\0' && i < MAX_ACTIVITY_COUNT; ++i) {
		if (strncmp(board->activities[i], desc, ACTIVITY_DESC_SZ) == 0) {
			puts(DUPLICATE_ACTIVITY_STR);
			return;
		}
	}

	/* Check if the activity description is valid. */
	for (j = 0; j < ACTIVITY_DESC_SZ && desc[j] != '\0'; ++j) {
		if (islower(desc[j])) {
			puts(INVALID_DESC_STR);
			return;
		}
	}

	/* Check if an empty slot was found. */
	if (i >= MAX_ACTIVITY_COUNT) {
		puts(TOO_MANY_ACTIVITIES_STR);
		return;
	}

	strncpy(board->activities[i], desc, ACTIVITY_DESC_SZ);
}

/*
 * Moves a task on a kanban board to another activity.
 * If the task is moved to the activity DONE_STR, TASK_DONE_FORMAT is sent to
 * stdout formatted with the predicted completion time and difference between
 * the real and predicted times.
 * 
 * If the task isn't on the board, NO_SUCH_TASK_STR is sent to
 * stdout and the operation is canceled.
 * Otherwise, if the target activity is TO_DO_STR, TASK_ALREADY_STARTED_STR
 * is sent to stdout and the operation is canceled.
 * Otherwise, if the user isn't on the board, NO_SUCH_USER_STR is sent to
 * stdout and the operation is canceled.
 * Otherwise, if the activity isn't on the board, NO_SUCH_ACTIVITY_STR is sent
 * to stdout and the operation is canceled.
 */
void move_task(struct kanban* board, int id, const char* usr, const char* act) {
	int diff;
	struct task* task;

	/* Check if the task exists and get pointer to data. */
	if ((task = find_task(board, id)) == NULL) {
		puts(NO_SUCH_TASK_STR);
		return;
	}

	/* Check if the task had already been started. */
	if (strncmp(act, TO_DO_STR, ACTIVITY_DESC_SZ) == 0) {
		puts(TASK_ALREADY_STARTED_STR);
		return;	
	}

	/* Check if the user exists and get pointer to data. */
	if ((usr = find_user(board, usr)) == NULL) {
		puts(NO_SUCH_USER_STR);
		return;	
	}

	/* Check if the activity exists and get pointer to data. */
	if ((act = find_activity(board, act)) == NULL) {
		puts(NO_SUCH_ACTIVITY_STR);
		return;
	}

	/* Check if the task is being started now. */
	if (strncmp(task->activity, TO_DO_STR, ACTIVITY_DESC_SZ) == 0) {
		task->start = board->time;
	}

	task->user = usr;
	task->activity = act;

	/* Check if the task is done. */
	if (strncmp(task->activity, DONE_STR, ACTIVITY_DESC_SZ) == 0) {
		diff = board->time - task->start;
		printf(TASK_DONE_FORMAT, diff, diff - task->duration);
	}
}

/*
 * Advances time on a kanban board and TIME_FORMAT is sent to stdout formatted
 * with the new current time.
 * 
 * If the duration is negative, INVALID_TIME_STR is sent to
 * stdout and the operation is canceled.
 */
void advance_time(struct kanban* board, int duration) {
	if (duration < 0) {
		puts(INVALID_TIME_STR);
	}
	
	board->time += duration;
	printf(TIME_FORMAT, board->time);
}

/*
 * Reads an ID from stdin.
 * If no ID is found, 0 is returned. Otherwise the ID is returned. 
 */
int read_id() {
	int c, id = -1;

	/* While a newline or a whitespace after the ID isn't found. */
	while ((c = getchar()) != '\n') {
		if (isspace(c) && id != -1) {
			return id;
		}
		else if (c >= '0' && c <= '9') {
			if (id == -1) {
				id = 0;
			}

			id = 10 * id + c - '0';
		}
	}

	/*
	 * Push the newline back into the stream so that the calling function 
	 * can detect the end of the line.
	 */
	if (c == '\n') {
		ungetc(c, stdin);
	}
	
	return id == -1 ? 0 : id;
}

/*
 * Reads either a task or activity description from stdin.
 * If no description is found, 0 is returned. Otherwise 1 is returned. 
 */
int read_desc(char* desc, int size) {
	/* Index of the last character read. */
	int index = -1;
	/* Index of the last non whitespace character. */
	int last_nws = 0;
	int c;

	/* While a newline isn't found. */
	while ((c = getchar()) != '\n') {
		if (isspace(c)) {
			/* Trim whitespace at the beginning of the stream. */
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

	/* Insert terminating null character if needed. */
	if (last_nws < size - 1) {
		desc[last_nws + 1] = '\0';
	}

	return 1;
}

/*
 * Tries to read a username from stdin.
 * If no username is found, 0 is returned. Otherwise 1 is returned. 
 */
int read_username(char* name, int size) {
	/* Index of the last character read. */
	int index = -1;
	int c;

	/* While a newline isn't found and no whitespace is found after the name. */
	while ((c = getchar()) != '\n') {
		if (isspace(c) && index != -1) {
			break;
		}
		else if (!isspace(c) && index < size - 1) {
			name[++index] = c;
		}
	}

	/* Insert terminating null character if needed. */
	if (index < size - 1) {
		name[index + 1] = '\0';
	}

	return index != -1;
}

/*
 * Reads and executes a 't' command from stdin, which adds a task to a kanban
 * board.
 * Input format: <duration> <description> 
 */
void read_t_command(struct kanban* board) {
	int duration;
	char desc[TASK_DESC_SZ];

	scanf("%d", &duration);
	read_desc(desc, TASK_DESC_SZ);
	add_task(board, duration, desc);
}

/*
 * Reads and executes a 'l' command from stdin, which prints either the tasks
 * passed to it or if none is passed prints all tasks in lexicographical order.
 * If a task is not found from its ID NO_SUCH_TASK_FORMAT is sent to stdout
 * formatted with the ID.
 * Input format: [<id> <id> ... <id>]
 */
void read_l_command(struct kanban* board) {
	int empty = 1, id;
	struct task* task;

	/* Try to read IDs. */
	while ((id = read_id())) {
		empty = 0;
		task = find_task(board, id);
		if (task == NULL) {
			printf(NO_SUCH_TASK_FORMAT, id);
		}
		else {
			print_task_1(task);				
		}
	}

	/* If no IDs are provided, list all tasks. */
	if (empty) {
		list_all_tasks(board);
	}
}

/*
 * Reads and executes a 'n' command from stdin, which advances the time in the
 * board.
 * Input format: <duration>
 */
void read_n_command(struct kanban* board) {
	int duration;

	scanf("%d", &duration);
	advance_time(board, duration);
}

/*
 * Reads and executes a 'u' command from stdin, which either adds an user to
 * the board or prints all of the users in creation order.
 * Input format: [<username>]
 */
void read_u_command(struct kanban* board) {
	char name[USER_NAME_SZ];

	if (read_username(name, USER_NAME_SZ)) {
		/* Add user to board. */
		add_user(board, name);
	}
	else {
		/* Print list of users. */
		list_users(board);
	}
}

/*
 * Reads and executes a 'm' command from stdin, which moves a task to another
 * activity.
 * Input format: <id> <username> <activity>
 */
void read_m_command(struct kanban* board) {
	int id;
	char user[USER_NAME_SZ], activity[ACTIVITY_DESC_SZ];

	scanf("%d", &id);
	read_username(user, USER_NAME_SZ);
	read_desc(activity, ACTIVITY_DESC_SZ);
	move_task(board, id, user, activity);
}

/*
 * Reads and executes a 'd' command from stdin, which prints all tasks in an
 * activity.
 * Input format: <activity>
 */
void read_d_command(struct kanban* board) {
	char activity[ACTIVITY_DESC_SZ];
	read_desc(activity, ACTIVITY_DESC_SZ);
	list_activity_tasks(board, activity);
}

/*
 * Reads and executes a 'a' command from stdin, which either adds an activity
 * to the board or prints all activities on the board.
 * Input format: [<activity>]
 */
void read_a_command(struct kanban* board) {
	char desc[ACTIVITY_DESC_SZ];

	if (read_desc(desc, ACTIVITY_DESC_SZ)) {
		/* Add activity to board. */
		add_activity(board, desc);
	}
	else {
		/* Print list of activities. */
		list_activities(board);
	}
}

/*
 * Tries to read a command from stdin. If the character passed is not a command
 * character, the function doesn't do anything. Otherwise, the respective
 * command is read and executed.
 */
void read_command(struct kanban* board, int c) {
	switch (c) {
	case 't':
		/* Adds a task to a board. */
		read_t_command(board);
		break;
	case 'l':
		/* Lists tasks. */
		read_l_command(board);
		break;
	case 'n':
		/* Advances time. */
		read_n_command(board);
		break;
	case 'u':
		/* Add user / list users. */
		read_u_command(board);
		break;
	case 'm':
		/* Move task. */
		read_m_command(board);
		break;
	case 'd':
		/* List tasks in activity. */
		read_d_command(board);
		break;
	case 'a':
		/* Add activity / list actvities. */
		read_a_command(board);
		break;
	}
}

/* Reads commands from stdin line by line and executes them. */
int main() {
	/*
		Kanban board, must be static to prevent stack overflow. It could also
		be declared globally but I decided that I didn't want to pollute the
		global scope.
	*/
	static struct kanban board;
	int c;

	init_kanban(&board);

	/* While q isn't entered. */
	while ((c = getchar()) != 'q') {
		read_command(&board, c);
	}

	return 0;
}