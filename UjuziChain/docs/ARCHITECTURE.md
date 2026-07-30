# Architecture Overview

## System diagram

```
┌─────────────────┐        ┌──────────────────────┐        ┌────────────────────┐
│   Admin wallet   │──────▶│                      │        │                    │
└─────────────────┘  reg.  │                      │        │                    │
                     inst.  │                      │        │                    │
┌─────────────────┐        │    UjuziChain.sol     │◀──────▶│  React Frontend    │
│Institution wallet│──────▶│  (Sepolia Testnet)    │  reads  │  (ethers.js v6)    │
└─────────────────┘  issue/│                      │  writes │                    │
                     revoke │                      │        │                    │
┌─────────────────┐        │                      │        └────────────────────┘
│  Student wallet   │◀─────│                      │                 ▲
└─────────────────┘  owns  └──────────────────────┘                 │
                                     ▲                                │
                                     │  free, no-wallet reads         │
                              ┌─────────────────┐                     │
                              │ Employer (public)│─────────────────────┘
                              └─────────────────┘
```

## On-chain layer

`UjuziChain.sol` is a single contract holding two core mappings:

- `institutions: address => Institution` — tracks which wallets are
  accredited to issue credentials.
- `credentials: uint256 => Credential` — the immutable record of every
  credential ever issued, keyed by an auto-incrementing ID.

Access control is intentionally simple and explicit:

- **Admin** = contract owner (OpenZeppelin `Ownable`) — manages institutions.
- **Institution** = any address in `institutions` with `isRegistered == true`
  — can issue/revoke credentials, but only ones it issued itself.
- **Everyone else** (students, employers) has full read access via `view`
  functions, and no write access at all.

## Off-chain layer (frontend)

The React app is organized around a single `Web3Context` that holds:

- The connected wallet's `provider`, `signer`, and `account`
- A **write-capable contract instance** (`contract`, uses `signer`)
- A **read-only contract instance** (`readOnlyContract`, uses `provider` or
  a public Sepolia RPC as fallback) — this is what powers the Verify page
  for visitors who have no wallet connected at all

`useRole()` determines whether the connected account is `admin`,
`institution`, or a general user, purely by querying the contract
(`owner()` and `isInstitutionRegistered()`) — there's no off-chain role
database to keep in sync.

## Data flow: issuing a credential

1. Institution fills out the **Issue Credential** form.
2. Frontend calls `contract.issueCredential(...)` using the institution's signer.
3. MetaMask prompts for confirmation; the transaction is sent to Sepolia.
4. Contract validates the caller is a registered institution, assigns the
   next sequential ID, stores the credential, and emits `CredentialIssued`.
5. Frontend awaits the transaction receipt and shows a success banner.

## Data flow: verifying a credential

1. Employer enters a credential ID on the public **Verify** page — no
   wallet connection needed.
2. Frontend calls `verifyCredential(id)`, a `view` function, via a public
   RPC endpoint if no wallet is connected.
3. Contract returns the full credential record plus whether the issuing
   institution is *currently* accredited — a revoked credential and a
   currently-unaccredited institution are tracked as two independent
   pieces of state, since removing an institution doesn't retroactively
   invalidate what it already issued.

## Why this design

- **No backend server, no database.** The Ethereum network Sepolia *is*
  the database. This eliminates an entire class of "is the API in sync
  with the chain" bugs.
- **Read access is free and universal.** Verification — the whole point
  of the product for employers — costs nothing and requires no account,
  by using `view` functions and a public RPC fallback.
- **Institution snapshot on issuance.** Storing `institutionName` on the
  credential itself (rather than looking it up live) means a credential's
  displayed details never change even if an institution is later renamed
  or removed.
