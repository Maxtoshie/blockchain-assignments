import { useEffect, useState } from "react";
import { useParams } from "react-router-dom";
import Navbar from "../components/Navbar";
import CredentialSeal from "../components/CredentialSeal";
import LoadingSpinner from "../components/LoadingSpinner";
import { useWeb3 } from "../hooks/useWeb3";
import { formatTimestamp, shortenAddress } from "../utils/format";
import { CONTRACT_ADDRESS, CONTRACT_ABI } from "../utils/contractConfig";
import { JsonRpcProvider, Contract } from "ethers";

const PUBLIC_SEPOLIA_RPC = "https://ethereum-sepolia-rpc.publicnode.com";

export default function CredentialDetailsPage() {
  const { id } = useParams();
  const { readOnlyContract } = useWeb3();
  const [credential, setCredential] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    let cancelled = false;

    async function fetchCredential() {
      setIsLoading(true);
      setError(null);
      try {
        const contract =
          readOnlyContract || new Contract(CONTRACT_ADDRESS, CONTRACT_ABI, new JsonRpcProvider(PUBLIC_SEPOLIA_RPC));
        const data = await contract.verifyCredential(id);
        if (!cancelled) setCredential(data);
      } catch (err) {
        if (!cancelled) setError("This credential could not be found.");
      } finally {
        if (!cancelled) setIsLoading(false);
      }
    }

    fetchCredential();
    return () => {
      cancelled = true;
    };
  }, [id, readOnlyContract]);

  return (
    <div>
      <Navbar />
      <section className="section" style={{ maxWidth: 700 }}>
        <p className="eyebrow">Credential #{id}</p>

        {isLoading && <LoadingSpinner label="Fetching credential from chain..." />}
        {error && <div className="tx-banner tx-error">{error}</div>}

        {credential && (
          <div className="card" style={{ display: "flex", gap: 24, marginTop: 16 }}>
            <CredentialSeal institutionName={credential.institutionName} revoked={credential.revoked} />
            <div>
              <span className={`status-pill ${credential.revoked ? "status-revoked" : "status-valid"}`}>
                {credential.revoked ? "Revoked" : "Valid Credential"}
              </span>
              <h1 style={{ fontSize: "1.6rem", marginTop: 12 }}>{credential.degreeOrCertification}</h1>
              <p style={{ color: "var(--charcoal-muted)" }}>{credential.course}</p>

              <dl style={{ marginTop: 16, fontSize: "0.92rem", color: "var(--charcoal-muted)", lineHeight: 1.8 }}>
                <p><strong>Student:</strong> {credential.studentName} ({shortenAddress(credential.student)})</p>
                <p><strong>Institution:</strong> {credential.institutionName} ({shortenAddress(credential.institution)})</p>
                <p><strong>Date issued:</strong> {formatTimestamp(credential.dateIssued)}</p>
                <p>
                  <strong>Institution accreditation:</strong>{" "}
                  {credential.institutionCurrentlyRegistered ? "Active" : "No longer active"}
                </p>
                {credential.metadataURI && (
                  <p><strong>Supporting documents:</strong> {credential.metadataURI}</p>
                )}
              </dl>
            </div>
          </div>
        )}
      </section>
    </div>
  );
}
