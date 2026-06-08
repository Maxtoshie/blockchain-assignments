#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Student *student_registry = NULL;
PendingAttendance *pending_pool = NULL;
Block *blockchain_head = NULL;
Block *blockchain_tail = NULL;
int total_mining_attempts = 0;

/* Initialize SHA-256 hash using EVP */
void init_sha256(char *output, const char *input) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create EVP context\n");
        output[0] = '\0';
        return;
    }
    
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        fprintf(stderr, "Failed to initialize digest\n");
        EVP_MD_CTX_free(ctx);
        output[0] = '\0';
        return;
    }
    
    if (EVP_DigestUpdate(ctx, input, strlen(input)) != 1) {
        fprintf(stderr, "Failed to update digest\n");
        EVP_MD_CTX_free(ctx);
        output[0] = '\0';
        return;
    }
    
    if (EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1) {
        fprintf(stderr, "Failed to finalize digest\n");
        EVP_MD_CTX_free(ctx);
        output[0] = '\0';
        return;
    }
    
    for (unsigned int i = 0; i < hash_len; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
    
    EVP_MD_CTX_free(ctx);
}

/* Compute block hash */
void compute_block_hash(Block *block) {
    char input[1024];
    sprintf(input, "%d%s%s%s%d%ld%s%d", 
            block->index, block->student_id, block->student_name,
            block->course, block->token_reward, block->timestamp,
            block->prev_hash, block->nonce);
    init_sha256(block->hash, input);
}

/* Mine block with proof-of-work */
int mine_block(Block *block, int difficulty) {
    char target[65];
    char hash_prefix[65];
    
    memset(target, '0', difficulty);
    target[difficulty] = '\0';
    
    block->nonce = 0;
    total_mining_attempts = 0;
    
    while (1) {
        compute_block_hash(block);
        strncpy(hash_prefix, block->hash, difficulty);
        hash_prefix[difficulty] = '\0';
        
        total_mining_attempts++;
        
        if (strcmp(hash_prefix, target) == 0) {
            printf("✓ Block mined! Nonce: %d, Attempts: %d\n", 
                   block->nonce, total_mining_attempts);
            printf("  Hash: %s\n", block->hash);
            return 1;
        }
        block->nonce++;
        if (block->nonce > 10000000) return 0;
    }
}

/* Add pending attendance */
void add_pending_attendance(const char *id, const char *name, 
                            const char *course, AttendanceStatus status) {
    PendingAttendance *new_pending = malloc(sizeof(PendingAttendance));
    if (!new_pending) return;
    
    strcpy(new_pending->student_id, id);
    strcpy(new_pending->student_name, name);
    strcpy(new_pending->course, course);
    new_pending->status = status;
    
    if (status == PRESENT) new_pending->token_reward = 10;
    else if (status == LATE) new_pending->token_reward = 5;
    else new_pending->token_reward = 0;
    
    new_pending->timestamp = time(NULL);
    new_pending->next = pending_pool;
    pending_pool = new_pending;
    
    printf("✓ Attendance added to pending pool: %s - %s (%d tokens)\n", 
           name, status == PRESENT ? "PRESENT" : 
           (status == LATE ? "LATE" : "ABSENT"), new_pending->token_reward);
}

/* Print pending pool */
void print_pending_pool(void) {
    if (!pending_pool) {
        printf("Pending pool is empty.\n");
        return;
    }
    
    printf("\n=== PENDING ATTENDANCE POOL ===\n");
    PendingAttendance *curr = pending_pool;
    int count = 1;
    while (curr) {
        printf("%d. %s (%s) - %s: %d tokens\n", 
               count++, curr->student_name, curr->student_id,
               curr->status == PRESENT ? "PRESENT" : 
               (curr->status == LATE ? "LATE" : "ABSENT"),
               curr->token_reward);
        curr = curr->next;
    }
}

/* Create block from pending record */
Block* create_block_from_pending(PendingAttendance *pending, const char *prev_hash) {
    Block *new_block = malloc(sizeof(Block));
    if (!new_block) return NULL;
    
    new_block->index = blockchain_tail ? blockchain_tail->index + 1 : 0;
    strcpy(new_block->student_id, pending->student_id);
    strcpy(new_block->student_name, pending->student_name);
    strcpy(new_block->course, pending->course);
    new_block->status = pending->status;
    new_block->token_reward = pending->token_reward;
    
    /* Create transaction ID */
    char tx_input[256];
    sprintf(tx_input, "%s%s%s%d%ld", pending->student_id, pending->student_name,
            pending->course, pending->token_reward, pending->timestamp);
    init_sha256(new_block->transaction_id, tx_input);
    
    new_block->timestamp = pending->timestamp;
    strcpy(new_block->prev_hash, prev_hash ? prev_hash : "0");
    new_block->nonce = 0;
    new_block->next = NULL;
    
    return new_block;
}

/* Confirm pending block and add to blockchain */
int confirm_pending_block(Block *block) {
    if (!blockchain_head) {
        blockchain_head = block;
        blockchain_tail = block;
    } else {
        blockchain_tail->next = block;
        blockchain_tail = block;
    }
    
    printf("\n✓ Block #%d added to blockchain!\n", block->index);
    return 1;
}

/* Print blockchain */
void print_blockchain(void) {
    printf("\n=== BLOCKCHAIN ===\n");
    Block *curr = blockchain_head;
    int count = 1;
    while (curr) {
        printf("Block %d: %s (%s) - %s | Reward: %d tokens\n",
               count++, curr->student_name, curr->student_id,
               curr->status == PRESENT ? "PRESENT" : 
               (curr->status == LATE ? "LATE" : "ABSENT"),
               curr->token_reward);
        printf("  Hash: %s...\n", curr->hash);
        printf("  TX ID: %s...\n", curr->transaction_id);
        curr = curr->next;
    }
}

/* Student registry functions */
void add_student(const char *id, const char *name) {
    Student *new_student = malloc(sizeof(Student));
    if (!new_student) return;
    
    strcpy(new_student->id, id);
    strcpy(new_student->name, name);
    new_student->next = student_registry;
    student_registry = new_student;
}

Student* find_student(const char *id) {
    Student *curr = student_registry;
    while (curr) {
        if (strcmp(curr->id, id) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void init_student_registry(void) {
    add_student("S001", "Alice Johnson");
    add_student("S002", "Bob Smith");
    add_student("S003", "Charlie Brown");
    add_student("S004", "Diana Prince");
    add_student("S005", "Evan Wright");
}
