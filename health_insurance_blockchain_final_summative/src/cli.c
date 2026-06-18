#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "cli.h"
#include "blockchain.h"
#include "mempool.h"
#include "insurance.h"
#include "mining.h"
#include "fraud.h"
#include "persistence.h"
#include "account.h"

#define MAX_INPUT 256
#define MAX_ADDRESS_LENGTH 64

/**
 * print_header - Print the application header
 */
static void print_header(void)
{
    printf("\n========================================\n");
    printf("   ALU HEALTH INSURANCE BLOCKCHAIN\n");
    printf("        NUMERIC CONTROL PANEL\n");
    printf("========================================\n");
}

/**
 * print_menu - Print the main menu
 */
static void print_menu(void)
{
    printf("\n--- CORE DASHBOARD OPTIONS ---\n");
    printf("1)  [Membership]     Register New Member\n");
    printf("2)  [Policy]         Enroll Insurance Policy\n");
    printf("3)  [Insurance]      Pay Premium (Triggers Reinsurance)\n");
    printf("4)  [Insurance]      Submit Healthcare Claim\n");
    printf("5)  [Mempool]        View Pending Transactions\n");
    printf("6)  [Mining]         Mine Block (Solo / Proof-of-Work)\n");
    printf("7)  [Ledger]         View Complete Blockchain\n");
    printf("8)  [Audit]          Verify Chain Linkage & Signatures\n");
    printf("9)  [Audit]          Fraud Review Pipeline\n");
    printf("10) [Balances]       Check Wallet & Reinsurance Balances\n");
    printf("11) [Policy]         Renew Policy\n");
    printf("12) [Policy]         View Policy Status\n");
    printf("13) [Claim]          Approve Claim\n");
    printf("14) [Claim]          Reject Claim\n");
    printf("15) [Claim]          Settle Claim\n");
    printf("0)  [Exit]           Save State and Terminate Terminal\n");
    printf("----------------------------------------\n");
}

/**
 * get_input - Get user input safely with trailing newline stripping
 */
