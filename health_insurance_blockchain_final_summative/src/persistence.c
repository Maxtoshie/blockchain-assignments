#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "persistence.h"
#include "blockchain.h"
#include "mempool.h"
#include "account.h"

#define SAVE_FILE "data/blockchain.bin"
#define STATE_FILE "data/chain_state.bin"

/**
 * save_blockchain - Save blockchain to default path
 */
int save_blockchain(void)
{
    return save_blockchain_to_file(SAVE_FILE);
}

/**
 * save_blockchain_to_file - General function to save blockchain to custom filepath
 */
int save_blockchain_to_file(const char *filename)
{
    if (!filename) return -1;

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Failed to open save file: %s\n", filename);
        return -1;
    }

    block_t *current = get_chain_head();
    uint32_t count = 0;

    while (current) {
        fwrite(&current->block_id, sizeof(uint32_t), 1, fp);
        fwrite(&current->timestamp, sizeof(time_t), 1, fp);
        fwrite(&current->transaction_count, sizeof(uint32_t), 1, fp);
        fwrite(current->previous_hash, SHA256_DIGEST_LENGTH, 1, fp);
        fwrite(current->merkle_root, SHA256_DIGEST_LENGTH, 1, fp);
        fwrite(current->hash, SHA256_DIGEST_LENGTH, 1, fp);
        fwrite(&current->nonce, sizeof(uint64_t), 1, fp);
        fwrite(current->miner_id, sizeof(current->miner_id), 1, fp);
        fwrite(&current->difficulty, sizeof(uint32_t), 1, fp);

        for (uint32_t i = 0; i < current->transaction_count; i++) {
            transaction_t *tx = current->transactions[i];

            fwrite(tx->transaction_id, sizeof(tx->transaction_id), 1, fp);
            fwrite(tx->sender_address, sizeof(tx->sender_address), 1, fp);
            fwrite(tx->receiver_address, sizeof(tx->receiver_address), 1, fp);
            fwrite(&tx->amount, sizeof(double), 1, fp);
            fwrite(&tx->type, sizeof(transaction_type_t), 1, fp);
            fwrite(&tx->timestamp, sizeof(time_t), 1, fp);
            fwrite(&tx->sender_nonce, sizeof(uint64_t), 1, fp);
            fwrite(&tx->signature_len, sizeof(int), 1, fp);

            if (tx->signature_len > 0)
                fwrite(tx->digital_signature, tx->signature_len, 1, fp);

            fwrite(tx->metadata, sizeof(tx->metadata), 1, fp);
        }

        count++;
        current = current->next;
    }

    fclose(fp);
    printf("Saved %u blocks to %s\n", count, filename);
    return 0;
}

/**
 * load_blockchain - Load blockchain from default path
 */
int load_blockchain(void)
{
    return load_blockchain_from_file(SAVE_FILE);
}

/**
 * load_blockchain_from_file - General function to load blockchain from custom filepath
 */
int load_blockchain_from_file(const char *filename)
{
    if (!filename) return -1;

    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return -1;

    block_t *block = NULL;
    block_t *prev = NULL;
    int read_err = 0;

    while (1) {
        uint32_t block_id;

        if (fread(&block_id, sizeof(uint32_t), 1, fp) != 1)
            break;

        block = calloc(1, sizeof(block_t));
        if (!block) {
            fclose(fp);
            return -1;
        }

        block->block_id = block_id;

        if (fread(&block->timestamp, sizeof(time_t), 1, fp) != 1 ||
            fread(&block->transaction_count, sizeof(uint32_t), 1, fp) != 1 ||
            fread(block->previous_hash, SHA256_DIGEST_LENGTH, 1, fp) != 1 ||
            fread(block->merkle_root, SHA256_DIGEST_LENGTH, 1, fp) != 1 ||
            fread(block->hash, SHA256_DIGEST_LENGTH, 1, fp) != 1 ||
            fread(&block->nonce, sizeof(uint64_t), 1, fp) != 1 ||
            fread(block->miner_id, sizeof(block->miner_id), 1, fp) != 1 ||
            fread(&block->difficulty, sizeof(uint32_t), 1, fp) != 1) {
            read_err = 1;
            free(block);
            break;
        }

        for (uint32_t i = 0; i < block->transaction_count; i++) {
            transaction_t *tx = calloc(1, sizeof(transaction_t));
            if (!tx) {
                read_err = 1;
                break;
            }

            if (fread(tx->transaction_id, sizeof(tx->transaction_id), 1, fp) != 1 ||
                fread(tx->sender_address, sizeof(tx->sender_address), 1, fp) != 1 ||
                fread(tx->receiver_address, sizeof(tx->receiver_address), 1, fp) != 1 ||
                fread(&tx->amount, sizeof(double), 1, fp) != 1 ||
                fread(&tx->type, sizeof(transaction_type_t), 1, fp) != 1 ||
                fread(&tx->timestamp, sizeof(time_t), 1, fp) != 1 ||
                fread(&tx->sender_nonce, sizeof(uint64_t), 1, fp) != 1 ||
                fread(&tx->signature_len, sizeof(int), 1, fp) != 1) {
                read_err = 1;
                free(tx);
                break;
            }

            if (tx->signature_len > 0) {
                if (fread(tx->digital_signature, tx->signature_len, 1, fp) != 1) {
                    read_err = 1;
                    free(tx);
                    break;
                }
            }

            if (fread(tx->metadata, sizeof(tx->metadata), 1, fp) != 1) {
                read_err = 1;
                free(tx);
                break;
            }

            block->transactions[i] = tx;
        }

        if (read_err) {
            break;
        }

        block->next = NULL;

        if (prev)
            prev->next = block;

        prev = block;
    }

    fclose(fp);

    if (read_err) {
        printf("Error: Data corruption detected while reading blockchain binary file.\n");
        return -1;
    }

    printf("Blockchain loaded successfully from %s\n", filename);
    return 0;
}

/**
 * save_chain_state - Save chain state
 */
int save_chain_state(chain_state_t *state)
{
    if (!state) return -1;

    FILE *fp = fopen(STATE_FILE, "wb");
    if (!fp)
        return -1;

    fwrite(&state->difficulty, sizeof(uint32_t), 1, fp);
    fwrite(&state->block_reward, sizeof(double), 1, fp);
    fwrite(&state->last_retarget_block, sizeof(uint32_t), 1, fp);
    fwrite(&state->total_blocks, sizeof(uint32_t), 1, fp);
    fwrite(&state->genesis_block_id, sizeof(uint32_t), 1, fp);

    fclose(fp);
    return 0;
}

/**
 * load_chain_state - Load chain state
 */
int load_chain_state(void)
{
    FILE *fp = fopen(STATE_FILE, "rb");
    if (!fp)
        return -1;

    chain_state_t state;

    if (fread(&state.difficulty, sizeof(uint32_t), 1, fp) != 1 ||
        fread(&state.block_reward, sizeof(double), 1, fp) != 1 ||
        fread(&state.last_retarget_block, sizeof(uint32_t), 1, fp) != 1 ||
        fread(&state.total_blocks, sizeof(uint32_t), 1, fp) != 1 ||
        fread(&state.genesis_block_id, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    fclose(fp);

    set_chain_state(&state);
    return 0;
}
