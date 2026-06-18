#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include "transaction.h"

#define SHA256_DIGEST_LENGTH 32

/* SHA-256 */
void sha256_hash(const unsigned char *data, size_t len, unsigned char *hash);

void compute_merkle_root(transaction_t * const *transactions, uint32_t count,
                         unsigned char *root);

int hash_meets_target(const unsigned char *hash, const unsigned char *target);

void compute_target(uint32_t difficulty, unsigned char *target);

void print_hash(const unsigned char *hash);

/* FIXED: single PoW verification model */
int verify_pow(const unsigned char *hash, const unsigned char *target);

/* ECDSA */
EVP_PKEY *generate_key_pair(void);

unsigned char *get_public_key(EVP_PKEY *key, size_t *len);

unsigned char *get_private_key(EVP_PKEY *key, size_t *len);

int sign_data(const unsigned char *data, size_t data_len, EVP_PKEY *key,
              unsigned char *signature, size_t *sig_len);

int verify_signature(const unsigned char *data, size_t data_len,
                     const unsigned char *signature, size_t sig_len,
                     EVP_PKEY *key);

void free_key(EVP_PKEY *key);

#endif
