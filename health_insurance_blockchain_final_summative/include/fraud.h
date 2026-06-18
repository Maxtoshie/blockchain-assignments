#ifndef FRAUD_H
#define FRAUD_H

#include "transaction.h"

int detect_fraud(const char *member_id, const char *provider_id, 
                 double amount, const char *claim_id);
void fraud_review(void);
int approve_suspicious(const char *tx_id);
int reject_suspicious(const char *tx_id);

#endif /* FRAUD_H */
