#include <atomic>
#include <stdint.h>

#define MAX_WORKER_COUNT 10
#define WORKER_WAKE 1
#define WORKER_SLEEP 0

struct ProcessControl {
    int futex;
    std::atomic<uint64_t> val;
};

struct ControlTable {
    std::atomic<int> nextWorkerId;
    struct ProcessControl controlBlock[MAX_WORKER_COUNT];
};