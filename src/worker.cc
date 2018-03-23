#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/futex.h>
#include <sys/syscall.h>

#include <cstdlib>
#include <cstdio>

#include "control.h"

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stderr, "Usage: worker\n");
        return 0;
    }

    size_t mem_size = sizeof(struct ControlTable);
    struct ControlTable *control;

    int fd = shm_open("/kthread-bench-ctrl", O_RDWR, S_IRWXU);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open shared memory region. (%s)\n", strerror(errno));
    }

    if (ftruncate(fd, mem_size) == -1)
    {
        fprintf(stderr, "Could not truncate memory region. (%s)\n", strerror(errno));
    }

    control = (struct ControlTable *)mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (control == (struct ControlTable *)-1)
    {
        fprintf(stderr, "Could not mmap memory region. (%s)\n", strerror(errno));
    }

    int workerId = control->nextWorkerId.fetch_add(1);

    if (workerId >= MAX_WORKER_COUNT)
    {
        printf("Aborting... too many workers.\n");
        return 0;
    }

    printf("WorkerID %d\n", workerId);

    while (1)
    {
        printf("blocking\n");
        syscall(SYS_futex, &control->controlBlock[workerId].futex, FUTEX_WAIT, WORKER_SLEEP, NULL, NULL, 0);
        printf("now awake\n");
        control->controlBlock[workerId].futex = WORKER_SLEEP;
        control->controlBlock[workerId].val.store(workerId);
    }

    return 0;
}