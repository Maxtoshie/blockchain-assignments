#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <stdint.h>
#include <stdbool.h>
#include "transaction.h"

#define MAX_ACCOUNT_NAME 64
#define MAX_ADDRESS_LENGTH 64

/**
 * struct account - Represents an account
 */
typedef struct account {
    char address[MAX_ADDRESS_LENGTH];
    double balance;
    uint64_t nonce;
    char name[MAX_ACCOUNT_NAME];
} account_t;

/* Function prototypes */
account_t *create_account(const char *address, const char *name);
void free_account(account_t *account);
int update_account_balance(const char *address, double amount);
double get_account_balance(const char *address);
uint64_t get_account_nonce(const char *address);
int increment_account_nonce(const char *address);
int validate_nonce(const char *address, uint64_t tx_nonce);
void register_member(const char *address, const char *name);

#endif /* ACCOUNT_H */
