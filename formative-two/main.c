#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int transaction_model = 1;

void print_menu(void) {
    printf("\n========================================\n");
    printf("BLOCKCHAIN ATTENDANCE SYSTEM\n");
    printf("========================================\n");
    printf("1. Mark Attendance (add to pending)\n");
    printf("2. View Pending Pool\n");
    printf("3. View Blockchain\n");
    printf("4. Solo Mining\n");
    printf("5. Pool Mining\n");
    printf("6. Cloud Mining\n");
    printf("7. View Balances\n");
    printf("8. View UTXO Set (if UTXO model)\n");
    printf("9. View Transaction History (if Account model)\n");
    printf("10. Transfer Tokens\n");
    printf("11. Switch Transaction Model\n");
    printf("12. Set Mining Difficulty\n");
    printf("0. Exit\n");
    printf("========================================\n");
    printf("Choice: ");
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void mark_attendance(void) {
    char id[10];
    char course[MAX_COURSE];
    int status_choice;
    int result;
    
    printf("\n--- Mark Attendance ---\n");
    printf("Student ID (S001-S005): ");
    result = scanf("%9s", id);
    if (result != 1) {
        printf("Invalid input\n");
        clear_input_buffer();
        return;
    }
    
    Student *student = find_student(id);
    if (!student) {
        printf("Student not found!\n");
        return;
    }
    
    printf("Course: ");
    result = scanf("%49s", course);
    if (result != 1) {
        printf("Invalid input\n");
        clear_input_buffer();
        return;
    }
    
    printf("Status (1=PRESENT, 2=LATE, 3=ABSENT): ");
    result = scanf("%d", &status_choice);
    if (result != 1) {
        printf("Invalid input\n");
        clear_input_buffer();
        return;
    }
    
    AttendanceStatus status;
    if (status_choice == 1) status = PRESENT;
    else if (status_choice == 2) status = LATE;
    else status = ABSENT;
    
    add_pending_attendance(id, student->name, course, status);
}

void view_balances(void) {
    printf("\n=== STUDENT BALANCES ===\n");
    if (transaction_model == 1) {
        print_utxo_set();
    } else {
        Student *student = student_registry;
        while (student) {
            print_account_balance(student->id);
            student = student->next;
        }
    }
}

void transfer_tokens(void) {
    char from[10], to[10];
    int amount;
    int result;
    
    printf("\n--- Transfer Tokens ---\n");
    printf("From Student ID: ");
    result = scanf("%9s", from);
    if (result != 1) {
        printf("Invalid input\n");
        clear_input_buffer();
        return;
    }
    
    printf("To Student ID: ");
    result = scanf("%9s", to);
    if (result != 1) {
        printf("Invalid input\n");
        clear_input_buffer();
        return;
    }
    
    printf("Amount: ");
    result = scanf("%d", &amount);
    if (result != 1) {
        printf("Invalid input\n");
        clear_input_buffer();
        return;
    }
    
    if (transaction_model == 1) {
        create_transaction_utxo(from, to, amount);
    } else {
        create_transaction_account(from, to, amount);
    }
}

void switch_model(void) {
    int choice;
    int result;
    
    printf("\n--- Switch Transaction Model ---\n");
    printf("Current model: %s\n", transaction_model == 1 ? "UTXO" : "Account-Based");
    printf("Select model (1=UTXO, 2=Account-Based): ");
    result = scanf("%d", &choice);
    if (result != 1) {
        printf("Invalid input\n");
        clear_input_buffer();
        return;
    }
    
    if (choice == 1 || choice == 2) {
        transaction_model = choice;
        if (transaction_model == 1) {
            init_utxo_model();
        } else {
            init_account_model();
        }
        printf("Switched to %s model\n", transaction_model == 1 ? "UTXO" : "Account-Based");
    } else {
        printf("Invalid choice\n");
    }
}

int main(void) {
    int choice, difficulty = DIFFICULTY;
    int num_miners, rental_duration;
    int result;
    
    printf("========================================\n");
    printf("BLOCKCHAIN ATTENDANCE SYSTEM v2.0\n");
    printf("With Transaction Models & Mining\n");
    printf("========================================\n");
    
    init_student_registry();
    init_utxo_model();
    
    while (1) {
        print_menu();
        result = scanf("%d", &choice);
        if (result != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        
        switch (choice) {
            case 1:
                mark_attendance();
                break;
            case 2:
                print_pending_pool();
                break;
            case 3:
                print_blockchain();
                break;
            case 4:
                solo_mining(difficulty);
                break;
            case 5:
                printf("Number of miners: ");
                result = scanf("%d", &num_miners);
                if (result != 1) {
                    printf("Invalid input\n");
                    clear_input_buffer();
                    break;
                }
                pool_mining(num_miners, difficulty);
                break;
            case 6:
                printf("Rental duration (1-5 rounds): ");
                result = scanf("%d", &rental_duration);
                if (result != 1) {
                    printf("Invalid input\n");
                    clear_input_buffer();
                    break;
                }
                if (rental_duration < 1) rental_duration = 1;
                if (rental_duration > 5) rental_duration = 5;
                cloud_mining(rental_duration, difficulty);
                break;
            case 7:
                view_balances();
                break;
            case 8:
                if (transaction_model == 1)
                    print_utxo_set();
                else
                    printf("UTXO model not active. Switch to UTXO model first.\n");
                break;
            case 9:
                if (transaction_model == 2) {
                    char id[10];
                    printf("Student ID: ");
                    result = scanf("%9s", id);
                    if (result != 1) {
                        printf("Invalid input\n");
                        clear_input_buffer();
                        break;
                    }
                    print_transaction_history(id);
                } else {
                    printf("Account model not active. Switch to Account model first.\n");
                }
                break;
            case 10:
                transfer_tokens();
                break;
            case 11:
                switch_model();
                break;
            case 12:
                printf("Enter difficulty (1-4): ");
                result = scanf("%d", &difficulty);
                if (result != 1) {
                    printf("Invalid input\n");
                    clear_input_buffer();
                    break;
                }
                if (difficulty < 1) difficulty = 1;
                if (difficulty > 4) difficulty = 4;
                printf("Mining difficulty set to %d\n", difficulty);
                break;
            case 0:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }
    
    return 0;
}
