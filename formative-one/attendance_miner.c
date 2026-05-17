#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ec.h> /* Fixed: Resolves implicit declaration of EC macros */

#define MAX_STUDENTS 100
#define SUCCESS 0
#define FAILURE -1

/**
 * struct Student - Represents student registry records
 * @student_id: Unique identification string for a student
 * @full_name: Student's complete legal name
 * @course_code: Code representing the specific classroom course
 */
typedef struct Student
{
	char student_id[20];
	char full_name[50];
	char course_code[10];
} Student;

/**
 * struct Block - Node representing an attendance entry in the immutable ledger
 * @index: Block positioning identification (Genesis = 0)
 * @timestamp: Epoch timeline signature when the block generated
 * @student_id: Linked unique identification string
 * @full_name: Student's name cached during block emission
 * @course_code: Course string targeted by attendance instance
 * @status: Evaluation string (PRESENT, ABSENT, or LATE)
 * @previous_hash: Standard hex-string map array linking parent ledger block
 * @signature: Digital ECDSA verification array tracking block state integrity
 * @sig_len: Tracked dynamic payload byte width of active signature array
 * @hash: Standard hex-string computing self structural footprint
 * @next: Memory location pointer sequencing the linked list network
 */
typedef struct Block
{
	int index;
	time_t timestamp;
	char student_id[20];
	char full_name[50];
	char course_code[10];
	char status[10];
	char previous_hash[65];
	unsigned char signature[72];
	unsigned int sig_len;
	char hash[65];
	struct Block *next;
} Block;

/* Global Storage Pointers */
Student student_registry[MAX_STUDENTS];
int registered_student_count = 0;
Block *blockchain_head = NULL;
Block *blockchain_tail = NULL;

/* Cryptographic Keypair Variables (Simulated System Authority Keys) */
EVP_PKEY *system_keypair = NULL;

/**
 * generate_system_keys - Prepares ECDSA system key pairs using OpenSSL EVP API
 * Return: SUCCESS on optimization, FAILURE upon cryptographic faults
 */
int generate_system_keys(void)
{
	EVP_PKEY_CTX *pctx = NULL;

	pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
	if (!pctx || EVP_PKEY_keygen_init(pctx) <= 0)
		return (FAILURE);

	if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1) <= 0)
	{
		EVP_PKEY_CTX_free(pctx);
		return (FAILURE);
	}

	if (EVP_PKEY_keygen(pctx, &system_keypair) <= 0)
	{
		EVP_PKEY_CTX_free(pctx);
		return (FAILURE);
	}

	EVP_PKEY_CTX_free(pctx);
	return (SUCCESS);
}

/**
 * calculate_block_hash - Formulates context values into SHA-256 via modern EVP
 * @b: Complete reference address locating target block properties
 * @output_hash: Pre-allocated string buffer holding structural hex output
 */
void calculate_block_hash(Block *b, char *output_hash)
{
	unsigned char raw_hash[32]; /* SHA256 output length */
	unsigned int md_len = 0;
	char input_buffer[512];
	EVP_MD_CTX *mdctx = NULL;
	unsigned int i;

	/* Serializing raw struct primitives into singular state string */
	sprintf(input_buffer, "%d%ld%s%s%s%s%s",
			b->index,
			(long)b->timestamp,
			b->student_id,
			b->full_name,
			b->course_code,
			b->status,
			b->previous_hash);

	/* Modern OpenSSL 3.x non-deprecated hashing routine */
	mdctx = EVP_MD_CTX_new();
	if (mdctx)
	{
		if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) > 0)
		{
			EVP_DigestUpdate(mdctx, input_buffer, strlen(input_buffer));
			EVP_DigestFinal_ex(mdctx, raw_hash, &md_len);
		}
		EVP_MD_CTX_free(mdctx);
	}

	for (i = 0; i < md_len; i++)
	{
		sprintf(&output_hash[i * 2], "%02x", raw_hash[i]);
	}
	output_hash[64] = '\0';
}

