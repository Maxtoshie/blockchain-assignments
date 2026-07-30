import { useState } from "react";
import Navbar from "../components/Navbar";
import CredentialSeal from "../components/CredentialSeal";
import LoadingSpinner from "../components/LoadingSpinner";
import { useWeb3 } from "../hooks/useWeb3";
import { formatTimestamp, shortenAddress } from "../utils/format";
import { CONTRACT_ADDRESS, CONTRACT_ABI } from "../utils/contractConfig";
import { JsonRpcProvider, Contract } from "ethers";

// Verification is read-only and should work even for a visitor with no
// wallet connected. If no wallet is available, fall back to a public RPC.
const PUBLIC_SEPOLIA_RPC = "https://ethereum-sepolia-rpc.publicnode.com";

export default function EmployerVerifyPage() {
  const { readOnlyContract } = useWeb3();
  const [credentialId, setCredentialId] = useState("");
  const [result, setResult] = useState(null);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);

  async function handleVerify(event) {
    event.preventDefault();
    setError(null);
    setResult(null);

    if (!credentialId.trim()) {
      setError("Please enter a credential ID.");
      return;
    }

    setIsLoading(true);
    try {
      const contract =
        readOnlyContract || new Contract(CONTRACT_ADDRESS, CONTRACT_ABI, new JsonRpcProvider(PUBLIC_SEPOLIA_RPC));

      const data = await contract.verifyCredential(credentialId.trim());
      setResult({
        student: data.student,
        studentName: data.studentName,
        institution: data.institution,
        institutionName: data.institutionName,
        degreeOrCertification: data.degreeOrCertification,
        course: data.course,
        dateIssued: data.dateIssued,
        metadataURI: data.metadataURI,
        revoked: data.revoked,
        institutionCurrentlyRegistered: data.institutionCurrentlyRegistered
      });
    } catch (err) {
      setError("No credential found with that ID, or it doesn't exist yet.");
    } finally {
      setIsLoading(false);
    }
  }

  return (
    <div>
      <Navbar />
      <section className="hero" style={{ paddingBottom: 20 }}>
        <p className="eyebrow hero-eyebrow">Employer verification</p>
        <h1 style={{ fontSize: "2.4rem" }}>Check a credential in seconds.</h1>
        <p>No account or wallet required. Just enter the credential ID.</p>
      </section>

      <section className="section" style={{ paddingTop: 0, maxWidth: 640 }}>
        <form onSubmit={handleVerify} className="card">
          <div className="field field-mono">
            <label htmlFor="credentialId">Credential ID</label>
            <input
              id="credentialId"
              type="number"
              min="1"
              placeholder="e.g. 1"
              value={credentialId}
              onChange={(e) => setCredentialId(e.target.value)}
            />
          </div>
          <button type="submit" className="btn btn-primary" disabled={isLoading}>
            {isLoading ? "Checking..." : "Verify Credential"}
          </button>
        </form>

        {isLoading && <LoadingSpinner label="Querying the blockchain..." />}

        {error && (
          <div className="tx-banner tx-error" style={{ marginTop: 20 }}>
            {error}
          </div>
        )}

        {result && (
          <div className="card" style={{ marginTop: 20, display: "flex", gap: 20 }}>
            <CredentialSeal institutionName={result.institutionName} revoked={result.revoked} />
            <div>
              <span className={`status-pill ${result.revoked ? "status-revoked" : "status-valid"}`}>
                {result.revoked ? "Revoked" : "Valid Credential"}
              </span>
              <h3 style={{ marginTop: 10, fontSize: "1.2rem" }}>{result.degreeOrCertification}</h3>
              <p style={{ color: "var(--charcoal-muted)", margin: "6px 0" }}>{result.course}</p>
              <dl style={{ marginTop: 12, fontSize: "0.9rem", color: "var(--charcoal-muted)" }}>
                <p><strong>Student:</strong> {result.studentName} ({shortenAddress(result.student)})</p>
                <p><strong>Issuing institution:</strong> {result.institutionName} ({shortenAddress(result.institution)})</p>
                <p><strong>Date issued:</strong> {formatTimestamp(result.dateIssued)}</p>
                <p>
                  <strong>Institution status:</strong>{" "}
                  {result.institutionCurrentlyRegistered ? "Currently accredited" : "No longer accredited"}
                </p>
              </dl>
            </div>
          </div>
        )}
      </section>
    </div>
  );
}
