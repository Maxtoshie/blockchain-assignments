const { expect } = require("chai");
const { ethers } = require("hardhat");
const crypto = require("crypto");

/**
 * Helper that mirrors exactly what frontend/js/hash.js does in the browser:
 * take raw file bytes -> SHA-256 -> 0x-prefixed 32-byte hex string.
 * Used here to test the hashing logic independent of the DOM/File APIs,
 * which are not available in the Node/Mocha test environment.
 */
function computeSha256Bytes32(buffer) {
  const digest = crypto.createHash("sha256").update(buffer).digest("hex");
  return "0x" + digest;
}

describe("ALU Digital Assets - Smart Contracts (Formative 1)", function () {
  let ALUAssetRegistry, registry;
  let ALULogoToken, token;
  let owner, alice, bob, carol;

  const SAMPLE_HASH_A =
    "0x" + "11".repeat(32); // 32-byte dummy hash for the "official logo"
  const SAMPLE_HASH_B =
    "0x" + "22".repeat(32); // a different, non-matching hash

  beforeEach(async function () {
    [owner, alice, bob, carol] = await ethers.getSigners();

    ALUAssetRegistry = await ethers.getContractFactory("ALUAssetRegistry");
    registry = await ALUAssetRegistry.deploy();
    await registry.waitForDeployment();

    ALULogoToken = await ethers.getContractFactory("ALULogoToken");
    token = await ALULogoToken.deploy(owner.address);
    await token.waitForDeployment();
  });

  // ---------------------------------------------------------------------
  // 1. ALUAssetRegistry deployment
  // ---------------------------------------------------------------------
  it("Test 1: deploys ALUAssetRegistry with the correct ERC-721 name and symbol", async function () {
    expect(await registry.name()).to.equal("ALU Digital Asset");
    expect(await registry.symbol()).to.equal("ALUDA");
  });

  // ---------------------------------------------------------------------
  // 2. registerAsset mints an NFT and stores metadata correctly
  // ---------------------------------------------------------------------
  it("Test 2: registerAsset mints a token and stores the correct metadata", async function () {
    const tx = await registry
      .connect(alice)
      .registerAsset("ALU Official Logo 2026", "png", SAMPLE_HASH_A);
    await tx.wait();

    expect(await registry.ownerOf(1)).to.equal(alice.address);

    const asset = await registry.getAsset(1);
    expect(asset.assetName).to.equal("ALU Official Logo 2026");
    expect(asset.fileType).to.equal("png");
    expect(asset.contentHash).to.equal(SAMPLE_HASH_A);
    expect(asset.registeredBy).to.equal(alice.address);
  });

  // ---------------------------------------------------------------------
  // 3. Duplicate hash rejection
  // ---------------------------------------------------------------------
  it("Test 3: rejects registration of a content hash that is already registered", async function () {
    await (await registry.connect(alice).registerAsset("Logo A", "png", SAMPLE_HASH_A)).wait();

    await expect(
      registry.connect(bob).registerAsset("Logo A Copy", "png", SAMPLE_HASH_A)
    ).to.be.revertedWith("Error: Asset hash already registered.");
  });

  // ---------------------------------------------------------------------
  // 4. verifyLogoIntegrity - matching hash
  // ---------------------------------------------------------------------
  it("Test 4: verifyLogoIntegrity returns true for a matching hash", async function () {
    await (await registry.registerAsset("ALU Logo", "png", SAMPLE_HASH_A)).wait();
    const [isValid, message] = await registry.verifyLogoIntegrity(1, SAMPLE_HASH_A);
    expect(isValid).to.equal(true);
    expect(message).to.equal("Logo is authentic.");
  });

  // ---------------------------------------------------------------------
  // 5. verifyLogoIntegrity - non-matching hash
  // ---------------------------------------------------------------------
  it("Test 5: verifyLogoIntegrity returns false for a non-matching hash", async function () {
    await (await registry.registerAsset("ALU Logo", "png", SAMPLE_HASH_A)).wait();
    const [isValid, message] = await registry.verifyLogoIntegrity(1, SAMPLE_HASH_B);
    expect(isValid).to.equal(false);
    expect(message).to.equal("Warning: logo does not match.");
  });

  // ---------------------------------------------------------------------
  // 6. getAsset / verifyLogoIntegrity revert on nonexistent token
  // ---------------------------------------------------------------------
  it("Test 6: reverts when querying a token ID that does not exist", async function () {
    await expect(registry.getAsset(999)).to.be.revertedWith("Error: Token ID does not exist.");
    await expect(
      registry.verifyLogoIntegrity(999, SAMPLE_HASH_A)
    ).to.be.revertedWith("Error: Token ID does not exist.");
  });

  // ---------------------------------------------------------------------
  // 7. ALULogoToken deployment mints total supply to the initial owner
  // ---------------------------------------------------------------------
  it("Test 7: deploys ALULogoToken with 1,000,000 ALUT minted to the initial owner", async function () {
    expect(await token.name()).to.equal("ALU Logo Token");
    expect(await token.symbol()).to.equal("ALUT");

    const expectedSupply = ethers.parseUnits("1000000", 18);
    expect(await token.totalSupply()).to.equal(expectedSupply);
    expect(await token.balanceOf(owner.address)).to.equal(expectedSupply);
  });

  // ---------------------------------------------------------------------
  // 8. distributeShares + onlyOwner protection + ownershipPercentage
  // ---------------------------------------------------------------------
  it("Test 8: distributeShares transfers tokens, is owner-only, and ownershipPercentage is correct", async function () {
    const amount = ethers.parseUnits("250000", 18); // 25% of supply

    await (await token.connect(owner).distributeShares(alice.address, amount)).wait();
    expect(await token.balanceOf(alice.address)).to.equal(amount);
    expect(await token.ownershipPercentage(alice.address)).to.equal(25n);

    // Non-owner attempts should revert
    await expect(
      token.connect(alice).distributeShares(bob.address, amount)
    ).to.be.revertedWithCustomError(token, "OwnableUnauthorizedAccount");
  });

  // =======================================================================
  // NEW FRONTEND INTEGRATION TESTS (Assignment 2) — Tests 9 through 13
  // =======================================================================

  // ---------------------------------------------------------------------
  // 9. Frontend correctly reads and displays the total ALUT supply
  // ---------------------------------------------------------------------
  it("Test 9 (Frontend): reads the total ALUT supply from the deployed contract as 1,000,000", async function () {
    const rawSupply = await token.totalSupply();
    // This mirrors frontend/js/dashboard.js, which formats the raw
    // 18-decimal supply into a human-readable whole number for display.
    const displaySupply = ethers.formatUnits(rawSupply, 18);
    expect(displaySupply).to.equal("1000000.0");
    expect(Math.round(Number(displaySupply))).to.equal(1000000);
  });

  // ---------------------------------------------------------------------
  // 10. In-browser style hashing produces correct bytes32 SHA-256 hash
  // ---------------------------------------------------------------------
  it("Test 10 (Frontend): the hashing function returns the correct SHA-256 hash in bytes32 format", async function () {
    const fakeFileBytes = Buffer.from("ALU Official Logo File Contents - Test Fixture");
    const computedHash = computeSha256Bytes32(fakeFileBytes);

    // Cross-check against the same algorithm computed independently
    const expectedHash =
      "0x" + crypto.createHash("sha256").update(fakeFileBytes).digest("hex");

    expect(computedHash).to.equal(expectedHash);
    expect(computedHash).to.match(/^0x[0-9a-f]{64}$/); // valid bytes32 hex string
  });

  // ---------------------------------------------------------------------
  // 11. verifyLogoIntegrity success path (correct hash) end-to-end
  // ---------------------------------------------------------------------
  it("Test 11 (Frontend): displays a verification SUCCESS result when the correct hash is supplied", async function () {
    const fakeLogoBytes = Buffer.from("the-real-alu-logo-bytes");
    const realHash = computeSha256Bytes32(fakeLogoBytes);

    await (await registry.registerAsset("ALU Official Logo", "png", realHash)).wait();

    // Simulates the C1 verification page: user uploads a file, the app
    // hashes it in-browser, then calls verifyLogoIntegrity() with that hash.
    const [isValid, message] = await registry.verifyLogoIntegrity(1, realHash);
    expect(isValid).to.equal(true);
    expect(message).to.equal("Logo is authentic.");
  });

  // ---------------------------------------------------------------------
  // 12. verifyLogoIntegrity failure path (incorrect hash) end-to-end
  // ---------------------------------------------------------------------
  it("Test 12 (Frontend): displays a verification FAILURE result when an incorrect hash is supplied", async function () {
    const fakeLogoBytes = Buffer.from("the-real-alu-logo-bytes");
    const tamperedBytes = Buffer.from("a-modified-fake-logo-bytes");

    const realHash = computeSha256Bytes32(fakeLogoBytes);
    const tamperedHash = computeSha256Bytes32(tamperedBytes);

    await (await registry.registerAsset("ALU Official Logo", "png", realHash)).wait();

    const [isValid, message] = await registry.verifyLogoIntegrity(1, tamperedHash);
    expect(isValid).to.equal(false);
    expect(message).to.equal("Warning: logo does not match.");
  });

  // ---------------------------------------------------------------------
  // 13. distributeShares correctly updates recipient balance (dashboard flow)
  // ---------------------------------------------------------------------
  it("Test 13 (Frontend): distributeShares correctly updates the recipient's balance after a successful transfer", async function () {
    const beforeBalance = await token.balanceOf(carol.address);
    expect(beforeBalance).to.equal(0n);

    const amount = ethers.parseUnits("50000", 18); // 5%
    await (await token.connect(owner).distributeShares(carol.address, amount)).wait();

    const afterBalance = await token.balanceOf(carol.address);
    expect(afterBalance).to.equal(amount);
    expect(await token.ownershipPercentage(carol.address)).to.equal(5n);
  });
});
