const hre = require("hardhat");

/**
 * Verifies the deployed UjuziChain contract on Etherscan (Sepolia).
 *
 * Usage:
 *   CONTRACT_ADDRESS=0xYourDeployedAddress npx hardhat run scripts/verify.js --network sepolia
 *
 * The contract takes no constructor arguments, so constructorArguments is empty.
 */
async function main() {
  const contractAddress = process.env.CONTRACT_ADDRESS;

  if (!contractAddress) {
    throw new Error(
      "Please set CONTRACT_ADDRESS env variable to the deployed contract's address before running this script."
    );
  }

  console.log(`Verifying UjuziChain at ${contractAddress} on Sepolia...`);

  await hre.run("verify:verify", {
    address: contractAddress,
    constructorArguments: []
  });

  console.log("Verification complete.");
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
