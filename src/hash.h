#ifndef HASH_H
#define HASH_H

#include <stdbool.h>
#include <stdint.h>

#include <Windows.h>
#include <bcrypt.h>

#define STATUS_SUCCESS ((NTSTATUS)0x00000000)

typedef struct cng_hash_algorithm {
    BCRYPT_ALG_HANDLE handle;
    NTSTATUS result;
    bool reusable;
} cng_hash_algorithm_t;

typedef struct cng_hash_object {
    BCRYPT_HASH_HANDLE handle;
    void *hash_obj;
    NTSTATUS result;
    DWORD output_size;
} cng_hash_object_t;

cng_hash_algorithm_t cng_hash_get_algorithm(
    const wchar_t *algorithm_id, const wchar_t *provider, bool reusable);
cng_hash_object_t cng_hash_new(cng_hash_algorithm_t *algorithm);
NTSTATUS cng_hash_update(cng_hash_object_t *hash_obj, void *data,
                                       unsigned long length);
NTSTATUS cng_hash_final(cng_hash_object_t *hash_obj,
                                      void *output);

void cng_hash_free_hash_object(cng_hash_object_t *hash_obj);
void cng_hash_free_hash_algorithm(cng_hash_algorithm_t *algorithm);

#endif