static void get_input(const char *prompt, char *buffer, size_t size)
{
    printf("%s", prompt);
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

/**
 * get_int_input - Get verified integer input
 */
static int get_int_input(const char *prompt)
{
    char buffer[MAX_INPUT];
    int value;
    
    get_input(prompt, buffer, sizeof(buffer));
    if (sscanf(buffer, "%d", &value) == 1)
        return value;
    return -1;
}

/**
 * get_double_input - Get verified double input
 */
static double get_double_input(const char *prompt)
{
    char buffer[MAX_INPUT];
    double value;
    
    get_input(prompt, buffer, sizeof(buffer));
    if (sscanf(buffer, "%lf", &value) == 1)
        return value;
    return -1.0;
}

/**
 * handle_register_member - Handle member registration
 */
static void handle_register_member(void)
{
    char address[MAX_ADDRESS_LENGTH];
    char name[64];
    
    printf("\n--- REGISTER NEW MEMBER ---\n");
    get_input("Enter member address (wallet ID): ", address, sizeof(address));
    get_input("Enter member name: ", name, sizeof(name));
    
    if (strlen(address) > 0 && strlen(name) > 0) {
        register_member(address, name);
        printf("Member registered successfully!\n");
    } else {
        printf("Invalid input. Registration failed.\n");
    }
}

/**
 * handle_enroll_policy - Handle policy enrollment
 */
static void handle_enroll_policy(void)
{
    char member_id[MAX_ADDRESS_LENGTH];
    char plan[64];
    double premium;
    
    printf("\n--- ENROLL INSURANCE POLICY ---\n");
    get_input("Enter member address: ", member_id, sizeof(member_id));
    get_input("Enter coverage plan (e.g., 'Basic', 'Standard', 'Premium'): ", plan, sizeof(plan));
    premium = get_double_input("Enter premium amount (AHT): ");
    
    if (strlen(member_id) > 0 && strlen(plan) > 0 && premium > 0) {
        enroll_policy(member_id, plan, premium);
    } else {
        printf("Invalid input. Policy enrollment failed.\n");
    }
}

/**
 * handle_pay_premium - Handle premium payment
 */
static void handle_pay_premium(void)
{
    char member_id[MAX_ADDRESS_LENGTH];
    char policy_id[64];
    double amount;
    
    printf("\n--- PAY PREMIUM ---\n");
    get_input("Enter member address: ", member_id, sizeof(member_id));
    get_input("Enter policy ID: ", policy_id, sizeof(policy_id));
    amount = get_double_input("Enter premium amount (AHT): ");
    
    if (strlen(member_id) > 0 && strlen(policy_id) > 0 && amount > 0) {
        pay_premium(member_id, policy_id, amount);
    } else {
        printf("Invalid input. Premium payment failed.\n");
    }
}

/**
 * handle_submit_claim - Handle claim submission
 */
static void handle_submit_claim(void)
{
    char policy_id[64];
    char provider_id[MAX_ADDRESS_LENGTH];
    double amount;
    
    printf("\n--- SUBMIT CLAIM ---\n");
    get_input("Enter policy ID: ", policy_id, sizeof(policy_id));
    get_input("Enter provider address: ", provider_id, sizeof(provider_id));
    amount = get_double_input("Enter claim amount (AHT): ");
    
    if (strlen(policy_id) > 0 && strlen(provider_id) > 0 && amount > 0) {
        int result = submit_claim(policy_id, provider_id, amount);
        if (result == 1) {
            printf("Claim flagged for fraud review.\n");
        } else if (result == 0) {
            printf("Claim submitted successfully.\n");
        }
    } else {
        printf("Invalid input. Claim submission failed.\n");
    }
}

/**
 * handle_mine_block - Handle mining
 */
static void handle_mine_block(void)
{
    char miner_id[MAX_ADDRESS_LENGTH];
    
    printf("\n--- MINE BLOCK ---\n");
    get_input("Enter miner address: ", miner_id, sizeof(miner_id));
    
    if (strlen(miner_id) > 0) {
        mempool_t *mempool = get_global_mempool();
        if (!mempool) {
            initialize_mempool();
            mempool = get_global_mempool();
        }
        mine_solo(mempool, miner_id);
    } else {
        printf("Invalid miner address.\n");
    }
}

/**
 * handle_fraud_review - Handle fraud review
 */
static void handle_fraud_review(void)
{
    char tx_id[64];
    int choice;
    
    printf("\n--- FRAUD REVIEW ---\n");
    fraud_review();
    
    printf("\nOptions:\n");
    printf("1) Approve suspicious transaction\n");
    printf("2) Reject suspicious transaction\n");
    printf("3) Back to main menu\n");
    choice = get_int_input("Enter choice: ");
    
    if (choice == 1) {
        get_input("Enter transaction ID to approve: ", tx_id, sizeof(tx_id));
        if (strlen(tx_id) > 0) {
            approve_suspicious(tx_id);
        }
    } else if (choice == 2) {
        get_input("Enter transaction ID to reject: ", tx_id, sizeof(tx_id));
        if (strlen(tx_id) > 0) {
            reject_suspicious(tx_id);
        }
    }
}

/**
 * handle_balances - Display balances
 */
static void handle_balances(void)
{
    char address[MAX_ADDRESS_LENGTH];
    
    printf("\n--- BALANCES ---\n");
    printf("Reinsurance Pool Balance: %.2f AHT\n", get_account_balance("REINSURANCE_POOL"));
    printf("Insurance Pool Balance: %.2f AHT\n", get_account_balance("INSURANCE_POOL"));
    printf("Miner Pool Balance: %.2f AHT\n", get_account_balance("MINER_POOL"));
    
    get_input("Enter address to check balance (or press Enter to skip): ", address, sizeof(address));
    if (strlen(address) > 0) {
        printf("Balance for %s: %.2f AHT\n", address, get_account_balance(address));
        printf("Nonce: %lu\n", (unsigned long)get_account_nonce(address));
    }
}

/**
 * handle_renew_policy - Handle policy renewal
 */
static void handle_renew_policy(void)
{
    char policy_id[64];
    
    printf("\n--- RENEW POLICY ---\n");
    get_input("Enter policy ID to renew: ", policy_id, sizeof(policy_id));
    
    if (strlen(policy_id) > 0) {
        renew_policy(policy_id);
    } else {
        printf("Invalid policy ID.\n");
    }
}

/**
 * handle_view_policy - Handle viewing policy status
 */
static void handle_view_policy(void)
{
    char policy_id[64];
    
    printf("\n--- VIEW POLICY STATUS ---\n");
    get_input("Enter policy ID: ", policy_id, sizeof(policy_id));
    
    if (strlen(policy_id) > 0) {
        show_policy_status(policy_id);
    } else {
        printf("Invalid policy ID.\n");
    }
}

/**
 * handle_approve_claim - Handle claim approval
 */
static void handle_approve_claim(void)
{
    char claim_id[64];
    double amount;
    
    printf("\n--- APPROVE CLAIM ---\n");
    get_input("Enter claim ID: ", claim_id, sizeof(claim_id));
    amount = get_double_input("Enter approved amount (AHT): ");
    
    if (strlen(claim_id) > 0 && amount > 0) {
        approve_claim(claim_id, amount);
    } else {
        printf("Invalid input.\n");
    }
}

/**
 * handle_reject_claim - Handle claim rejection
 */
static void handle_reject_claim(void)
{
    char claim_id[64];
    char reason[256];
    
    printf("\n--- REJECT CLAIM ---\n");
    get_input("Enter claim ID: ", claim_id, sizeof(claim_id));
    get_input("Enter rejection reason: ", reason, sizeof(reason));
    
    if (strlen(claim_id) > 0 && strlen(reason) > 0) {
        reject_claim(claim_id, reason);
    } else {
        printf("Invalid input.\n");
    }
}

/**
 * handle_settle_claim - Handle claim settlement
 */
static void handle_settle_claim(void)
{
    char claim_id[64];
    double amount;
    
    printf("\n--- SETTLE CLAIM ---\n");
    get_input("Enter claim ID: ", claim_id, sizeof(claim_id));
    amount = get_double_input("Enter settlement amount (AHT): ");
    
    if (strlen(claim_id) > 0 && amount > 0) {
        settle_claim(claim_id, amount);
    } else {
        printf("Invalid input.\n");
    }
}

/**
 * run_cli - Main Dashboard Entry Loop
 */
void run_cli(void)
{
    int choice;
    char dummy_buf[MAX_INPUT];
    
    while (1) {
        print_header();
        print_menu();
        choice = get_int_input("Choose an operation (0-15): ");
        
        switch (choice) {
            case 0:
                printf("\nSaving state... Goodbye!\n");
                save_blockchain();
                save_chain_state(get_chain_state());
                return;
            
            case 1:
                handle_register_member();
                break;
            
            case 2:
                handle_enroll_policy();
                break;
            
            case 3:
                handle_pay_premium();
                break;
            
            case 4:
                handle_submit_claim();
                break;
            
            case 5:
                display_mempool(get_global_mempool());
                break;
            
            case 6:
                handle_mine_block();
                break;
            
            case 7:
                display_blockchain();
                break;
            
            case 8:
                verify_blockchain();
                break;
            
            case 9:
                handle_fraud_review();
                break;
            
            case 10:
                handle_balances();
                break;
            
            case 11:
                handle_renew_policy();
                break;
            
            case 12:
                handle_view_policy();
                break;
            
            case 13:
                handle_approve_claim();
                break;
            
            case 14:
                handle_reject_claim();
                break;
            
            case 15:
                handle_settle_claim();
                break;
            
            default:
                printf("Invalid choice. Please enter a number between 0 and 15.\n");
                break;
        }
        
        /* Deterministic pause mechanism - avoids stdin line clipping */
        get_input("\nPress Enter to continue...", dummy_buf, sizeof(dummy_buf));
    }
}
