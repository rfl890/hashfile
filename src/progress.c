#include <stdio.h>
#include <math.h>

#include "progress.h"

void progress_start(progress_state_t *in, char *title, size_t total_steps) {
    in->total = (double)total_steps;
    in->buffer[0] = '[';
    for (size_t i = 1; i <= 38; i++) {
        in->buffer[i] = ' ';
    }
    in->buffer[39] = ']';
    in->buffer[40] = '\x00';
    in->title = title;
    in->progress = 0;
    in->finalized = 0;
}

void progress_finalize(progress_state_t *in) {
    if (!in->finalized) {
        in->finalized = 1;
        printf("%c", '\n');
    }
}

void progress_update(progress_state_t *in, size_t amount) {
    if (in->finalized)
        return;
    char should_finalize = 0;
    in->progress += amount;
    if (in->progress >= in->total) {
        in->progress = in->total;
        should_finalize = 1;
    }
    size_t equal_signs_to_fill = (size_t)floor((in->progress / in->total) * 38);
    for (size_t i = 1; i <= equal_signs_to_fill; i++) {
        if (i < equal_signs_to_fill) {
            in->buffer[i] = '=';
        } else {
            if (should_finalize) {
                in->buffer[i] = '=';
            } else {
                in->buffer[i] = '>';
            }
        }
    }
    printf("%s%s %zu%%\r", in->title, in->buffer,
           (size_t)floor((in->progress / in->total) * 100));
    fflush(stdout);
    if (should_finalize) {
        progress_finalize(in);
    }
}