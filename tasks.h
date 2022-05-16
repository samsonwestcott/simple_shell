#ifndef _tasks_h
#define _tasks_h

#include "csapp.h"

typedef struct task {
    unsigned jid;
    pid_t pid;
    int completed;
    int stopped;
    int notified;
    struct task *last;
    struct task *next;
    char cmdline[MAXLINE];
} task;

task *make_task(unsigned jid, pid_t pid, char *cmdline);
void remove_task(task *task);
task *task_with_jid(unsigned jid);
task *task_with_pid(pid_t pid);
int mark_process_status(pid_t pid, int status);
void update_tasks();
void wait_for_task(task *j);

extern task *first_task;

#endif 
