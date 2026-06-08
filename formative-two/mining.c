#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Solo mining simulation */
void solo_mining(int difficulty) {
    if (!pending_pool) {
        printf("No pending attendance to mine.\n");
        return;
    }
    
    printf("\n=== SOLO MINING SIMULATION ===\n");
    printf("Mining %d pending block(s) with difficulty %d\n", 
           pending_pool ? 1 : 0, difficulty);
    
    PendingAttendance *to_mine = pending_pool;
    pending_pool = pending_pool->next;
    
    Block *new_block = create_block_from_pending(to_mine, 
                          blockchain_tail ? blockchain_tail->hash : "0");
    
    if (mine_block(new_block, difficulty)) {
        confirm_pending_block(new_block);
        
        /* Update balance based on model */
        if (transaction_model == 1) { /* UTXO */
            add_utxo(new_block->transaction_id, new_block->student_id, 
                    new_block->token_reward);
            /* Add mining reward as UTXO */
            char miner_tx[65];
            char miner_input[256];
            sprintf(miner_input, "MINER%d%ld", new_block->index, time(NULL));
            init_sha256(miner_tx, miner_input);
            add_utxo(miner_tx, "MINER", MINING_REWARD);
            printf("✓ Mining reward of %d tokens added to MINER\n", MINING_REWARD);
        } else { /* Account model */
            Account *student_acc = get_account(new_block->student_id);
            if (student_acc) student_acc->balance += new_block->token_reward;
            Account *miner_acc = get_account("MINER");
            if (!miner_acc) {
                /* Create MINER account if needed */
                Account *new_miner = malloc(sizeof(Account));
                strcpy(new_miner->student_id, "MINER");
                new_miner->balance = MINING_REWARD;
                new_miner->nonce = 0;
                new_miner->history = NULL;
                new_miner->next = account_list;
                account_list = new_miner;
            } else {
                miner_acc->balance += MINING_REWARD;
            }
            printf("✓ Mining reward of %d tokens added to MINER\n", MINING_REWARD);
        }
        
        free(to_mine);
    } else {
        printf("✗ Failed to mine block\n");
        free(new_block);
    }
}

/* Pool mining simulation */
void pool_mining(int num_miners, int difficulty) {
    if (!pending_pool) {
        printf("No pending attendance to mine.\n");
        return;
    }
    
    printf("\n=== POOL MINING SIMULATION ===\n");
    printf("Pool with %d miners mining at difficulty %d\n", num_miners, difficulty);
    
    /* Simulate miner hash rates */
    int *attempts = malloc(num_miners * sizeof(int));
    int total_attempts = 0;
    
    srand(time(NULL));
    for (int i = 0; i < num_miners; i++) {
        attempts[i] = rand() % 10000 + 1000;
        total_attempts += attempts[i];
    }
    
    /* Mine the block */
    PendingAttendance *to_mine = pending_pool;
    pending_pool = pending_pool->next;
    
    Block *new_block = create_block_from_pending(to_mine, 
                          blockchain_tail ? blockchain_tail->hash : "0");
    
    if (mine_block(new_block, difficulty)) {
        confirm_pending_block(new_block);
        
        /* Calculate and distribute rewards */
        int total_reward = MINING_REWARD + new_block->token_reward;
        int pool_fee = (total_reward * POOL_FEE_PERCENT) / 100;
        int distributed_reward = total_reward - pool_fee;
        
        printf("\n--- Reward Distribution ---\n");
        printf("Total reward: %d tokens\n", total_reward);
        printf("Pool fee (%d%%): %d tokens\n", POOL_FEE_PERCENT, pool_fee);
        printf("Distributed: %d tokens\n\n", distributed_reward);
        printf("%-10s %-15s %-15s %-15s\n", "Miner", "Attempts", "Share %%", "Reward");
        printf("------------------------------------------------\n");
        
        for (int i = 0; i < num_miners; i++) {
            float share = (float)attempts[i] / total_attempts * 100;
            int reward = (attempts[i] * distributed_reward) / total_attempts;
            
            printf("%-10d %-15d %-15.2f %-15d\n", i+1, attempts[i], share, reward);
            
            /* Add reward to miner */
            char miner_id[12];
            if (transaction_model == 1) {
                char miner_tx[65];
                char miner_input[256];
                sprintf(miner_input, "POOLMINER%d%d%ld", i, new_block->index, time(NULL));
                init_sha256(miner_tx, miner_input);
                sprintf(miner_id, "M%d", i+1);
                add_utxo(miner_tx, miner_id, reward);
            } else {
                sprintf(miner_id, "M%d", i+1);
                Account *miner_acc = get_account(miner_id);
                if (!miner_acc) {
                    Account *new_miner = malloc(sizeof(Account));
                    strcpy(new_miner->student_id, miner_id);
                    new_miner->balance = reward;
                    new_miner->nonce = 0;
                    new_miner->history = NULL;
                    new_miner->next = account_list;
                    account_list = new_miner;
                } else {
                    miner_acc->balance += reward;
                }
            }
        }
        
        /* Update student balance */
        if (transaction_model == 1) {
            add_utxo(new_block->transaction_id, new_block->student_id, 
                    new_block->token_reward);
        } else {
            Account *student_acc = get_account(new_block->student_id);
            if (student_acc) student_acc->balance += new_block->token_reward;
        }
        
        free(to_mine);
    } else {
        printf("✗ Failed to mine block\n");
        free(new_block);
    }
    
    free(attempts);
}

/* Cloud mining simulation */
void cloud_mining(int rental_duration, int difficulty) {
    printf("\n=== CLOUD MINING SIMULATION ===\n");
    printf("Rental duration: %d rounds, Difficulty: %d\n", rental_duration, difficulty);
    
    int rental_fee_per_round = 15;
    int avg_reward_per_round = 25;
    int gross_earnings = 0;
    int total_fees = 0;
    
    printf("\n%-10s %-15s %-15s %-15s %-15s\n", 
           "Round", "Gross Earnings", "Fees Paid", "Net Profit", "Status");
    printf("--------------------------------------------------------\n");
    
    for (int round = 1; round <= rental_duration; round++) {
        gross_earnings += avg_reward_per_round;
        total_fees += rental_fee_per_round;
        int net_profit = gross_earnings - total_fees;
        
        printf("%-10d %-15d %-15d %-15d ", 
               round, gross_earnings, total_fees, net_profit);
        
        if (net_profit < 0) {
            printf("⚠ WARNING: Unprofitable!\n");
        } else {
            printf("✓ Profitable\n");
        }
    }
    
    printf("\n--- Summary ---\n");
    printf("Total gross earnings: %d tokens\n", gross_earnings);
    printf("Total fees paid: %d tokens\n", total_fees);
    printf("Net profit: %d tokens\n", gross_earnings - total_fees);
    
    if (gross_earnings - total_fees < 0) {
        printf("⚠ This cloud mining rental is unprofitable!\n");
    }
}
