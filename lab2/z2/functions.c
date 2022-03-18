#include <unistd.h>
#include <sys/types.h>
#include "functions.h"

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}
