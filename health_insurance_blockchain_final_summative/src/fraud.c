#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fraud.h"
#include "mempool.h"
#include "blockchain.h"

#define MAX_HISTORY 1000

/**
 * claim_history_t - Claim history for fraud detection
 */
typedef struct {
    char member_id[64];
    char provider_id[64];
    time_t claim_time;
    double amount;
    char claim_id[64];
} claim_history_t;

static claim_history_t history[MAX_HISTORY];
static int history_count = 0;

/**
 * detect_fraud - Detect fraudulent transactions
 */
int detect_fraud(const char *member_id, const char *provider_id, 
                 double amount, const char *claim_id)
{
    int suspicious = 0;
    time_t current = time(NULL);
    int claims_24h = 0;
    double total_provider_claims = 0.0;
    int provider_count = 0;
    
    /* Check high-frequency claims */
    for (int i = 0; i < history_count; i++) {
        if (strcmp(history[i].member_id, member_id) == 0) {
            time_t diff = current - history[i].claim_time;
            if (diff < 86400) {  /* 24 hours */
                claims_24h++;
            }
        }
        
        if (strcmp(history[i].provider_id, provider_id) == 0) {
            total_provider_claims += history[i].amount;
            provider_count++;
        }
    }
    
    if (claims_24h >= 3) {
        printf("High-frequency claims: %d claims in 24 hours\n", claims_24h + 1);
        suspicious = 1;
    }
    
    /* Check abnormal claim amount */
    if (provider_count > 0) {
        double avg = total_provider_claims / provider_count;
        if (amount > avg * 2.0) {
            printf("Abnormal claim amount: %.2f > 2x average (%.2f)\n", 
                   amount, avg);
            suspicious = 1;
        }
    }
    
    /* Record history */
    if (history_count < MAX_HISTORY) {
        claim_history_t *h = &history[history_count++];
        strncpy(h->member_id, member_id, sizeof(h->member_id));
        strncpy(h->provider_id, provider_id, sizeof(h->provider_id));
        h->claim_time = current;
        h->amount = amount;
        strncpy(h->claim_id, claim_id, sizeof(h->claim_id));
    }
    
    return suspicious;
}

/**
 * fraud_review - Display suspicious transactions
 */
void fraud_review(void)
{
    printf("\n=== FRAUD REVIEW ===\n");
    printf("No suspicious transactions currently pending.\n");
    printf("Check mempool for transactions with SUSPICIOUS status.\n");
}

/**
 * approve_suspicious - Approve suspicious transaction
 */
int approve_suspicious(const char *tx_id)
{
    printf("Approving suspicious transaction: %s\n", tx_id);
    return 0;
}

/**
 * reject_suspicious - Reject suspicious transaction
 */
int reject_suspicious(const char *tx_id)
{
    printf("Rejecting suspicious transaction: %s\n", tx_id);
    return 0;
}
