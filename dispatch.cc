#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
    struct ControlTable* controlTable;

    int fd = shm_open("/kthread-bench-ctrl", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open shared memory region. (%d)\n", strerror(errno));
    }

    if (ftruncate(fd, mem_size) == -1)
    {
        fprintf(stderr, "Could not truncate memory region. (%d)\n", strerror(errno));
    }

    controlTable = (struct ControlTable*)mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (controlTable == (struct ControlTable*)-1)
    {
        fprintf(stderr, "Could not mmap memory region. (%s)\n", strerror(errno));
    }

    int i = 0;
    while(1) {
        controlTable->controlBlock[0].val.store(i++);
    }

    return 0;
}
