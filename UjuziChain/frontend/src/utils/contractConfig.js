// -----------------------------------------------------------------------
// Contract configuration
// -----------------------------------------------------------------------
// After running `npx hardhat run scripts/deploy.js --network sepolia`,
// paste the deployed contract address below.
// -----------------------------------------------------------------------

export const CONTRACT_ADDRESS = "0x5FbDB2315678afecb367f032d93F642f64180aa3";

export const SEPOLIA_CHAIN_ID = 11155111n;

export const CONTRACT_ABI = [
  // --- Institution management ---
  "function registerInstitution(address institutionAddress, string name) external",
  "function removeInstitution(address institutionAddress) external",
  "function isInstitutionRegistered(address institutionAddress) external view returns (bool)",
  "function institutions(address) public view returns (string name, bool isRegistered, uint256 registeredAt)",

  // --- Credential management ---
  "function issueCredential(address student, string studentName, string degreeOrCertification, string course, string metadataURI) external returns (uint256 credentialId)",
  "function revokeCredential(uint256 credentialId) external",
  "function updateMetadataURI(uint256 credentialId, string newMetadataURI) external",

  // --- Verification / reads ---
  "function verifyCredential(uint256 credentialId) external view returns (address student, string studentName, address institution, string institutionName, string degreeOrCertification, string course, uint256 dateIssued, string metadataURI, bool revoked, bool institutionCurrentlyRegistered)",
  "function getCredentialsByStudent(address student) external view returns (uint256[])",
  "function totalCredentialsIssued() external view returns (uint256)",
  "function owner() external view returns (address)",

  // --- Events ---
  "event InstitutionRegistered(address indexed institution, string name, uint256 timestamp)",
  "event InstitutionRemoved(address indexed institution, uint256 timestamp)",
  "event CredentialIssued(uint256 indexed credentialId, address indexed student, address indexed institution, string degreeOrCertification, uint256 dateIssued)",
  "event CredentialRevoked(uint256 indexed credentialId, address indexed institution, uint256 timestamp)",
  "event CredentialMetadataUpdated(uint256 indexed credentialId, string newMetadataURI)",
];
