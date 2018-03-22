#include <atomic>

#define MAX_WORKER_COUNT 10

struct ProcessControl {
    std::atomic<int> val;
};

struct ControlTable {
    struct ProcessControl controlBlock[MAX_WORKER_COUNT];
};