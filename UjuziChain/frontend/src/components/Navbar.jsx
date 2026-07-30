import { Link } from "react-router-dom";
import ConnectWalletButton from "./ConnectWalletButton";

export default function Navbar() {
  return (
    <nav className="landing-nav">
      <Link to="/" style={{ textDecoration: "none" }}>
        <span className="sidebar-logo" style={{ color: "var(--ink)" }}>UjuziChain</span>
      </Link>
      <div style={{ display: "flex", gap: 24, alignItems: "center" }}>
        <Link to="/about" style={{ textDecoration: "none", fontWeight: 500 }}>
          About
        </Link>
        <Link to="/verify" style={{ textDecoration: "none", fontWeight: 500 }}>
          Verify a Credential
        </Link>
        <ConnectWalletButton />
      </div>
    </nav>
  );
}
