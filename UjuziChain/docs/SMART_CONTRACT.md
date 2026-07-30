# Smart Contract Explanation

Full source: [`contracts/UjuziChain.sol`](../contracts/UjuziChain.sol)

## Inheritance

```solidity
contract UjuziChain is Ownable
```

Uses OpenZeppelin's audited `Ownable` for admin access control rather than
hand-rolling it — one less place for a security bug to hide.

## Data structures

```solidity
struct Institution {
    string name;
    bool isRegistered;
    uint256 registeredAt;
}

struct Credential {
    uint256 id;
    address student;
    string studentName;
    address institution;
    string institutionName;
    string degreeOrCertification;
    string course;
    uint256 dateIssued;
    string metadataURI;
    bool revoked;
}
```

`institutionName` is stored on the `Credential` itself (a snapshot at
issuance time) rather than looked up live from the `institutions` mapping.
This means a credential's displayed institution name never silently
changes, even if the institution is renamed or later removed.

## Functions

### Institution management (admin only)

| Function | Access | Purpose |
|---|---|---|
| `registerInstitution(address, string)` | `onlyOwner` | Accredit a new institution |
| `removeInstitution(address)` | `onlyOwner` | Revoke an institution's ability to issue *new* credentials |

### Credential management (registered institution only)

| Function | Access | Purpose |
|---|---|---|
| `issueCredential(...)` | `onlyRegisteredInstitution` | Mint a new credential to a student |
| `revokeCredential(uint256)` | issuing institution only | Flag a credential as revoked |
| `updateMetadataURI(uint256, string)` | issuing institution only | Update supporting-document link |

### Verification (public, free)

| Function | Access | Purpose |
|---|---|---|
| `verifyCredential(uint256)` | anyone (`view`) | Full credential detail + live institution status |
| `getCredentialsByStudent(address)` | anyone (`view`) | List all credential IDs owned by a student |
| `isInstitutionRegistered(address)` | anyone (`view`) | Quick accreditation check |
| `totalCredentialsIssued()` | anyone (`view`) | Running count, including revoked ones |

## Events

Every state-changing action emits an event, giving the frontend (and any
block explorer) a full audit trail without needing to poll storage:

- `InstitutionRegistered`, `InstitutionRemoved`
- `CredentialIssued`, `CredentialRevoked`, `CredentialMetadataUpdated`

## Security considerations

- **Access control** is enforced with `modifier`s (`onlyRegisteredInstitution`,
  `credentialExists`) checked at the top of every state-changing function —
  checks-effects-interactions is followed throughout (no external calls at all,
  in fact, since this contract has no token transfers or external dependencies).
- **No credential deletion.** Revocation is a boolean flag, not a `delete` —
  preserving a permanent audit trail is a deliberate design choice for a
  credentialing system, not an oversight.
- **Zero-address checks** guard both institution registration and credential
  issuance.
- **Institution removal doesn't retroactively revoke** credentials it
  already issued — this is intentional (a credential was valid when
  earned) and is reflected as a *separate* field in `verifyCredential`'s
  return value (`institutionCurrentlyRegistered`), so the frontend can
  show both facts honestly instead of conflating them.

## Gas / design trade-offs worth noting in your report

- `require` with string messages was chosen over Solidity custom errors for
  clarity during grading/demoing, at a small gas cost.
- `metadataURI` supports an optional IPFS link for supporting documents
  (transcripts, certificates) without storing large data on-chain.
