// Copyright 2023 Nastase Cristian-Gabriel 315CA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef UTILS_H_
#define UTILS_H_

/* useful macro for handling error codes */
#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)

#endif  // UTILS_H_
