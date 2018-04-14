#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <linux/futex.h>
#include <sys/syscall.h>

#include <cstdio>

#include "control.h"
#include "PerfUtils/Cycles.h"
#include "docopt.h"

using PerfUtils::Cycles;

static const char USAGE[] = 
R"(Futex Benchmark Dispatch

    Usage:
        dispatch <num_workers> <num_samples> [--sleep-us TIME]

    Options:
        -h --help           Show this screen.
        --version           Show version.
        --sleep-us TIME     Time between futex wakeups in usec [default: 100].
)";

int main(int argc, char *argv[])
{
    std::map<std::string, docopt::value> args
        = docopt::docopt(USAGE,
                         { argv + 1, argv + argc },
                         true,
                         "Futex Benchmark Dispatch 0.1");

    int num_workers = args.at("<num_workers>").asLong();
    int num_samples = args.at("<num_samples>").asLong();
    int sleep_us = args.at("--sleep-us").asLong();

    if (num_workers < 1)
    {
        fprintf(stderr, "Cannot support less than 1 worker. "
                        "%d workers requested.\n",
                num_workers);
        return 1;
    }

    if (num_workers > MAX_WORKER_COUNT)
    {
        fprintf(stderr, "Cannot support more than %d workers. "
                        "%d workers requested.\n",
                MAX_WORKER_COUNT, num_workers);
        return 1;
    }

    uint64_t samples[num_samples];
    size_t mem_size = sizeof(struct ControlTable);
    struct ControlTable *control;

    int fd = shm_open("/kthread-bench-ctrl", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open shared memory region. (%s)\n", strerror(errno));
        return 1;
    }

    if (ftruncate(fd, mem_size) == -1)
    {
        fprintf(stderr, "Could not truncate memory region. (%s)\n", strerror(errno));
        return 1;
    }

    control = (struct ControlTable *)mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (control == (struct ControlTable *)-1)
    {
        fprintf(stderr, "Could not mmap memory region. (%s)\n", strerror(errno));
        return 1;
    }

    memset(control, 0, mem_size);

    // Wait for requested number of workers to spin up.
    while (control->nextWorkerId.load() < num_workers)
        ;

    for (int i = 0; i < num_samples; ++i)
    {
        // Race Condition; Slow down the dispatch to try to make sure the worker
        // has time to go back to sleep before we try to signal.
        Cycles::sleep(sleep_us);

        int workerId = rand() % control->nextWorkerId.load();
        int err = 0;
        uint64_t startTSC = 0;
        uint64_t endTSC = 0;

        control->controlBlock[workerId].futex = WORKER_WAKE;
        startTSC = Cycles::rdtsc();
        err = syscall(SYS_futex, &control->controlBlock[workerId].futex, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (err == -1)
        {
            fprintf(stderr, "Error while trying to wake futex. (%s)\n", strerror(errno));
            return 1;
        }

        // Wait for worker to wake up and report latency.
        while (control->controlBlock[workerId].val.load() == 0)
            ;
        endTSC = control->controlBlock[workerId].val.load();
        control->controlBlock[workerId].val.store(0);

        samples[i] = Cycles::toNanoseconds(endTSC - startTSC);
    }

    // Print samples
    for (int i = 0; i < num_samples; ++i)
    {
        printf("%7lu,", samples[i]);
        if (i % 10 == 9)
        {
            printf("\n");
        }
    }
    if (num_samples % 10 != 0)
    {
        printf("\n");
    }

    return 0;
}
