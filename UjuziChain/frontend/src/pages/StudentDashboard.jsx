import { useEffect, useState } from "react";
import DashboardLayout from "../components/DashboardLayout";
import CredentialCard from "../components/CredentialCard";
import LoadingSpinner from "../components/LoadingSpinner";
import { useWeb3 } from "../hooks/useWeb3";

export default function StudentDashboard() {
  const { account, readOnlyContract } = useWeb3();
  const [credentials, setCredentials] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);

  useEffect(() => {
    let cancelled = false;

    async function fetchCredentials() {
      if (!account || !readOnlyContract) {
        setCredentials([]);
        return;
      }

      setIsLoading(true);
      setError(null);
      try {
        const ids = await readOnlyContract.getCredentialsByStudent(account);
        const details = await Promise.all(
          ids.map(async (id) => {
            const data = await readOnlyContract.verifyCredential(id);
            return { id, ...data };
          })
        );
        if (!cancelled) setCredentials(details);
      } catch (err) {
        if (!cancelled) setError("Couldn't load your credentials right now.");
      } finally {
        if (!cancelled) setIsLoading(false);
      }
    }

    fetchCredentials();
    return () => {
      cancelled = true;
    };
  }, [account, readOnlyContract]);

  if (!account) {
    return (
      <DashboardLayout>
        <h1>Student Dashboard</h1>
        <p style={{ color: "var(--charcoal-muted)", marginTop: 12 }}>
          Connect your wallet to see the credentials issued to you.
        </p>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <h1>Your Credentials</h1>
      <p style={{ color: "var(--charcoal-muted)", marginTop: 8, marginBottom: 28 }}>
        These credentials were issued directly to your wallet address. Share
        a credential's ID with any employer for instant verification.
      </p>

      {isLoading && <LoadingSpinner label="Loading your credentials..." />}
      {error && <div className="tx-banner tx-error">{error}</div>}

      {!isLoading && !error && credentials.length === 0 && (
        <div className="card">
          <p style={{ color: "var(--charcoal-muted)" }}>
            No credentials yet. Once an accredited institution issues one to
            your wallet, it will appear here.
          </p>
        </div>
      )}

      <div style={{ display: "flex", flexDirection: "column", gap: 16 }}>
        {credentials.map((credential) => (
          <CredentialCard key={credential.id.toString()} credential={credential} />
        ))}
      </div>
    </DashboardLayout>
  );
}
