import { useState } from "react";
import DashboardLayout from "../components/DashboardLayout";
import TransactionStatus from "../components/TransactionStatus";
import { useWeb3 } from "../hooks/useWeb3";

export default function AdminDashboard() {
  const { contract, account } = useWeb3();
  const [institutionAddress, setInstitutionAddress] = useState("");
  const [institutionName, setInstitutionName] = useState("");
  const [removeAddress, setRemoveAddress] = useState("");
  const [txStatus, setTxStatus] = useState(null);
  const [txMessage, setTxMessage] = useState("");

  async function handleRegister(event) {
    event.preventDefault();
    if (!contract) return;

    try {
      setTxStatus("pending");
      setTxMessage("Confirm the transaction in your wallet...");
      const tx = await contract.registerInstitution(institutionAddress.trim(), institutionName.trim());
      setTxMessage("Waiting for confirmation on-chain...");
      await tx.wait();
      setTxStatus("success");
      setTxMessage(`${institutionName} was registered successfully.`);
      setInstitutionAddress("");
      setInstitutionName("");
    } catch (err) {
      setTxStatus("error");
      setTxMessage(err?.reason || err?.message || "Transaction failed.");
    }
  }

  async function handleRemove(event) {
    event.preventDefault();
    if (!contract) return;

    try {
      setTxStatus("pending");
      setTxMessage("Confirm the transaction in your wallet...");
      const tx = await contract.removeInstitution(removeAddress.trim());
      setTxMessage("Waiting for confirmation on-chain...");
      await tx.wait();
      setTxStatus("success");
      setTxMessage("Institution removed. Past credentials remain valid and verifiable.");
      setRemoveAddress("");
    } catch (err) {
      setTxStatus("error");
      setTxMessage(err?.reason || err?.message || "Transaction failed.");
    }
  }

  if (!account) {
    return (
      <DashboardLayout>
        <h1>Admin Dashboard</h1>
        <p style={{ color: "var(--charcoal-muted)", marginTop: 12 }}>
          Connect your wallet to manage institutions.
        </p>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <h1>Admin Dashboard</h1>
      <p style={{ color: "var(--charcoal-muted)", marginTop: 8, marginBottom: 28 }}>
        Register accredited institutions or remove one that no longer qualifies.
        Removing an institution only blocks future issuance — past credentials stay verifiable.
      </p>

      <TransactionStatus status={txStatus} message={txMessage} />

      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 24 }}>
        <form onSubmit={handleRegister} className="card">
          <h3 style={{ fontSize: "1.05rem", marginBottom: 16 }}>Register Institution</h3>
          <div className="field field-mono">
            <label htmlFor="reg-address">Institution wallet address</label>
            <input
              id="reg-address"
              placeholder="0x..."
              value={institutionAddress}
              onChange={(e) => setInstitutionAddress(e.target.value)}
              required
            />
          </div>
          <div className="field">
            <label htmlFor="reg-name">Institution name</label>
            <input
              id="reg-name"
              placeholder="e.g. University of Nairobi"
              value={institutionName}
              onChange={(e) => setInstitutionName(e.target.value)}
              required
            />
          </div>
          <button type="submit" className="btn btn-primary">Register</button>
        </form>

        <form onSubmit={handleRemove} className="card">
          <h3 style={{ fontSize: "1.05rem", marginBottom: 16 }}>Remove Institution</h3>
          <div className="field field-mono">
            <label htmlFor="rem-address">Institution wallet address</label>
            <input
              id="rem-address"
              placeholder="0x..."
              value={removeAddress}
              onChange={(e) => setRemoveAddress(e.target.value)}
              required
            />
          </div>
          <button type="submit" className="btn btn-ghost">Remove</button>
        </form>
      </div>
    </DashboardLayout>
  );
}
