#ifndef PROGRESS_H
#define PROGRESS_H

#include <stdio.h>
#include <stdbool.h>

typedef struct progress_state {
    char buffer[41];
    char *title;
    double total;
    double progress;
    size_t last_chars_to_fill;
    bool finalized;
} progress_state_t;

void progress_init(progress_state_t *in, char *title, size_t total_steps);
void progress_update(progress_state_t *in, size_t amount);
void progress_final(progress_state_t *in, bool print_newline);

#endif