/**
 * sign_block_data - Generates ECDSA cryptographic signature on block hash digest
 * @b: Reference layout address targeted for security signing updates
 * Return: SUCCESS or FAILURE
 */
int sign_block_data(Block *b)
{
	EVP_MD_CTX *mctx = NULL;
	size_t sig_len_out = 72;

	mctx = EVP_MD_CTX_new();
	if (!mctx)
		return (FAILURE);

	if (EVP_DigestSignInit(mctx, NULL, EVP_sha256(), NULL, system_keypair) <= 0)
	{
		EVP_MD_CTX_free(mctx);
		return (FAILURE);
	}

	if (EVP_DigestSignUpdate(mctx, b->hash, strlen(b->hash)) <= 0)
	{
		EVP_MD_CTX_free(mctx);
		return (FAILURE);
	}

	if (EVP_DigestSignFinal(mctx, b->signature, &sig_len_out) <= 0)
	{
		EVP_MD_CTX_free(mctx);
		return (FAILURE);
	}

	b->sig_len = (unsigned int)sig_len_out;
	EVP_MD_CTX_free(mctx);
	return (SUCCESS);
}

/**
 * verify_block_signature - Cryptographically verifies block signature correctness
 * @b: Specific target evaluation node
 * Return: 1 if cryptographically valid, 0 if verification fails
 */
int verify_block_signature(Block *b)
{
	EVP_MD_CTX *mctx = NULL;
	int auth_result;

	mctx = EVP_MD_CTX_new();
	if (!mctx)
		return (0);

	if (EVP_DigestVerifyInit(mctx, NULL, EVP_sha256(), NULL, system_keypair) <= 0)
	{
		EVP_MD_CTX_free(mctx);
		return (0);
	}

	if (EVP_DigestVerifyUpdate(mctx, b->hash, strlen(b->hash)) <= 0)
	{
		EVP_MD_CTX_free(mctx);
		return (0);
	}

	auth_result = EVP_DigestVerifyFinal(mctx, b->signature, b->sig_len);
	EVP_MD_CTX_free(mctx);

	return (auth_result == 1 ? 1 : 0);
}

/**
 * load_student_registry - Reads target flatfile asset system database parsing IDs
 * Return: SUCCESS on completion context execution loop, FAILURE upon system faults
 */
int load_student_registry(void)
{
	FILE *file_ptr;
	char row_line[128];

	file_ptr = fopen("students.txt", "r");
	if (!file_ptr)
	{
		printf("ERROR: students.txt is missing or empty.\n");
		return (FAILURE);
	}

	while (fgets(row_line, sizeof(row_line), file_ptr) &&
		   registered_student_count < MAX_STUDENTS)
	{
		char *token;
		row_line[strcspn(row_line, "\r\n")] = 0; /* Clear newlines */

		token = strtok(row_line, ",");
		if (token)
			strcpy(student_registry[registered_student_count].student_id, token);

		token = strtok(NULL, ",");
		if (token)
			strcpy(student_registry[registered_student_count].full_name, token);

		token = strtok(NULL, ",");
		if (token)
			strcpy(student_registry[registered_student_count].course_code, token);

		registered_student_count++;
	}

	fclose(file_ptr);

	if (registered_student_count == 0)
	{
		printf("ERROR: students.txt is missing or empty.\n");
		return (FAILURE);
	}

	printf("SUCCESS: Loaded %d students into memory registry.\n", registered_student_count);
	return (SUCCESS);
}

/**
 * query_student_directory - Searches internal registry space matching given key
 * @id: String identifier payload mapping key targets
 * Return: Reference pointer locating validated memory slot, NULL otherwise
 */
Student *query_student_directory(const char *id)
{
	int i;

	for (i = 0; i < registered_student_count; i++)
	{
		if (strcmp(student_registry[i].student_id, id) == 0)
		{
			return (&student_registry[i]);
		}
	}
	return (NULL);
}

