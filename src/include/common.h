#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

#define handleError(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#endif