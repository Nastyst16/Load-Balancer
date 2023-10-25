/* Copyright 2023 <> */

// #pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
// #include "server.h"
// #include "load_balancer.h"
// #include "hashtable_impl.h"

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

#endif /* UTILS_H_ */

// compare function for sorting the servers


