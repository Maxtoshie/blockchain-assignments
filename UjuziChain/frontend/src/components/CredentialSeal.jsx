// The signature visual element of UjuziChain: a circular wax-seal-style
// badge with the issuing institution's name arced around the inner rim —
// the digital analogue of a physical university seal on a paper certificate.

export default function CredentialSeal({ institutionName = "UJUZICHAIN", revoked = false }) {
  const pathId = `seal-arc-${institutionName.replace(/\s+/g, "-")}-${revoked}`;
  const label = institutionName.toUpperCase();

  return (
    <div className={`seal ${revoked ? "seal-revoked" : ""}`}>
      <svg viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
        <defs>
          <path id={pathId} d="M 15,50 A 35,35 0 1,1 85,50" fill="none" />
        </defs>
        <circle className="seal-ring" cx="50" cy="50" r="46" />
        <circle className="seal-ring-inner" cx="50" cy="50" r="38" />
        <text className="seal-text" textAnchor="middle">
          <textPath href={`#${pathId}`} startOffset="50%">
            {label}
          </textPath>
        </text>
        {revoked ? (
          <text x="50" y="56" textAnchor="middle" fontSize="11" fill="#f2ecdd" fontFamily="IBM Plex Mono">
            REVOKED
          </text>
        ) : (
          <path
            d="M 38 50 L 46 58 L 63 40"
            stroke="#e8a33d"
            strokeWidth="4"
            fill="none"
            strokeLinecap="round"
            strokeLinejoin="round"
          />
        )}
      </svg>
    </div>
  );
}
