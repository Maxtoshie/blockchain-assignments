// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import "@openzeppelin/contracts/access/Ownable.sol";

/**
 * @title UjuziChain
 * @author UjuziChain Team
 * @notice A decentralized Skills Passport that lets accredited institutions
 *         issue tamper-proof, revocable academic/professional credentials.
 *         Employers can verify any credential instantly on-chain.
 *
 * @dev Design notes:
 *      - The contract owner (deployer) acts as the platform Admin, and is
 *        responsible for registering/removing institutions.
 *      - Only registered institutions may issue or revoke credentials.
 *      - Credentials are never deleted — they are marked `revoked` to
 *        preserve a permanent, auditable history (immutability by design).
 *      - Verification is a plain view function: anyone (employers included)
 *        can call it for free, with no wallet connection required for reads.
 */
contract UjuziChain is Ownable {
    // ---------------------------------------------------------------------
    // Types
    // ---------------------------------------------------------------------

    struct Institution {
        string name;
        bool isRegistered;
        uint256 registeredAt;
    }

    struct Credential {
        uint256 id;
        address student;
        string studentName;
        address institution;   // wallet address of the issuing institution
        string institutionName; // snapshot at issuance time, for history integrity
        string degreeOrCertification;
        string course;
        uint256 dateIssued;
        string metadataURI;    // e.g. IPFS URI for supporting documents
        bool revoked;
    }

    // ---------------------------------------------------------------------
    // State
    // ---------------------------------------------------------------------

    /// @notice institution wallet address => Institution details
    mapping(address => Institution) public institutions;

    /// @notice credential ID => Credential details
    mapping(uint256 => Credential) public credentials;

    /// @notice student wallet address => list of credential IDs they own
    mapping(address => uint256[]) private studentCredentials;

    /// @notice auto-incrementing credential ID counter
    uint256 private nextCredentialId = 1;

    // ---------------------------------------------------------------------
    // Events
    // ---------------------------------------------------------------------

    event InstitutionRegistered(address indexed institution, string name, uint256 timestamp);
    event InstitutionRemoved(address indexed institution, uint256 timestamp);

    event CredentialIssued(
        uint256 indexed credentialId,
        address indexed student,
        address indexed institution,
        string degreeOrCertification,
        uint256 dateIssued
    );

    event CredentialRevoked(uint256 indexed credentialId, address indexed institution, uint256 timestamp);
    event CredentialMetadataUpdated(uint256 indexed credentialId, string newMetadataURI);

    // ---------------------------------------------------------------------
    // Modifiers
    // ---------------------------------------------------------------------

    modifier onlyRegisteredInstitution() {
        require(institutions[msg.sender].isRegistered, "UjuziChain: caller is not a registered institution");
        _;
    }

    modifier credentialExists(uint256 credentialId) {
        require(credentials[credentialId].id != 0, "UjuziChain: credential does not exist");
        _;
    }

    // ---------------------------------------------------------------------
    // Constructor
    // ---------------------------------------------------------------------

    constructor() Ownable(msg.sender) {}

    // ---------------------------------------------------------------------
    // Institution Management (Admin only)
    // ---------------------------------------------------------------------

    /**
     * @notice Register a new accredited institution.
     * @param institutionAddress The wallet address that will issue credentials.
     * @param name The institution's display name.
     */
    function registerInstitution(address institutionAddress, string calldata name) external onlyOwner {
        require(institutionAddress != address(0), "UjuziChain: zero address");
        require(!institutions[institutionAddress].isRegistered, "UjuziChain: already registered");
        require(bytes(name).length > 0, "UjuziChain: name required");

        institutions[institutionAddress] = Institution({
            name: name,
            isRegistered: true,
            registeredAt: block.timestamp
        });

        emit InstitutionRegistered(institutionAddress, name, block.timestamp);
    }

    /**
     * @notice Remove an institution's ability to issue new credentials.
     * @dev Previously issued credentials remain valid/verifiable — removing
     *      an institution does NOT retroactively revoke its past credentials.
     */
    function removeInstitution(address institutionAddress) external onlyOwner {
        require(institutions[institutionAddress].isRegistered, "UjuziChain: not registered");

        institutions[institutionAddress].isRegistered = false;

        emit InstitutionRemoved(institutionAddress, block.timestamp);
    }

    // ---------------------------------------------------------------------
    // Credential Management (Registered Institutions only)
    // ---------------------------------------------------------------------

    /**
     * @notice Issue a new credential to a student.
     * @return credentialId The ID of the newly issued credential.
     */
    function issueCredential(
        address student,
        string calldata studentName,
        string calldata degreeOrCertification,
        string calldata course,
        string calldata metadataURI
    ) external onlyRegisteredInstitution returns (uint256 credentialId) {
        require(student != address(0), "UjuziChain: zero address");
        require(bytes(studentName).length > 0, "UjuziChain: student name required");

        credentialId = nextCredentialId++;

        credentials[credentialId] = Credential({
            id: credentialId,
            student: student,
            studentName: studentName,
            institution: msg.sender,
            institutionName: institutions[msg.sender].name,
            degreeOrCertification: degreeOrCertification,
            course: course,
            dateIssued: block.timestamp,
            metadataURI: metadataURI,
            revoked: false
        });

        studentCredentials[student].push(credentialId);

        emit CredentialIssued(credentialId, student, msg.sender, degreeOrCertification, block.timestamp);
    }

    /**
     * @notice Revoke a credential. Only the institution that issued it may revoke it.
     */
    function revokeCredential(uint256 credentialId)
        external
        onlyRegisteredInstitution
        credentialExists(credentialId)
    {
        Credential storage cred = credentials[credentialId];
        require(cred.institution == msg.sender, "UjuziChain: not the issuing institution");
        require(!cred.revoked, "UjuziChain: already revoked");

        cred.revoked = true;

        emit CredentialRevoked(credentialId, msg.sender, block.timestamp);
    }

    /**
     * @notice Update the metadata URI of a credential (e.g. if supporting docs move).
     */
    function updateMetadataURI(uint256 credentialId, string calldata newMetadataURI)
        external
        onlyRegisteredInstitution
        credentialExists(credentialId)
    {
        Credential storage cred = credentials[credentialId];
        require(cred.institution == msg.sender, "UjuziChain: not the issuing institution");

        cred.metadataURI = newMetadataURI;

        emit CredentialMetadataUpdated(credentialId, newMetadataURI);
    }

    // ---------------------------------------------------------------------
    // Verification (Public / Employer-facing)
    // ---------------------------------------------------------------------

    /**
     * @notice Verify a credential's full details in one call.
     * @dev Free to call (view function) — no gas cost for employers checking credentials.
     */
    function verifyCredential(uint256 credentialId)
        external
        view
        credentialExists(credentialId)
        returns (
            address student,
            string memory studentName,
            address institution,
            string memory institutionName,
            string memory degreeOrCertification,
            string memory course,
            uint256 dateIssued,
            string memory metadataURI,
            bool revoked,
            bool institutionCurrentlyRegistered
        )
    {
        Credential storage cred = credentials[credentialId];
        return (
            cred.student,
            cred.studentName,
            cred.institution,
            cred.institutionName,
            cred.degreeOrCertification,
            cred.course,
            cred.dateIssued,
            cred.metadataURI,
            cred.revoked,
            institutions[cred.institution].isRegistered
        );
    }

    /**
     * @notice Get all credential IDs owned by a student.
     */
    function getCredentialsByStudent(address student) external view returns (uint256[] memory) {
        return studentCredentials[student];
    }

    /**
     * @notice Check whether an address is a currently registered institution.
     */
    function isInstitutionRegistered(address institutionAddress) external view returns (bool) {
        return institutions[institutionAddress].isRegistered;
    }

    /**
     * @notice Total number of credentials issued so far (including revoked ones).
     */
    function totalCredentialsIssued() external view returns (uint256) {
        return nextCredentialId - 1;
    }
}
