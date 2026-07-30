import Navbar from "../components/Navbar";

export default function AboutPage() {
  return (
    <div>
      <Navbar />
      <section className="hero" style={{ paddingBottom: 40 }}>
        <p className="eyebrow hero-eyebrow">About the project</p>
        <h1 style={{ fontSize: "2.6rem" }}>A Skills Passport for a region on the move.</h1>
        <p>
          Graduates and professionals across East Africa often struggle to
          secure employment because employers can't easily verify credentials
          issued by institutions in other countries. Paper certificates get
          lost, forged, or simply take too long to confirm.
        </p>
      </section>

      <section className="section" style={{ background: "var(--paper)", paddingTop: 0 }}>
        <div className="grid-3">
          <div className="card">
            <h3 style={{ fontSize: "1.05rem", marginBottom: 10 }}>Global Challenge</h3>
            <p style={{ color: "var(--charcoal-muted)", fontSize: "0.92rem" }}>Job Creation</p>
          </div>
          <div className="card">
            <h3 style={{ fontSize: "1.05rem", marginBottom: 10 }}>Global Opportunity</h3>
            <p style={{ color: "var(--charcoal-muted)", fontSize: "0.92rem" }}>Regional Integration</p>
          </div>
          <div className="card">
            <h3 style={{ fontSize: "1.05rem", marginBottom: 10 }}>Application Area</h3>
            <p style={{ color: "var(--charcoal-muted)", fontSize: "0.92rem" }}>Education</p>
          </div>
        </div>
      </section>

      <section className="section">
        <h2 className="section-title">How verification works</h2>
        <ol style={{ maxWidth: 640, lineHeight: 1.9, color: "var(--charcoal-muted)", paddingLeft: 20 }}>
          <li>An accredited institution is registered on-chain by the platform admin.</li>
          <li>The institution issues a credential directly to a student's wallet address.</li>
          <li>The credential — degree, course, date, issuing institution — is stored immutably.</li>
          <li>Anyone, including an employer with no wallet at all, can look up a credential ID and see its full, current status.</li>
          <li>If needed, only the original issuing institution can revoke a credential it issued.</li>
        </ol>
      </section>
    </div>
  );
}
