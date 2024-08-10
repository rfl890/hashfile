#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>

#include <Windows.h>
#include <bcrypt.h>

#define BUFFER_SIZE 1024 * 1024

#include "hash.h"
#include "progress.h"

int wmain(int argc, wchar_t *argv[]) {
    if (argc < 2) {
        wprintf(L"%s%s%s\n", L"usage: ", argv[0], L" file");
        return EXIT_FAILURE;
    }
    FILE *input = _wfopen(argv[1], L"rb");
    if (input == NULL) {
        wprintf(L"error opening input file %s: %s\n", argv[1],
                _wcserror(errno));
        return EXIT_FAILURE;
    }

    struct _stat64 input_info;
    char buffer[BUFFER_SIZE];

    if (_fstat64(fileno(input), &input_info) == -1) {
        wprintf(L"cannot stat input file %s: %s\n", argv[1], _wcserror(errno));
        return EXIT_FAILURE;
    }

    progress_state_t progress;
    progress_start(&progress, "", input_info.st_size);
    cng_hash_algorithm_t hash_alg =
        cng_hash_get_algorithm(L"SHA256", NULL, false);
    if (hash_alg.result != STATUS_SUCCESS) {
        wprintf(L"Error hashing data\n");
        return EXIT_FAILURE;
    }
    cng_hash_object_t hash_obj = cng_hash_new(&hash_alg);
    if (hash_obj.result != STATUS_SUCCESS) {
        wprintf(L"Error hashing data\n");
        return EXIT_FAILURE;
    }

    while (1) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, input);
        bool should_break = false;

        if (bytes_read < BUFFER_SIZE) {
            if (feof(input) && !ferror(input)) {
                should_break = true;
            } else {
                fprintf(stderr, "error reading input file: %s\n",
                        strerror(errno));
                return EXIT_FAILURE;
            }
        }

        if (cng_hash_update(&hash_obj, buffer, bytes_read) != STATUS_SUCCESS) {
            wprintf(L"Error hashing data\n");
            return EXIT_FAILURE;
        }
        progress_update(&progress, bytes_read);

        if (should_break) {
            break;
        };
    }

    unsigned char hash[32];

    progress_finalize(&progress);
    if (cng_hash_final(&hash_obj, hash) != STATUS_SUCCESS) {
        wprintf(L"Error hashing data\n");
        return EXIT_FAILURE;
    }

    wprintf(L"SHA-256 hash of file %s: ", argv[1]);
    for (int i = 0; i < 32; i++) {
        wprintf(L"%02x", hash[i]);
    }
    wprintf(L"\n");
}