#include "builtins.h"
#include "csapp.h"
#include "tasks.h"

int command_tasks(int argc, char **argv);
int command_bg(int argc, char **argv);
int command_fg(int argc, char **argv);


/* If first arg is a builtin command, run it and return true */
int builtin_command(int argc, char **argv)
{
    if (!strcmp(argv[0], "quit")) /* quit command */
        exit(0);
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
        return 1;
    if (!strcmp(argv[0], "tasks"))
        return command_tasks(argc, argv);
    if (!strcmp(argv[0], "bg"))
        return command_bg(argc, argv);
    if (!strcmp(argv[0], "fg"))
        return command_fg(argc, argv);

    return 0;                     /* Not a builtin command */
}

int command_tasks(int argc, char **argv) {
    pid_t pid;
    int status;
    task *j;
    for (j = first_task; j; j = j->next)
        printf("[%d] %d %s   %s", 
            j->jid, 
            j->pid, 
            j->stopped ? "Stopped" : "Running", 
            j->cmdline);
    return 1;
}

int command_bg(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: bg <task>\n");
        printf("    task can be either a process id or task id prefixed with %%\n");
        printf("Examples:\n");
        printf("    bg 12345  -  Runs task with process id 12345 in the background\n");
        printf("    bg %%5     -  Runs task with task id 5 in the background\n");
        return 1;
    }

    task *j;
    if (argv[1][0] == '%') {
        unsigned jid = atoi(argv[1]+1);
        j = task_with_jid(jid);
    } else {
        pid_t pid = atoi(argv[1]);
        j = task_with_pid(pid);
    }

    if (!j) {
        fprintf(stderr, "No task with specified jid or pid\n");
        return 1;
    }

    Kill(-j->pid, SIGCONT);
    j->stopped = 0;

    return 1;
}

int command_fg(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: fg <task>\n");
        printf("    task can be either a process id or task id prefixed with %%\n");
        printf("Examples:\n");
        printf("    fg 69105  -  Runs task with process id 69105 in the foreground\n");
        printf("    fg %%3     -  Runs task with task id 3 in the foreground\n");
        return 1;
    }

    task *j;
    if (argv[1][0] == '%') {
        unsigned jid = atoi(argv[1]+1);
        j = task_with_jid(jid);
    } else {
        pid_t pid = atoi(argv[1]);
        j = task_with_pid(pid);
    }

    if (!j) {
        fprintf(stderr, "No task with specified jid or pid\n");
        return 1;
    }

    Kill(-j->pid, SIGCONT);
    j->stopped = 0;

    // mark task as needing to continue in foreground
    wait_for_task(j);

    return 1;
}

