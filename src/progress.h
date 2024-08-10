#ifndef PROGRESS_H
#define PROGRESS_H

#include <stdio.h>

typedef struct progress_state {
    char buffer[41];
    char *title;
    double total;
    double progress;
    char finalized;
} progress_state_t;

void progress_start(progress_state_t *in, char *title, size_t total_steps);
void progress_update(progress_state_t *in, size_t amount);
void progress_finalize(progress_state_t *in);

#endif