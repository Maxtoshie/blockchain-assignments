import { Link } from "react-router-dom";
import Navbar from "../components/Navbar";
import CredentialSeal from "../components/CredentialSeal";

export default function LandingPage() {
  return (
    <div>
      <Navbar />

      <section className="hero">
        <p className="eyebrow hero-eyebrow">
          Skills Passport &middot; Built on Ethereum
        </p>
        <h1>
          Empowering East African talent through trusted digital credentials.
        </h1>
        <p>
          UjuziChain lets accredited institutions issue tamper-proof academic
          and professional credentials. Students own them. Employers verify them
          instantly — no phone calls, no paperwork, no borders.
        </p>

        {/* --- Quick Action Hub Buttons --- */}
        <div
          className="hero-actions"
          style={{
            display: "flex",
            gap: "12px",
            flexWrap: "wrap",
            marginTop: "24px",
          }}
        >
          <Link to="/dashboard/admin" className="btn btn-gold">
            🛡️ Admin Portal
          </Link>
          <Link to="/dashboard/institution" className="btn btn-gold">
            🏛️ Institution Portal
          </Link>
          <Link to="/dashboard/student" className="btn btn-gold">
            🎓 Student Portal
          </Link>
          <Link to="/verify" className="btn btn-gold">
            🔍 Verify Credential
          </Link>
          <Link to="/about" className="btn btn-ghost">
            How it works
          </Link>
        </div>
      </section>

      <section className="section" style={{ background: "var(--paper)" }}>
        <h2 className="section-title">One seal, three roles, zero friction</h2>
        <div className="grid-3">
          <div
            className="card"
            style={{
              display: "flex",
              flexDirection: "column",
              justifyContent: "space-between",
            }}
          >
            <div>
              <CredentialSeal institutionName="Institution" />
              <h3 style={{ marginTop: 16, fontSize: "1.1rem" }}>
                Institutions issue
              </h3>
              <p
                style={{
                  color: "var(--charcoal-muted)",
                  fontSize: "0.92rem",
                  marginBottom: 20,
                }}
              >
                Accredited universities and colleges register once, then issue
                credentials directly to a student's wallet — each one a
                permanent, tamper-proof record.
              </p>
            </div>
            <Link
              to="/dashboard/institution"
              className="btn btn-ghost"
              style={{ alignSelf: "flex-start" }}
            >
              Go to Issuer Portal &rarr;
            </Link>
          </div>

          <div
            className="card"
            style={{
              display: "flex",
              flexDirection: "column",
              justifyContent: "space-between",
            }}
          >
            <div>
              <CredentialSeal institutionName="Student" />
              <h3 style={{ marginTop: 16, fontSize: "1.1rem" }}>
                Students own
              </h3>
              <p
                style={{
                  color: "var(--charcoal-muted)",
                  fontSize: "0.92rem",
                  marginBottom: 20,
                }}
              >
                Credentials live in the student's own wallet — not a registrar's
                filing cabinet. Share proof of achievement with a wallet
                address, nothing else.
              </p>
            </div>
            <Link
              to="/dashboard/student"
              className="btn btn-ghost"
              style={{ alignSelf: "flex-start" }}
            >
              View Student Wallet &rarr;
            </Link>
          </div>

          <div
            className="card"
            style={{
              display: "flex",
              flexDirection: "column",
              justifyContent: "space-between",
            }}
          >
            <div>
              <CredentialSeal institutionName="Employer" revoked={false} />
              <h3 style={{ marginTop: 16, fontSize: "1.1rem" }}>
                Employers verify
              </h3>
              <p
                style={{
                  color: "var(--charcoal-muted)",
                  fontSize: "0.92rem",
                  marginBottom: 20,
                }}
              >
                Paste a credential ID, get an instant, cryptographically backed
                answer — issuing institution, date, and current status.
              </p>
            </div>
            <Link
              to="/verify"
              className="btn btn-ghost"
              style={{ alignSelf: "flex-start" }}
            >
              Verify a Record &rarr;
            </Link>
          </div>
        </div>
      </section>

      <section className="section">
        <h2 className="section-title">Why it matters</h2>
        <div className="grid-3">
          <div className="card">
            <p className="eyebrow">The problem</p>
            <p style={{ marginTop: 8 }}>
              Cross-border credential checks are slow, costly, and easy to fake
              — a real barrier to regional job mobility.
            </p>
          </div>
          <div className="card">
            <p className="eyebrow">The fix</p>
            <p style={{ marginTop: 8 }}>
              An immutable, publicly verifiable record that any employer in East
              Africa can check in seconds, for free.
            </p>
          </div>
          <div className="card">
            <p className="eyebrow">The outcome</p>
            <p style={{ marginTop: 8 }}>
              Faster hiring, less fraud, and a regional talent pool employers
              can actually trust.
            </p>
          </div>
        </div>
      </section>
    </div>
  );
}
