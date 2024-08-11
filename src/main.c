#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>

#include <Windows.h>
#include <winternl.h>
#include <bcrypt.h>

#define BUFFER_SIZE 1024 * 1024

#include "common.h"
#include "hash.h"
#include "progress.h"
#include "util.h"

int wmain(int argc, wchar_t *argv[]) {
    if (argc < 2) {
        werrprintf(L"%s%s%s\n", L"usage: ", argv[0], L" file1 file2 file3 ...");
        return EXIT_FAILURE;
    }
    cng_hash_algorithm_t alg_sha256 =
        cng_hash_get_algorithm(BCRYPT_SHA256_ALGORITHM, NULL, true);
    if (alg_sha256.result != STATUS_SUCCESS) {
        werrprintf(L"Error fetching hash algorithm\n");
        werrprintf(L"Code: 0x%08x\n", alg_sha256.result);

        return EXIT_FAILURE;
    }

    cng_hash_object_t hash_state = cng_hash_new(&alg_sha256);
    if (hash_state.result != STATUS_SUCCESS) {
        werrprintf(L"Error creating hash object\n");
        werrprintf(L"Code: 0x%08x\n", hash_state.result);

        return EXIT_FAILURE;
    }

    unsigned char *buffer =
        VirtualAlloc(NULL, BUFFER_SIZE, MEM_COMMIT | MEM_RESERVE,
                     PAGE_READWRITE);
    if (buffer == NULL) {
        werrprintf(L"Error allocating memory: %s\n",
                   get_error_str(GetLastError()));
        return EXIT_FAILURE;
    }

    unsigned char *hash_buffer = malloc(hash_state.output_size);
    if (hash_buffer == NULL) {
        werrprintf(L"Unrecoverable error\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        HANDLE file_handle =
            CreateFileW(argv[i], GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                        NULL);
        if (file_handle == INVALID_HANDLE_VALUE) {
            wchar_t *err_str = get_error_str(GetLastError());
            werrprintf(L"Error opening file %s: %s\n", argv[i], err_str);
            LocalFree(err_str);
            continue;
        }

        DWORD bytes_read;
        NTSTATUS status;
        progress_state_t progress;
        LARGE_INTEGER file_size;

        if (GetFileSizeEx(file_handle, &file_size) == 0) {
            wchar_t *err_str = get_error_str(GetLastError());
            werrprintf(L"Error getting filesize for file %s: %s\n", argv[i],
                       err_str);
            LocalFree(err_str);
            continue;
        }

        progress_init(&progress, "", file_size.QuadPart);

        while (1) {
            if (ReadFile(file_handle, buffer, BUFFER_SIZE, &bytes_read, NULL) !=
                TRUE) {
                wchar_t *err_str = get_error_str(GetLastError());
                werrprintf(L"Error reading from file %s: %s\n", argv[i],
                           err_str);
                LocalFree(err_str);
                CloseHandle(file_handle);
                continue;
            }

            if (bytes_read == 0) {
                break;
            }

            if ((status = cng_hash_update(&hash_state, buffer, bytes_read)) !=
                STATUS_SUCCESS) {
                wchar_t *err_str = get_error_str(RtlNtStatusToDosError(status));
                werrprintf(L"Error hashing file %s: %s\n", argv[i], err_str);

                return EXIT_FAILURE;
            }

            progress_update(&progress, bytes_read);

            if (bytes_read < BUFFER_SIZE) {
                break;
            }
        }

        CloseHandle(file_handle);
        if (cng_hash_final(&hash_state, hash_buffer) != STATUS_SUCCESS) {
            wchar_t *err_str = get_error_str(RtlNtStatusToDosError(status));
            werrprintf(L"Error hashing file %s: %s\n", argv[i], err_str);

            return EXIT_FAILURE;
        }

        progress_final(&progress, false);
        wprintf(L"SHA-256 of file %s: ", argv[i]);
        for (unsigned int i = 0; i < hash_state.output_size; i++) {
            wprintf(L"%02x", hash_buffer[i]);
        }
        wprintf(L"%c", L'\n');
    }
}