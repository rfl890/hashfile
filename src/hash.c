#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <Windows.h>
#include <bcrypt.h>

#define LIBCNG_SOURCE

#include "hash.h"

cng_hash_algorithm_t cng_hash_get_algorithm(
    const wchar_t *algorithm_id, const wchar_t *provider, bool reusable) {
    cng_hash_algorithm_t alg = {
        .handle = NULL,
        .result = STATUS_SUCCESS,
        .reusable = reusable,
    };

    alg.result =
        BCryptOpenAlgorithmProvider(&alg.handle, algorithm_id, provider,
                                    alg.reusable ? BCRYPT_HASH_REUSABLE_FLAG
                                                 : 0);

    return alg;
}

cng_hash_object_t cng_hash_new(cng_hash_algorithm_t *algorithm) {
    cng_hash_object_t hash_object = {
        .handle = NULL,
        .hash_obj = NULL,

        .result = STATUS_SUCCESS,
        .output_size = 0,
    };
    DWORD hash_object_length = 0;
    ULONG _unused_pcbResult;
    NTSTATUS result;

    BCryptGetProperty(algorithm->handle, BCRYPT_OBJECT_LENGTH,
                      (PUCHAR)&hash_object_length, sizeof(DWORD),
                      &_unused_pcbResult, 0);
    BCryptGetProperty(algorithm->handle, BCRYPT_HASH_LENGTH,
                      (PUCHAR)&hash_object.output_size, sizeof(DWORD),
                      &_unused_pcbResult, 0);

    hash_object.hash_obj =
        HeapAlloc(GetProcessHeap(), 0, (SIZE_T)hash_object_length);
    if (hash_object.hash_obj == NULL) {
        hash_object.result = STATUS_NO_MEMORY;
        return hash_object;
    }

    if ((result =
             BCryptCreateHash(algorithm->handle, &hash_object.handle,
                              (PUCHAR)hash_object.hash_obj, hash_object_length,
                              NULL, 0,
                              algorithm->reusable ? BCRYPT_HASH_REUSABLE_FLAG
                                                  : 0)) != STATUS_SUCCESS) {
        HeapFree(GetProcessHeap(), 0, hash_object.hash_obj);
        hash_object.result = result;
        return hash_object;
    }

    return hash_object;
}

NTSTATUS cng_hash_update(cng_hash_object_t *hash_obj, void *data,
                                       unsigned long length) {
    return BCryptHashData(hash_obj->handle, (PUCHAR)data, length, 0);
}

NTSTATUS cng_hash_final(cng_hash_object_t *hash_obj,
                                      void *output) {
    return BCryptFinishHash(hash_obj->handle, (PUCHAR)output,
                            hash_obj->output_size, 0);
}