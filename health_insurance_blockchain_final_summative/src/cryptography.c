#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include "cryptography.h"

/* =========================
 * SHA256
 * ========================= */
void sha256_hash(const unsigned char *data, size_t len, unsigned char *hash)
{
    SHA256(data, len, hash);
}

/* =========================
 * PRINT HASH
 * ========================= */
void print_hash(const unsigned char *hash)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        printf("%02x", hash[i]);
}

/* =========================
 * MERKLE ROOT (updated signature)
 * ========================= */
void compute_merkle_root(transaction_t * const *transactions, uint32_t count,
                         unsigned char *root)
{
    if (!transactions || count == 0) {
        memset(root, 0, SHA256_DIGEST_LENGTH);
        return;
    }

    sha256_hash((unsigned char *)transactions[0],
                sizeof(transaction_t),
                root);
}

/* =========================
 * TARGET GENERATION
 * ========================= */
void compute_target(uint32_t difficulty, unsigned char *target)
{
    memset(target, 0xFF, SHA256_DIGEST_LENGTH);

    for (uint32_t i = 0; i < difficulty && i < SHA256_DIGEST_LENGTH; i++)
        target[i] = 0x00;
}

/* =========================
 * COMPARE HASH VS TARGET
 * ========================= */
int hash_meets_target(const unsigned char *hash, const unsigned char *target)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        if (hash[i] < target[i]) return 1;
        if (hash[i] > target[i]) return 0;
    }
    return 1;
}

/* =========================
 * FIXED POW
 * ========================= */
int verify_pow(const unsigned char *hash, const unsigned char *target)
{
    return hash_meets_target(hash, target);
}

/* =========================
 * STUBS (unchanged)
 * ========================= */
EVP_PKEY *generate_key_pair(void) { return NULL; }

unsigned char *get_public_key(EVP_PKEY *key, size_t *len)
{
    (void)key; (void)len; return NULL;
}

unsigned char *get_private_key(EVP_PKEY *key, size_t *len)
{
    (void)key; (void)len; return NULL;
}

int sign_data(const unsigned char *data, size_t data_len, EVP_PKEY *key,
              unsigned char *signature, size_t *sig_len)
{
    (void)data; (void)data_len; (void)key; (void)signature; (void)sig_len;
    return 0;
}

int verify_signature(const unsigned char *data, size_t data_len,
                     const unsigned char *signature, size_t sig_len,
                     EVP_PKEY *key)
{
    (void)data; (void)data_len; (void)signature; (void)sig_len; (void)key;
    return 1;
}

void free_key(EVP_PKEY *key)
{
    (void)key;
}
