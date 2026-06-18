#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "blockchain.h"
#include "persistence.h"
#include "cli.h"
#include "mempool.h"

static void signal_handler(int sig)
{
    (void)sig;  /* Suppress unused parameter warning */
    printf("\nSaving state and exiting...\n");
    save_blockchain();
    save_chain_state(get_chain_state());
    exit(0);
}

int main(void)
{
    /* Set up signal handler */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    /* Initialize blockchain */
    initialize_blockchain();
    
    /* Initialize mempool */
    initialize_mempool();
    
    /* Load saved state */
    if (load_chain_state() == 0) {
        printf("Loaded saved blockchain state\n");
        load_blockchain();
    } else {
        printf("No saved state found. Creating genesis block...\n");
        create_genesis_block();
        save_blockchain();
        save_chain_state(get_chain_state());
    }
    
    /* Run CLI */
    run_cli();
    
    return 0;
}
