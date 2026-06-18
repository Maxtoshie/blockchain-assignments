#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "insurance.h"
#include "blockchain.h"
#include "mempool.h"
#include "account.h"
#include "fraud.h"

#define MAX_POLICIES 1000
#define MAX_CLAIMS 1000

/**
 * policy_t - Insurance policy structure
 */
typedef struct {
    char policy_id[64];
    char member_id[64];
    char coverage_plan[64];
    time_t enrollment_date;
    time_t expiry_date;
    char status[16];
    double premium_amount;
} policy_t;

/**
 * claim_t - Insurance claim structure
 */
typedef struct {
    char claim_id[64];
    char policy_id[64];
    char provider_id[64];
    double amount;
    char status[16];
    time_t submission_date;
    time_t approval_date;
    double approved_amount;
    char rejection_reason[256];
} claim_t;

static policy_t policies[MAX_POLICIES];
static int policy_count = 0;
static claim_t claims[MAX_CLAIMS];
static int claim_count = 0;

/**
 * find_policy - Find policy by ID
 */
void *find_policy(const char *policy_id)
{
    for (int i = 0; i < policy_count; i++) {
        if (strcmp(policies[i].policy_id, policy_id) == 0) {
            return &policies[i];
        }
    }
    return NULL;
}

/**
 * find_claim - Find claim by ID
 */
void *find_claim(const char *claim_id)
{
    for (int i = 0; i < claim_count; i++) {
        if (strcmp(claims[i].claim_id, claim_id) == 0) {
            return &claims[i];
        }
    }
    return NULL;
}

/**
 * enroll_policy - Enroll a new policy
 */
int enroll_policy(const char *member_id, const char *coverage_plan, 
                  double premium_amount)
{
    policy_t *policy;
    
    if (policy_count >= MAX_POLICIES) {
        printf("Maximum policies reached\n");
        return -1;
    }
    
    policy = &policies[policy_count];
    snprintf(policy->policy_id, sizeof(policy->policy_id), 
             "POL-%u-%ld", policy_count + 1, time(NULL));
    strncpy(policy->member_id, member_id, sizeof(policy->member_id) - 1);
    policy->member_id[sizeof(policy->member_id) - 1] = '\0';
    strncpy(policy->coverage_plan, coverage_plan, sizeof(policy->coverage_plan) - 1);
    policy->coverage_plan[sizeof(policy->coverage_plan) - 1] = '\0';
    policy->enrollment_date = time(NULL);
    policy->expiry_date = policy->enrollment_date + 365 * 24 * 3600;
    strcpy(policy->status, "ACTIVE");
    policy->premium_amount = premium_amount;
    
    policy_count++;
    
    printf("Policy enrolled: %s for member %s\n", policy->policy_id, member_id);
    return 0;
}

/**
 * pay_premium - Pay premium with reinsurance contribution
 */
int pay_premium(const char *member_id, const char *policy_id, double amount)
{
    policy_t *policy = (policy_t *)find_policy(policy_id);
    
    if (!policy) {
        printf("Policy not found: %s\n", policy_id);
        return -1;
    }
    
    if (strcmp(policy->status, "ACTIVE") != 0) {
        printf("Policy is not active: %s\n", policy_id);
        return -1;
    }
    
    double balance = get_account_balance(member_id);
    if (balance < amount) {
        printf("Insufficient balance: %.2f < %.2f\n", balance, amount);
        return -1;
    }
    
    update_account_balance(member_id, -amount);
    update_account_balance("INSURANCE_POOL", amount);
    
    /* Reinsurance contribution (5%) */
    double reinsurance_amount = amount * 0.05;
    update_account_balance("INSURANCE_POOL", -reinsurance_amount);
    update_account_balance("REINSURANCE_POOL", reinsurance_amount);
    
    printf("Premium paid: %.2f AHT for policy %s\n", amount, policy_id);
    printf("Reinsurance contribution: %.2f AHT\n", reinsurance_amount);
    return 0;
}

/**
 * submit_claim - Submit a claim
 */
int submit_claim(const char *policy_id, const char *provider_id, double amount)
{
    policy_t *policy = (policy_t *)find_policy(policy_id);
    claim_t *claim;
    
    if (!policy) {
        printf("Policy not found: %s\n", policy_id);
        return -1;
    }
    
    /* Check policy expiry */
    time_t current = time(NULL);
    if (current > policy->expiry_date) {
        strcpy(policy->status, "EXPIRED");
        printf("Policy expired\n");
        return -1;
    }
    
    if (strcmp(policy->status, "ACTIVE") != 0) {
        printf("Policy is not active: %s\n", policy_id);
        return -1;
    }
    
    if (claim_count >= MAX_CLAIMS) {
        printf("Maximum claims reached\n");
        return -1;
    }
    
    claim = &claims[claim_count];
    snprintf(claim->claim_id, sizeof(claim->claim_id),
             "CLM-%u-%ld", claim_count + 1, time(NULL));
    strncpy(claim->policy_id, policy_id, sizeof(claim->policy_id) - 1);
    claim->policy_id[sizeof(claim->policy_id) - 1] = '\0';
    strncpy(claim->provider_id, provider_id, sizeof(claim->provider_id) - 1);
    claim->provider_id[sizeof(claim->provider_id) - 1] = '\0';
    claim->amount = amount;
    strcpy(claim->status, "PENDING");
    claim->submission_date = time(NULL);
    claim->approved_amount = 0.0;
    claim->rejection_reason[0] = '\0';
    
    claim_count++;
    
    /* Check for fraud */
    if (detect_fraud(policy->member_id, provider_id, amount, claim->claim_id)) {
        strcpy(claim->status, "SUSPICIOUS");
        printf("Claim flagged as SUSPICIOUS: %s\n", claim->claim_id);
        return 1;
    }
    
    printf("Claim submitted: %s for %.2f AHT\n", claim->claim_id, amount);
    return 0;
}

