const { ethers } = require("hardhat");

async function main() {
  // Get the deployer wallet address (Account #0)
  const [deployer] = await ethers.getSigners();
  console.log("Deploying contracts with the account:", deployer.address);

  // 1. Deploy ALUAssetRegistry
  const ALUAssetRegistry = await ethers.getContractFactory("ALUAssetRegistry");
  const assetRegistry = await ALUAssetRegistry.deploy();
  await assetRegistry.waitForDeployment();
  const registryAddress = await assetRegistry.getAddress();
  console.log("ALUAssetRegistry deployed to:", registryAddress);

  // 2. Register the ALU Logo immediately as part of deployment
  // REPLACE THIS HEX STRING WITH THE ACTUAL HASH GENERATED FROM YOUR STEP ABOVE!
  const aluLogoHash = "0x7f83b1657ff1fc53b92cb1813021181302118130211813021181302118130211"; 
  
  console.log("Registering ALU Logo asset...");
  const tx = await assetRegistry.registerAsset(
    "ALU Official Logo",
    "webp",
    aluLogoHash
  );
  await tx.wait();
  console.log("ALU Logo successfully registered in the asset registry!");

  // 3. Deploy ALULogoToken, passing the deployer as the initial owner
  const ALULogoToken = await ethers.getContractFactory("ALULogoToken");
  const logoToken = await ALULogoToken.deploy(deployer.address);
  await logoToken.waitForDeployment();
  const tokenAddress = await logoToken.getAddress();
  console.log("ALULogoToken deployed to:", tokenAddress);
  console.log("1,000,000 ALUT tokens minted to:", deployer.address);
}

main()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error);
    process.exit(1);
  });
