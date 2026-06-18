#ifndef INSURANCE_H
#define INSURANCE_H

#include <stdint.h>
#include <time.h>

#define MAX_POLICIES 1000
#define MAX_CLAIMS 1000

/* Function prototypes */
int enroll_policy(const char *member_id, const char *coverage_plan, 
                  double premium_amount);
int pay_premium(const char *member_id, const char *policy_id, double amount);
int submit_claim(const char *policy_id, const char *provider_id, double amount);
int approve_claim(const char *claim_id, double amount);
int reject_claim(const char *claim_id, const char *reason);
int settle_claim(const char *claim_id, double approved_amount);
int renew_policy(const char *policy_id);
void show_policy_status(const char *policy_id);
void register_member(const char *address, const char *name);
void *find_policy(const char *policy_id);
void *find_claim(const char *claim_id);

#endif /* INSURANCE_H */
