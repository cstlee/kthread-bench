#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <linux/futex.h>
#include <sys/syscall.h>

#include <cstdlib>
#include <cstdio>

#include "control.h"

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stderr, "Usage: dispatch\n");
        return 0;
    }

    size_t mem_size = sizeof(struct ControlTable);
    struct ControlTable *control;

    int fd = shm_open("/kthread-bench-ctrl", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open shared memory region. (%d)\n", strerror(errno));
    }

    if (ftruncate(fd, mem_size) == -1)
    {
        fprintf(stderr, "Could not truncate memory region. (%d)\n", strerror(errno));
    }

    control = (struct ControlTable *)mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (control == (struct ControlTable *)-1)
    {
        fprintf(stderr, "Could not mmap memory region. (%s)\n", strerror(errno));
    }

    memset(control, 0, mem_size);
    printf("Ready\n");

    int i = 0;
    while (1)
    {
        usleep(100);
        if (control->nextWorkerId.load() < 1)
        {
            continue;
        }

        int workerId = rand() % control->nextWorkerId.load();

        control->controlBlock[workerId].futex = WORKER_WAKE;
        syscall(SYS_futex, &control->controlBlock[workerId].futex, FUTEX_WAKE, 1, NULL, NULL, 0);

        while (control->controlBlock[workerId].val.load() == -1)
        {
            usleep(10);
            printf("%d %d\n", workerId, control->nextWorkerId.load());
        }

        printf("val: %d\n", control->controlBlock[workerId].val.load());
        control->controlBlock[workerId].val.store(-1);
    }

    return 0;
}
