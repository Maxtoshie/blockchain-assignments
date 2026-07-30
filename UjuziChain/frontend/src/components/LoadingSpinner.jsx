export default function LoadingSpinner({ label = "Loading..." }) {
  return (
    <div style={{ display: "flex", alignItems: "center", gap: 10, color: "var(--charcoal-muted)", padding: "20px 0" }}>
      <span className="spinner" />
      <span>{label}</span>
    </div>
  );
}
