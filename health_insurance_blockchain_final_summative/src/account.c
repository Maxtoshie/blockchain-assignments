#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

#define MAX_ACCOUNTS 1000

static account_t *accounts[MAX_ACCOUNTS];
static int account_count = 0;

/**
 * create_account - Create a new account
 */
account_t *create_account(const char *address, const char *name)
{
    account_t *account;
    
    if (account_count >= MAX_ACCOUNTS) {
        printf("Maximum accounts reached\n");
        return NULL;
    }
    
    account = (account_t *)calloc(1, sizeof(account_t));
    if (!account)
        return NULL;
    
    strncpy(account->address, address, MAX_ADDRESS_LENGTH - 1);
    strncpy(account->name, name, MAX_ACCOUNT_NAME - 1);
    account->balance = 0.0;
    account->nonce = 0;
    
    accounts[account_count++] = account;
    return account;
}

/**
 * find_account - Find account by address
 */
static account_t *find_account(const char *address)
{
    for (int i = 0; i < account_count; i++) {
        if (strcmp(accounts[i]->address, address) == 0)
            return accounts[i];
    }
    return NULL;
}

/**
 * register_member - Register a new member
 */
void register_member(const char *address, const char *name)
{
    account_t *account = find_account(address);
    if (!account) {
        account = create_account(address, name);
        if (account) {
            account->balance = 1000.0;  /* Initial balance */
            printf("Member registered: %s (%s)\n", name, address);
        }
    } else {
        printf("Member already registered: %s\n", address);
    }
}

/**
 * update_account_balance - Update account balance
 */
int update_account_balance(const char *address, double amount)
{
    account_t *account = find_account(address);
    if (!account) {
        /* Create account if it doesn't exist */
        account = create_account(address, address);
        if (!account)
            return -1;
    }
    
    account->balance += amount;
    return 0;
}

/**
 * get_account_balance - Get account balance
 */
double get_account_balance(const char *address)
{
    account_t *account = find_account(address);
    if (!account)
        return 0.0;
    return account->balance;
}

/**
 * get_account_nonce - Get account nonce
 */
uint64_t get_account_nonce(const char *address)
{
    account_t *account = find_account(address);
    if (!account)
        return 0;
    return account->nonce;
}

/**
 * increment_account_nonce - Increment account nonce
 */
int increment_account_nonce(const char *address)
{
    account_t *account = find_account(address);
    if (!account)
        return -1;
    
    account->nonce++;
    return 0;
}

/**
 * validate_nonce - Validate transaction nonce
 */
int validate_nonce(const char *address, uint64_t tx_nonce)
{
    uint64_t current_nonce = get_account_nonce(address);
    return (tx_nonce == current_nonce + 1) ? 1 : 0;
}

/**
 * free_account - Free an account
 */
void free_account(account_t *account)
{
    if (account)
        free(account);
}
