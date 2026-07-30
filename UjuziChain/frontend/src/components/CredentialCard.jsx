import { Link } from "react-router-dom";
import CredentialSeal from "./CredentialSeal";
import { formatTimestamp, shortenAddress } from "../utils/format";

export default function CredentialCard({ credential }) {
  const {
    id,
    studentName,
    institutionName,
    degreeOrCertification,
    course,
    dateIssued,
    revoked,
    student
  } = credential;

  return (
    <Link to={`/credentials/${id}`} className="card" style={{ display: "flex", gap: 20, textDecoration: "none" }}>
      <CredentialSeal institutionName={institutionName} revoked={revoked} />
      <div style={{ flex: 1 }}>
        <div style={{ display: "flex", justifyContent: "space-between", alignItems: "flex-start" }}>
          <div>
            <p className="eyebrow" style={{ marginBottom: 4 }}>Credential #{id.toString()}</p>
            <h3 style={{ fontSize: "1.15rem" }}>{degreeOrCertification}</h3>
          </div>
          <span className={`status-pill ${revoked ? "status-revoked" : "status-valid"}`}>
            {revoked ? "Revoked" : "Valid"}
          </span>
        </div>
        <p style={{ color: "var(--charcoal-muted)", margin: "8px 0 4px", fontSize: "0.92rem" }}>
          {course} &middot; {institutionName}
        </p>
        <p style={{ color: "var(--charcoal-muted)", margin: 0, fontSize: "0.85rem" }}>
          Issued to {studentName} ({shortenAddress(student)}) on {formatTimestamp(dateIssued)}
        </p>
      </div>
    </Link>
  );
}
