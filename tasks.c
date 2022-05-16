#include "task.h"

task *first_task = NULL;

task *make_task(unsigned jid, pid_t pid, char *cmdline) {
    task *task = malloc(sizeof(task));

    if (!task) {
        fprintf(stderr, "unable to allocate memory for new task");
        return NULL;
    }

    task->jid = jid;
    task->pid = pid;
    task->completed = 0;
    task->stopped = 0;
    task->notified = 0;
    task->last = NULL;
    task->next = NULL;
    strcpy(task->cmdline, cmdline);

    if (!first_task) {
        first_task = task;
        return task;
    }

    /* add to list of bg tasks */
    task *j = first_task;
    while (j->next)
        j = j->next;
    j->next = task;
    task->last = j;

    return task;
}

void remove_task(task *task) {
    task *j;
    for (j = first_task; j; j = j->next) {
        if (j->pid == task->pid) {
            if (!j->last && !j->next)
                first_task = NULL;
            else {
                if (!j->last)
                    first_task = j->next;
                else 
                    j->last->next = j->next;

                if (j->next)
                    j->next->last = j->last;
            }
            free(task);
            return;
        }
    }
    fprintf(stderr, "task %d not in bg task list\n", task->pid);
}

task *task_with_jid(unsigned jid) {
    task *j;
    for (j = first_task; j; j = j->next)
        if (j->jid == jid)
            return j;
    return NULL;
}

task *task_with_pid(pid_t pid) {
    task *j;
    for (j = first_task; j; j = j->next)
        if (j->pid == pid)
            return j;
    return NULL;
}

int mark_process_status(pid_t pid, int status) {
    task *j;

    if (pid > 0) {
        /* Update the record for the process. */
        for (j = first_task; j; j = j->next) {
            if (j->pid == pid) {
                if (WIFSTOPPED(status)) {
                    printf("task %d stopped\n", pid);
                    j->stopped = 1;
                } else {
                    j->completed = 1;
                    if (WIFSIGNALED(status))
                        fprintf(stderr, "task %d terminated by signal: %d\n", j->pid, WTERMSIG(status));
                    else
                        printf("task %d exited normally\n", pid);
                }
                return 0;
            }
        }
        fprintf(stderr, "No child process %d\n", pid);
        return -1;
    }
    else if (pid == 0 || errno == ECHILD)
        /* No processes ready to report. */
        return -1;
    else {
        /* Other weird errors.  */
        perror("waitpid");
        return -1;
    }
}

void update_tasks() {
    pid_t pid;
    int status;

    do 
        pid = waitpid(-1, &status, WUNTRACED|WNOHANG);
    while (!mark_process_status(pid, status));

    task *j;
    for (j = first_task; j; j = j->next) {
        if (j->completed)
            remove_task(j);
        else if (j->stopped && !j->notified)
            j->notified = 1;
    }
}


