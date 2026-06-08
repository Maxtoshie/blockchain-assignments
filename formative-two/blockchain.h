#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>

#define MAX_NAME 50
#define MAX_COURSE 50
#define MAX_STUDENTS 100
#define MAX_PENDING 100
#define DIFFICULTY 2
#define MINING_REWARD 50
#define TRANSACTION_FEE 1
#define POOL_FEE_PERCENT 2

/* Attendance status */
typedef enum {
    PRESENT = 1,
    LATE = 2,
    ABSENT = 3
} AttendanceStatus;

/* Block structure */
typedef struct Block {
    int index;
    char student_id[10];
    char student_name[MAX_NAME];
    char course[MAX_COURSE];
    AttendanceStatus status;
    int token_reward;
    char transaction_id[65];
    time_t timestamp;
    char prev_hash[65];
    char hash[65];
    int nonce;
    struct Block *next;
} Block;

/* Pending attendance record */
typedef struct PendingAttendance {
    char student_id[10];
    char student_name[MAX_NAME];
    char course[MAX_COURSE];
    AttendanceStatus status;
    int token_reward;
    time_t timestamp;
    struct PendingAttendance *next;
} PendingAttendance;

/* UTXO structure */
typedef struct UTXO {
    char tx_id[65];
    char owner_id[10];
    int amount;
    int is_spent;
    struct UTXO *next;
} UTXO;

/* Account structure for account-based model */
typedef struct Account {
    char student_id[10];
    int balance;
    int nonce;
    struct TransactionLog *history;
    struct Account *next;
} Account;

/* Transaction log entry */
typedef struct TransactionLog {
    char from_id[10];
    char to_id[10];
    int amount;
    int fee;
    int nonce;
    time_t timestamp;
    struct TransactionLog *next;
} TransactionLog;

/* Function declarations */
void init_sha256(char *output, const char *input);
void compute_block_hash(Block *block);
int mine_block(Block *block, int difficulty);
void add_pending_attendance(const char *id, const char *name, 
                            const char *course, AttendanceStatus status);
void print_pending_pool(void);
Block* create_block_from_pending(PendingAttendance *pending, const char *prev_hash);
int confirm_pending_block(Block *block);
void print_blockchain(void);

/* UTXO functions */
void init_utxo_model(void);
void add_utxo(const char *tx_id, const char *owner, int amount);
int get_balance_utxo(const char *student_id);
int create_transaction_utxo(const char *from, const char *to, int amount);
void print_utxo_set(void);

/* Account functions */
void init_account_model(void);
Account* get_account(const char *student_id);
int create_transaction_account(const char *from, const char *to, int amount);
void print_account_balance(const char *student_id);
void print_transaction_history(const char *student_id);

/* Mining functions */
void solo_mining(int difficulty);
void pool_mining(int num_miners, int difficulty);
void cloud_mining(int rental_duration, int difficulty);

/* Student registry */
typedef struct Student {
    char id[10];
    char name[MAX_NAME];
    struct Student *next;
} Student;

void add_student(const char *id, const char *name);
Student* find_student(const char *id);
void init_student_registry(void);

/* Global variables */
extern Student *student_registry;
extern PendingAttendance *pending_pool;
extern Block *blockchain_head;
extern Block *blockchain_tail;
extern UTXO *utxo_list;
extern Account *account_list;
extern int transaction_model;
extern int total_mining_attempts;

#endif
