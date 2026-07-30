const { expect } = require("chai");
const { ethers } = require("hardhat");
const { anyValue } = require("@nomicfoundation/hardhat-chai-matchers/withArgs");

describe("UjuziChain", function () {
  let ujuziChain;
  let admin, institution1, institution2, student1, student2, employer, stranger;

  beforeEach(async function () {
    [admin, institution1, institution2, student1, student2, employer, stranger] =
      await ethers.getSigners();

    const UjuziChain = await ethers.getContractFactory("UjuziChain");
    ujuziChain = await UjuziChain.connect(admin).deploy();
    await ujuziChain.waitForDeployment();
  });

  // -------------------------------------------------------------------
  // Institution Registration
  // -------------------------------------------------------------------
  describe("Institution Registration", function () {
    it("allows admin to register an institution", async function () {
      await expect(
        ujuziChain.connect(admin).registerInstitution(institution1.address, "University of Nairobi")
      )
        .to.emit(ujuziChain, "InstitutionRegistered")
        .withArgs(institution1.address, "University of Nairobi", anyValue);

      expect(await ujuziChain.isInstitutionRegistered(institution1.address)).to.equal(true);
    });

    it("rejects registration from a non-admin account", async function () {
      await expect(
        ujuziChain.connect(stranger).registerInstitution(institution1.address, "Fake University")
      ).to.be.revertedWithCustomError(ujuziChain, "OwnableUnauthorizedAccount");
    });

    it("rejects duplicate registration of the same institution", async function () {
      await ujuziChain.connect(admin).registerInstitution(institution1.address, "Makerere University");
      await expect(
        ujuziChain.connect(admin).registerInstitution(institution1.address, "Makerere University")
      ).to.be.revertedWith("UjuziChain: already registered");
    });

    it("rejects registration with an empty name", async function () {
      await expect(
        ujuziChain.connect(admin).registerInstitution(institution1.address, "")
      ).to.be.revertedWith("UjuziChain: name required");
    });

    it("rejects registration with the zero address", async function () {
      await expect(
        ujuziChain.connect(admin).registerInstitution(ethers.ZeroAddress, "Ghost University")
      ).to.be.revertedWith("UjuziChain: zero address");
    });

    it("allows admin to remove a registered institution", async function () {
      await ujuziChain.connect(admin).registerInstitution(institution1.address, "Strathmore University");
      await expect(ujuziChain.connect(admin).removeInstitution(institution1.address))
        .to.emit(ujuziChain, "InstitutionRemoved")
        .withArgs(institution1.address, anyValue);

      expect(await ujuziChain.isInstitutionRegistered(institution1.address)).to.equal(false);
    });

    it("rejects removing an institution that was never registered", async function () {
      await expect(
        ujuziChain.connect(admin).removeInstitution(institution1.address)
      ).to.be.revertedWith("UjuziChain: not registered");
    });
  });

  // -------------------------------------------------------------------
  // Credential Issuance
  // -------------------------------------------------------------------
  describe("Credential Issuance", function () {
    beforeEach(async function () {
      await ujuziChain.connect(admin).registerInstitution(institution1.address, "University of Nairobi");
    });

    it("allows a registered institution to issue a credential", async function () {
      await expect(
        ujuziChain
          .connect(institution1)
          .issueCredential(
            student1.address,
            "Amina Wanjiru",
            "BSc Computer Science",
            "Distributed Systems",
            "ipfs://QmExampleHash"
          )
      )
        .to.emit(ujuziChain, "CredentialIssued")
        .withArgs(1, student1.address, institution1.address, "BSc Computer Science", anyValue);

      expect(await ujuziChain.totalCredentialsIssued()).to.equal(1);
    });

    it("rejects issuance from an unregistered institution", async function () {
      await expect(
        ujuziChain
          .connect(institution2)
          .issueCredential(student1.address, "Amina Wanjiru", "BSc CS", "DS", "ipfs://x")
      ).to.be.revertedWith("UjuziChain: caller is not a registered institution");
    });

    it("rejects issuance to the zero address", async function () {
      await expect(
        ujuziChain
          .connect(institution1)
          .issueCredential(ethers.ZeroAddress, "Ghost Student", "BSc CS", "DS", "ipfs://x")
      ).to.be.revertedWith("UjuziChain: zero address");
    });

    it("rejects issuance with an empty student name", async function () {
      await expect(
        ujuziChain.connect(institution1).issueCredential(student1.address, "", "BSc CS", "DS", "ipfs://x")
      ).to.be.revertedWith("UjuziChain: student name required");
    });

    it("tracks multiple credentials per student", async function () {
      await ujuziChain
        .connect(institution1)
        .issueCredential(student1.address, "Amina Wanjiru", "BSc CS", "DS", "ipfs://a");
      await ujuziChain
        .connect(institution1)
        .issueCredential(student1.address, "Amina Wanjiru", "Postgrad Diploma", "AI", "ipfs://b");

      const ids = await ujuziChain.getCredentialsByStudent(student1.address);
      expect(ids.length).to.equal(2);
      expect(ids[0]).to.equal(1);
      expect(ids[1]).to.equal(2);
    });

    it("increments credential IDs sequentially across institutions", async function () {
      await ujuziChain.connect(admin).registerInstitution(institution2.address, "Kenyatta University");

      await ujuziChain
        .connect(institution1)
        .issueCredential(student1.address, "Amina Wanjiru", "BSc CS", "DS", "ipfs://a");
      await ujuziChain
        .connect(institution2)
        .issueCredential(student2.address, "Brian Otieno", "BCom", "Finance", "ipfs://b");

      expect(await ujuziChain.totalCredentialsIssued()).to.equal(2);
    });
  });

  // -------------------------------------------------------------------
  // Credential Revocation
  // -------------------------------------------------------------------
  describe("Credential Revocation", function () {
    beforeEach(async function () {
      await ujuziChain.connect(admin).registerInstitution(institution1.address, "University of Nairobi");
      await ujuziChain.connect(admin).registerInstitution(institution2.address, "Kenyatta University");
      await ujuziChain
        .connect(institution1)
        .issueCredential(student1.address, "Amina Wanjiru", "BSc CS", "DS", "ipfs://a");
    });

    it("allows the issuing institution to revoke its own credential", async function () {
      await expect(ujuziChain.connect(institution1).revokeCredential(1))
        .to.emit(ujuziChain, "CredentialRevoked")
        .withArgs(1, institution1.address, anyValue);

      const result = await ujuziChain.verifyCredential(1);
      expect(result.revoked).to.equal(true);
    });

    it("rejects revocation from a different institution", async function () {
      await expect(
        ujuziChain.connect(institution2).revokeCredential(1)
      ).to.be.revertedWith("UjuziChain: not the issuing institution");
    });

    it("rejects revocation from a non-institution account", async function () {
      await expect(
        ujuziChain.connect(stranger).revokeCredential(1)
      ).to.be.revertedWith("UjuziChain: caller is not a registered institution");
    });

    it("rejects double revocation", async function () {
      await ujuziChain.connect(institution1).revokeCredential(1);
      await expect(
        ujuziChain.connect(institution1).revokeCredential(1)
      ).to.be.revertedWith("UjuziChain: already revoked");
    });

    it("rejects revocation of a non-existent credential", async function () {
      await expect(
        ujuziChain.connect(institution1).revokeCredential(999)
      ).to.be.revertedWith("UjuziChain: credential does not exist");
    });
  });

  // -------------------------------------------------------------------
  // Metadata Updates
  // -------------------------------------------------------------------
  describe("Metadata Updates", function () {
    beforeEach(async function () {
      await ujuziChain.connect(admin).registerInstitution(institution1.address, "University of Nairobi");
      await ujuziChain
        .connect(institution1)
        .issueCredential(student1.address, "Amina Wanjiru", "BSc CS", "DS", "ipfs://old");
    });

    it("allows the issuing institution to update metadata URI", async function () {
      await expect(ujuziChain.connect(institution1).updateMetadataURI(1, "ipfs://new"))
        .to.emit(ujuziChain, "CredentialMetadataUpdated")
        .withArgs(1, "ipfs://new");

      const result = await ujuziChain.verifyCredential(1);
      expect(result.metadataURI).to.equal("ipfs://new");
    });

    it("rejects metadata update from a non-issuing institution", async function () {
      await ujuziChain.connect(admin).registerInstitution(institution2.address, "Kenyatta University");
      await expect(
        ujuziChain.connect(institution2).updateMetadataURI(1, "ipfs://new")
      ).to.be.revertedWith("UjuziChain: not the issuing institution");
    });
  });

  // -------------------------------------------------------------------
  // Verification (Employer-facing)
  // -------------------------------------------------------------------
  describe("Credential Verification", function () {
    beforeEach(async function () {
      await ujuziChain.connect(admin).registerInstitution(institution1.address, "University of Nairobi");
      await ujuziChain
        .connect(institution1)
        .issueCredential(student1.address, "Amina Wanjiru", "BSc CS", "DS", "ipfs://a");
    });

    it("allows anyone (including employers) to verify a credential", async function () {
      const result = await ujuziChain.connect(employer).verifyCredential(1);

      expect(result.student).to.equal(student1.address);
      expect(result.studentName).to.equal("Amina Wanjiru");
      expect(result.institutionName).to.equal("University of Nairobi");
      expect(result.revoked).to.equal(false);
      expect(result.institutionCurrentlyRegistered).to.equal(true);
    });

    it("reflects institution removal without altering past credential validity records", async function () {
      await ujuziChain.connect(admin).removeInstitution(institution1.address);
      const result = await ujuziChain.verifyCredential(1);

      // Credential itself is untouched, but flag shows institution no longer active
      expect(result.revoked).to.equal(false);
      expect(result.institutionCurrentlyRegistered).to.equal(false);
    });

    it("reverts when verifying a non-existent credential", async function () {
      await expect(ujuziChain.verifyCredential(42)).to.be.revertedWith(
        "UjuziChain: credential does not exist"
      );
    });
  });

  // -------------------------------------------------------------------
  // Edge Cases
  // -------------------------------------------------------------------
  describe("Edge Cases", function () {
    it("returns an empty array for a student with no credentials", async function () {
      const ids = await ujuziChain.getCredentialsByStudent(student1.address);
      expect(ids.length).to.equal(0);
    });

    it("returns zero for totalCredentialsIssued on a fresh contract", async function () {
      expect(await ujuziChain.totalCredentialsIssued()).to.equal(0);
    });

    it("allows ownership transfer and enforces new owner's exclusive admin rights", async function () {
      await ujuziChain.connect(admin).transferOwnership(institution1.address);
      await expect(
        ujuziChain.connect(admin).registerInstitution(institution2.address, "Should Fail Uni")
      ).to.be.revertedWithCustomError(ujuziChain, "OwnableUnauthorizedAccount");

      await expect(
        ujuziChain.connect(institution1).registerInstitution(institution2.address, "New Admin Uni")
      ).to.not.be.reverted;
    });
  });
});

