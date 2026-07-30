import { NavLink } from "react-router-dom";
import ConnectWalletButton from "./ConnectWalletButton";
import { useRole } from "../hooks/useRole";

const NAV_ITEMS = {
  admin: [
    { to: "/dashboard/admin", label: "Admin Dashboard" },
    { to: "/verify", label: "Verify Credential" }
  ],
  institution: [
    { to: "/dashboard/institution", label: "Institution Dashboard" },
    { to: "/verify", label: "Verify Credential" }
  ],
  guest: [
    { to: "/dashboard/student", label: "Student Dashboard" },
    { to: "/verify", label: "Verify Credential" }
  ]
};

export default function DashboardLayout({ children }) {
  const { role } = useRole();
  const navItems = NAV_ITEMS[role] || NAV_ITEMS.guest;

  return (
    <div className="app-shell">
      <aside className="sidebar">
        <div className="sidebar-logo">UjuziChain</div>
        <nav className="sidebar-nav">
          <NavLink to="/" className="sidebar-link" end>
            Home
          </NavLink>
          {navItems.map((item) => (
            <NavLink
              key={item.to}
              to={item.to}
              className={({ isActive }) => `sidebar-link ${isActive ? "active" : ""}`}
            >
              {item.label}
            </NavLink>
          ))}
        </nav>
        <div style={{ marginTop: "auto" }}>
          <ConnectWalletButton />
        </div>
      </aside>
      <main className="main-content">{children}</main>
    </div>
  );
}
