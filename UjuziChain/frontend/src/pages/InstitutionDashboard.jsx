import { useState } from "react";
import DashboardLayout from "../components/DashboardLayout";
import TransactionStatus from "../components/TransactionStatus";
import { useWeb3 } from "../hooks/useWeb3";

export default function InstitutionDashboard() {
  const { contract, account } = useWeb3();
  const [form, setForm] = useState({
    student: "",
    studentName: "",
    degree: "",
    course: "",
    metadataURI: ""
  });
  const [revokeId, setRevokeId] = useState("");
  const [txStatus, setTxStatus] = useState(null);
  const [txMessage, setTxMessage] = useState("");

  function updateField(field, value) {
    setForm((prev) => ({ ...prev, [field]: value }));
  }

  async function handleIssue(event) {
    event.preventDefault();
    if (!contract) return;

    try {
      setTxStatus("pending");
      setTxMessage("Confirm the transaction in your wallet...");
      const tx = await contract.issueCredential(
        form.student.trim(),
        form.studentName.trim(),
        form.degree.trim(),
        form.course.trim(),
        form.metadataURI.trim()
      );
      setTxMessage("Waiting for confirmation on-chain...");
      const receipt = await tx.wait();
      setTxStatus("success");
      setTxMessage(`Credential issued to ${form.studentName} successfully.`);
      setForm({ student: "", studentName: "", degree: "", course: "", metadataURI: "" });
      void receipt;
    } catch (err) {
      setTxStatus("error");
      setTxMessage(err?.reason || err?.message || "Transaction failed.");
    }
  }

  async function handleRevoke(event) {
    event.preventDefault();
    if (!contract) return;

    try {
      setTxStatus("pending");
      setTxMessage("Confirm the transaction in your wallet...");
      const tx = await contract.revokeCredential(revokeId.trim());
      setTxMessage("Waiting for confirmation on-chain...");
      await tx.wait();
      setTxStatus("success");
      setTxMessage(`Credential #${revokeId} revoked.`);
      setRevokeId("");
    } catch (err) {
      setTxStatus("error");
      setTxMessage(err?.reason || err?.message || "Transaction failed.");
    }
  }

  if (!account) {
    return (
      <DashboardLayout>
        <h1>Institution Dashboard</h1>
        <p style={{ color: "var(--charcoal-muted)", marginTop: 12 }}>
          Connect your registered institution wallet to issue credentials.
        </p>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <h1>Institution Dashboard</h1>
      <p style={{ color: "var(--charcoal-muted)", marginTop: 8, marginBottom: 28 }}>
        Issue a new credential directly to a student's wallet, or revoke one
        your institution previously issued.
      </p>

      <TransactionStatus status={txStatus} message={txMessage} />

      <div style={{ display: "grid", gridTemplateColumns: "1.3fr 1fr", gap: 24, alignItems: "start" }}>
        <form onSubmit={handleIssue} className="card">
          <h3 style={{ fontSize: "1.05rem", marginBottom: 16 }}>Issue Credential</h3>
          <div className="field field-mono">
            <label htmlFor="student-address">Student wallet address</label>
            <input
              id="student-address"
              placeholder="0x..."
              value={form.student}
              onChange={(e) => updateField("student", e.target.value)}
              required
            />
          </div>
          <div className="field">
            <label htmlFor="student-name">Student name</label>
            <input
              id="student-name"
              placeholder="e.g. Amina Wanjiru"
              value={form.studentName}
              onChange={(e) => updateField("studentName", e.target.value)}
              required
            />
          </div>
          <div className="field">
            <label htmlFor="degree">Degree or certification</label>
            <input
              id="degree"
              placeholder="e.g. BSc Computer Science"
              value={form.degree}
              onChange={(e) => updateField("degree", e.target.value)}
              required
            />
          </div>
          <div className="field">
            <label htmlFor="course">Course</label>
            <input
              id="course"
              placeholder="e.g. Distributed Systems"
              value={form.course}
              onChange={(e) => updateField("course", e.target.value)}
              required
            />
          </div>
          <div className="field">
            <label htmlFor="metadata">Metadata URI (optional)</label>
            <input
              id="metadata"
              placeholder="ipfs://..."
              value={form.metadataURI}
              onChange={(e) => updateField("metadataURI", e.target.value)}
            />
          </div>
          <button type="submit" className="btn btn-primary">Issue Credential</button>
        </form>

        <form onSubmit={handleRevoke} className="card">
          <h3 style={{ fontSize: "1.05rem", marginBottom: 16 }}>Revoke Credential</h3>
          <div className="field field-mono">
            <label htmlFor="revoke-id">Credential ID</label>
            <input
              id="revoke-id"
              type="number"
              min="1"
              placeholder="e.g. 1"
              value={revokeId}
              onChange={(e) => setRevokeId(e.target.value)}
              required
            />
          </div>
          <button type="submit" className="btn btn-ghost">Revoke</button>
        </form>
      </div>
    </DashboardLayout>
  );
}