/**
 * append_genesis_block - Constructs base structure layer at node index 0
 */
void append_genesis_block(void)
{
	Block *genesis = (Block *)malloc(sizeof(Block));
	int i;

	if (!genesis)
		return;

	genesis->index = 0;
	genesis->timestamp = time(NULL);
	strcpy(genesis->student_id, "SYSTEM");
	strcpy(genesis->full_name, "Genesis Record Platform");
	strcpy(genesis->course_code, "NONE");
	strcpy(genesis->status, "INITIAL");

	for (i = 0; i < 64; i++)
		genesis->previous_hash[i] = '0';
	genesis->previous_hash[64] = '\0';

	calculate_block_hash(genesis, genesis->hash);
	sign_block_data(genesis);
	genesis->next = NULL;

	blockchain_head = genesis;
	blockchain_tail = genesis;
}

/**
 * mark_attendance - Business logical layer inserting evaluated blocks into the chain
 * @id: String matching registry
 * @status: String tracking matching constraints
 */
void mark_attendance(const char *id, const char *status)
{
	Student *match = query_student_directory(id);
	Block *new_block = NULL;

	if (!match)
	{
		printf("ERROR: Student ID not found\n");
		return;
	}

	new_block = (Block *)malloc(sizeof(Block));
	if (!new_block)
		return;

	new_block->index = blockchain_tail->index + 1;
	new_block->timestamp = time(NULL);
	strcpy(new_block->student_id, match->student_id);
	strcpy(new_block->full_name, match->full_name);
	strcpy(new_block->course_code, match->course_code);
	strcpy(new_block->status, status);
	strcpy(new_block->previous_hash, blockchain_tail->hash);

	calculate_block_hash(new_block, new_block->hash);
	sign_block_data(new_block);
	new_block->next = NULL;

	blockchain_tail->next = new_block;
	blockchain_tail = new_block;

	printf("SUCCESS: Attendance recorded securely for %s (Block #%d)\n", match->full_name, new_block->index);
}

/**
 * validate_blockchain_integrity - Iterates structures asserting linkages and crypts
 * Return: SUCCESS (0) when completely pure chain exists, FAILURE (-1) for tampered blocks
 */
int validate_blockchain_integrity(void)
{
	Block *current = blockchain_head;
	char verification_hash[65];

	while (current != NULL)
	{
		calculate_block_hash(current, verification_hash);
		if (strcmp(current->hash, verification_hash) != 0)
		{
			printf("TAMPER DETECTED: Recalculated hash mismatch at block index %d!\n", current->index);
			return (FAILURE);
		}

		if (current != blockchain_head)
		{
			Block *prev = blockchain_head;
			while (prev->next != current)
				prev = prev->next;

			if (strcmp(current->previous_hash, prev->hash) != 0)
			{
				printf("TAMPER DETECTED: Linkage broken. Block %d point back corruption!\n", current->index);
				return (FAILURE);
			}
		}

		if (!verify_block_signature(current))
		{
			printf("TAMPER DETECTED: Cryptographic key verification failed at block index %d!\n", current->index);
			return (FAILURE);
		}

		current = current->next;
	}
	return (SUCCESS);
}

/**
 * print_attendance_records - Renders all processed ledger rows to standard stdout
 */
