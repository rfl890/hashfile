#include <stdio.h>
#include <math.h>

#include "common.h"
#include "progress.h"

void progress_init(progress_state_t *in, char *title, size_t total_steps) {
    in->total = (double)total_steps;
    in->buffer[0] = '[';
    for (size_t i = 1; i <= 38; i++) {
        in->buffer[i] = ' ';
    }
    in->buffer[39] = ']';
    in->buffer[40] = '\x00';
    in->title = title;
    in->progress = 0;
    in->finalized = false;
    in->last_chars_to_fill = 0;
}

void progress_final(progress_state_t *in, bool print_newline) {
    if (!in->finalized) {
        in->finalized = true;
        if (print_newline) {
            errprintf("%c", '\n');
        }
    }
}

void progress_update(progress_state_t *in, size_t amount) {
    if (in->finalized)
        return;
    in->progress += amount;

    size_t chars_to_fill = (size_t)floor((in->progress / in->total) * 38);
    if (chars_to_fill > in->last_chars_to_fill) {
        in->last_chars_to_fill = chars_to_fill;
        for (size_t i = 1; i <= chars_to_fill; i++) {
            if (i < chars_to_fill) {
                in->buffer[i] = '=';
            } else {
                if (in->progress >= in->total) {
                    in->buffer[i] = '=';
                } else {
                    in->buffer[i] = '>';
                }
            }
        }
        errprintf("%s%s %zu%%\r", in->title, in->buffer,
               (size_t)floor((in->progress / in->total) * 100));
        fflush(stderr);
    }
}