/**
 * approve_claim - Approve a claim
 */
int approve_claim(const char *claim_id, double amount)
{
    claim_t *claim = (claim_t *)find_claim(claim_id);
    
    if (!claim) {
        printf("Claim not found: %s\n", claim_id);
        return -1;
    }
    
    if (strcmp(claim->status, "PENDING") != 0) {
        printf("Claim is not pending: %s\n", claim_id);
        return -1;
    }
    
    strcpy(claim->status, "APPROVED");
    claim->approved_amount = amount;
    claim->approval_date = time(NULL);
    
    printf("Claim approved: %s for %.2f AHT\n", claim_id, amount);
    return 0;
}

/**
 * reject_claim - Reject a claim
 */
int reject_claim(const char *claim_id, const char *reason)
{
    claim_t *claim = (claim_t *)find_claim(claim_id);
    
    if (!claim) {
        printf("Claim not found: %s\n", claim_id);
        return -1;
    }
    
    strcpy(claim->status, "REJECTED");
    strncpy(claim->rejection_reason, reason, sizeof(claim->rejection_reason) - 1);
    claim->rejection_reason[sizeof(claim->rejection_reason) - 1] = '\0';
    
    printf("Claim rejected: %s\n", claim_id);
    return 0;
}

/**
 * settle_claim - Settle a claim
 */
int settle_claim(const char *claim_id, double approved_amount)
{
    claim_t *claim = (claim_t *)find_claim(claim_id);
    double insurance_balance, reinsurance_balance;
    
    if (!claim) {
        printf("Claim not found: %s\n", claim_id);
        return -1;
    }
    
    if (strcmp(claim->status, "APPROVED") != 0) {
        printf("Claim not approved: %s\n", claim_id);
        return -1;
    }
    
    strcpy(claim->status, "SETTLED");
    claim->approved_amount = approved_amount;
    claim->approval_date = time(NULL);
    
    insurance_balance = get_account_balance("INSURANCE_POOL");
    reinsurance_balance = get_account_balance("REINSURANCE_POOL");
    
    if (approved_amount <= 1000.0) {
        /* Simple settlement from insurance pool */
        if (insurance_balance < approved_amount) {
            printf("Insufficient insurance pool balance\n");
            return -1;
        }
        
        update_account_balance("INSURANCE_POOL", -approved_amount);
        update_account_balance(claim->provider_id, approved_amount);
    } else {
        /* Split settlement */
        double insurance_part = 1000.0;
        double reinsurance_part = approved_amount - 1000.0;
        
        if (insurance_balance < insurance_part) {
            printf("Insufficient insurance pool balance\n");
            return -1;
        }
        
        if (reinsurance_balance < reinsurance_part) {
            printf("Insufficient reinsurance pool balance. Partial approval needed.\n");
            reinsurance_part = reinsurance_balance;
        }
        
        update_account_balance("INSURANCE_POOL", -insurance_part);
        update_account_balance(claim->provider_id, insurance_part);
        
        if (reinsurance_part > 0) {
            update_account_balance("REINSURANCE_POOL", -reinsurance_part);
            update_account_balance(claim->provider_id, reinsurance_part);
        }
    }
    
    printf("Claim settled: %s for %.2f AHT\n", claim_id, approved_amount);
    return 0;
}

/**
 * renew_policy - Renew a policy
 */
int renew_policy(const char *policy_id)
{
    policy_t *policy = (policy_t *)find_policy(policy_id);
    
    if (!policy) {
        printf("Policy not found: %s\n", policy_id);
        return -1;
    }
    
    policy->expiry_date = time(NULL) + 365 * 24 * 3600;
    strcpy(policy->status, "RENEWED");
    
    printf("Policy renewed: %s\n", policy_id);
    return 0;
}

/**
 * show_policy_status - Display policy status
 */
void show_policy_status(const char *policy_id)
{
    policy_t *policy = (policy_t *)find_policy(policy_id);
    
    if (!policy) {
        printf("Policy not found: %s\n", policy_id);
        return;
    }
    
    printf("\n=== POLICY STATUS ===\n");
    printf("Policy ID: %s\n", policy->policy_id);
    printf("Member: %s\n", policy->member_id);
    printf("Coverage Plan: %s\n", policy->coverage_plan);
    printf("Status: %s\n", policy->status);
    printf("Enrollment Date: %s", ctime(&policy->enrollment_date));
    printf("Expiry Date: %s", ctime(&policy->expiry_date));
    printf("Premium: %.2f AHT\n", policy->premium_amount);
}