void print_attendance_records(void)
{
	Block *curr = blockchain_head;
	char time_buffer[26];

	printf("\n==================== BLOCKED ATTENDANCE RECORDS LAYER ====================\n");
	while (curr != NULL)
	{
		strcpy(time_buffer, ctime(&curr->timestamp));
		time_buffer[24] = '\0'; /* Truncate newline trailing standard conversions */

		printf("Block Index:   %d\n", curr->index);
		printf("Timestamp:     %s\n", time_buffer);
		printf("Student ID:    %s\n", curr->student_id);
		printf("Student Name:  %s\n", curr->full_name);
		printf("Course Code:   %s\n", curr->course_code);
		printf("Status Flag:   %s\n", curr->status);
		printf("Current Hash:  %s\n", curr->hash);
		printf("Previous Hash: %s\n", curr->previous_hash);
		printf("Crypt Signature Verified: %s\n", verify_block_signature(curr) ? "YES (VALID)" : "NO (CORRUPTED)");
		printf("--------------------------------------------------------------------------\n");
		curr = curr->next;
	}
}

/**
 * launch_tamper_simulation - Manually corrupts internal storage elements to demo safety
 */
void launch_tamper_simulation(void)
{
	if (blockchain_head == NULL || blockchain_head->next == NULL)
	{
		printf("ERROR: Add block records first before triggering tamper routines.\n");
		return;
	}
	/* Targeting Index 1 explicitly for structural manipulation */
	strcpy(blockchain_head->next->status, "ABSENT");
	printf("\nMALICIOUS ACTOR ACTION: Modifying Block #1 attendance record status to 'ABSENT'...\n");
}

/**
 * clear_allocated_memory - Frees global lists and OpenSSL objects before application shutdown
 */
void clear_allocated_memory(void)
{
	Block *current = blockchain_head;
	Block *next_node = NULL;

	while (current != NULL)
	{
		next_node = current->next;
		free(current);
		current = next_node;
	}
	if (system_keypair)
		EVP_PKEY_free(system_keypair);
}

/**
 * main - Central runtime routing loop for execution environment CLI interface
 * Return: 0 upon graceful process execution
 */
int main(void)
{
	int selection_item = 0;
	char user_id_in[20];
	char user_status_in[15];

	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	if (generate_system_keys() != SUCCESS)
	{
		printf("FATAL ERROR: Failed initializing OpenSSL Cryptographic Provider Frameworks.\n");
		return (1);
	}

	printf("--- INITIALIZING SYSTEM BLOCKCHAIN REGISTRATION LEDGER ---\n");
	if (load_student_registry() != SUCCESS)
	{
		printf("System Halt: Application cannot continue without loading a valid student directory.\n");
		if (system_keypair)
			EVP_PKEY_free(system_keypair);
		return (1);
	}

	append_genesis_block();

	while (1)
	{
		printf("\n=== ATTENDANCE TRACKER MANAGEMENT PANEL ===\n");
		printf("1. Mark Attendance Entry\n");
		printf("2. View Verified Ledger Chains\n");
		printf("3. Perform Cryptographic Integrity Audit\n");
		printf("4. Inject Malicious Block Exploits (Simulate Tampering)\n");
		printf("5. Shutdown Terminal Application\n");
		printf("Select Choice: ");

		if (scanf("%d", &selection_item) != 1)
		{
			printf("Invalid Input detected. Aborting execution.\n");
			break;
		}

		if (selection_item == 1)
		{
			printf("Enter student identification ID: ");
			scanf("%s", user_id_in);
			printf("Enter attendance status metric (PRESENT/ABSENT/LATE): ");
			scanf("%s", user_status_in);
			mark_attendance(user_id_in, user_status_in);
		}
		else if (selection_item == 2)
		{
			print_attendance_records();
		}
		else if (selection_item == 3)
		{
			if (validate_blockchain_integrity() == SUCCESS)
				printf("AUDIT REPORT SUCCESS: Blockchain integrity verified. Zero anomalies identified.\n");
		}
		else if (selection_item == 4)
		{
			launch_tamper_simulation();
		}
		else if (selection_item == 5)
		{
			printf("Cleaning internal security components. Terminating application run.\n");
			break;
		}
		else
		{
			printf("Option mapping unrecognized. Try again.\n");
		}
	}

	clear_allocated_memory();
	EVP_cleanup();
	return (0);
}
