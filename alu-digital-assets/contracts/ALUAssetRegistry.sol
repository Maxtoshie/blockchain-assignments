// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

import "@openzeppelin/contracts/token/ERC721/ERC721.sol";

/**
 * @title ALUAssetRegistry
 * @dev Registers and protects the official ALU logo as a unique digital asset on the blockchain.
 */
contract ALUAssetRegistry is ERC721 {
    uint256 private _tokenIds;

    // Struct to hold asset metadata
    struct AssetMetadata {
        string assetName;
        string fileType;
        bytes32 contentHash;
        address registeredBy;
        uint256 timestamp;
    }

    // Mapping from token ID to Asset Metadata
    mapping(uint256 => AssetMetadata) private _assets;

    // Mapping to track registered content hashes to prevent duplicates
    mapping(bytes32 => bool) private _registeredHashes;

    // Event emitted when a new asset is successfully registered
    event AssetRegistered(
        uint256 indexed tokenId,
        string assetName,
        bytes32 indexed contentHash,
        address indexed registeredBy
    );

    constructor() ERC721("ALU Digital Asset", "ALUDA") {}

    /**
     * @notice Registers a new unique asset on the blockchain
     * @param assetName The name of the digital asset
     * @param fileType The file extension/type (e.g., png, webp)
     * @param contentHash The SHA-256 hash of the file content
     * @return The newly generated token ID
     */
    function registerAsset(
        string memory assetName,
        string memory fileType,
        bytes32 contentHash
    ) public returns (uint256) {
        // Enforce uniqueness: Reject if the hash has already been registered
        require(!_registeredHashes[contentHash], "Error: Asset hash already registered.");

        _tokenIds++;
        uint256 newTokenId = _tokenIds;

        // Mint the NFT safely to the function caller
        _safeMint(msg.sender, newTokenId);

        // Populate the metadata struct
        _assets[newTokenId] = AssetMetadata({
            assetName: assetName,
            fileType: fileType,
            contentHash: contentHash,
            registeredBy: msg.sender,
            timestamp: block.timestamp
        });

        // Flag the hash as registered to prevent duplicates
        _registeredHashes[contentHash] = true;

        // Emit the structural event log
        emit AssetRegistered(newTokenId, assetName, contentHash, msg.sender);

        return newTokenId;
    }

    /**
     * @notice Verifies whether a provided content hash matches the registered asset
     * @param tokenId The unique token ID of the asset
     * @param suppliedHash The SHA-256 hash provided by the validator
     * @return isValid True if matching, false otherwise
     * @return message A clear message text indicating authenticity status
     */
    function verifyLogoIntegrity(
        uint256 tokenId,
        bytes32 suppliedHash
    ) public view returns (bool isValid, string memory message) {
        // Ensure the token exists by checking its registered owner
        require(_ownerOf(tokenId) != address(0), "Error: Token ID does not exist.");

        if (_assets[tokenId].contentHash == suppliedHash) {
            return (true, "Logo is authentic.");
        } else {
            return (false, "Warning: logo does not match.");
        }
    }

    /**
     * @notice Retrieves the full structural asset data metadata for a given token ID
     * @param tokenId The unique identifier of the registered asset
     */
    function getAsset(uint256 tokenId) public view returns (AssetMetadata memory) {
        require(_ownerOf(tokenId) != address(0), "Error: Token ID does not exist.");
        return _assets[tokenId];
    }
}
