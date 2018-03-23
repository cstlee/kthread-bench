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
#include "PerfUtils/Cycles.h"

using PerfUtils::Cycles;

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stderr, "Usage: worker\n");
        return 1;
    }

    size_t mem_size = sizeof(struct ControlTable);
    struct ControlTable *control;

    int fd = shm_open("/kthread-bench-ctrl", O_RDWR, S_IRWXU);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open shared memory region. [%d](%s)\n", errno, strerror(errno));
        return 1;
    }

    if (ftruncate(fd, mem_size) == -1)
    {
        fprintf(stderr, "Could not truncate memory region. [%d](%s)\n", errno, strerror(errno));
        return 1;
    }

    control = (struct ControlTable *)mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (control == (struct ControlTable *)-1)
    {
        fprintf(stderr, "Could not mmap memory region. [%d](%s)\n", errno, strerror(errno));
        return 1;
    }

    int workerId = control->nextWorkerId.fetch_add(1);

    if (workerId >= MAX_WORKER_COUNT)
    {
        fprintf(stderr, "Aborting... too many workers.\n");
        return 1;
    }


    while (1)
    {
        int err = 0;
        uint64_t endTSC = 0;
        err = syscall(SYS_futex, &control->controlBlock[workerId].futex, FUTEX_WAIT, WORKER_SLEEP, NULL, NULL, 0);
        endTSC = Cycles::rdtsc();
        if (err == -1)
        {
            fprintf(stderr, "Error while trying to wait on futex. [%d](%s)\n", errno, strerror(errno));
            return 1;
        }
        control->controlBlock[workerId].futex = WORKER_SLEEP;
        control->controlBlock[workerId].val.store(endTSC);
    }

    return 0;
}