// wallet.js — Part A1/A2: wallet connection + contract instances
// A single global namespace (window.ALU) is used so the other small script
// files (hash.js, register.js, verify.js, dashboard.js) can share state
// without a bundler/module system, matching the "plain HTML/CSS/JS" stack
// option offered in the assignment brief.

window.ALU = window.ALU || {};

(function () {
  const cfg = window.ALU_CONTRACT_CONFIG;

  ALU.provider = null;
  ALU.signer = null;
  ALU.address = null;
  ALU.registry = null; // ALUAssetRegistry contract (with signer, for write calls)
  ALU.token = null; // ALULogoToken contract (with signer)
  ALU.readOnlyProvider = null; // for the public verify page (no wallet required)
  ALU.readOnlyRegistry = null;

  function short(addr) {
    return addr ? addr.slice(0, 6) + "..." + addr.slice(-4) : "";
  }
  ALU.shortAddress = short;

  function showWalletWarning(message) {
    const box = document.getElementById("wallet-warning");
    if (!message) {
      box.classList.remove("show");
      box.textContent = "";
      return;
    }
    box.textContent = message;
    box.classList.add("show");
  }

  // ---------------------------------------------------------------------
  // Read-only provider: lets the public Verify page work with NO wallet.
  // Falls back to the injected wallet's provider (read-only) if present,
  // otherwise talks directly to the local Hardhat node.
  // ---------------------------------------------------------------------
  function initReadOnly() {
    try {
      if (window.ethereum) {
        ALU.readOnlyProvider = new ethers.BrowserProvider(window.ethereum);
      } else {
        ALU.readOnlyProvider = new ethers.JsonRpcProvider("http://127.0.0.1:8545");
      }
      ALU.readOnlyRegistry = new ethers.Contract(
        cfg.aluAssetRegistry.address,
        cfg.aluAssetRegistry.abi,
        ALU.readOnlyProvider
      );
    } catch (e) {
      console.warn("Read-only provider init failed:", e);
    }
  }

  async function connectWallet() {
    if (!window.ethereum) {
      showWalletWarning(
        "No Web3 wallet detected. Please install MetaMask (or another Web3 wallet) to register assets or distribute shares. You can still use the public Verify page without a wallet."
      );
      return;
    }
    showWalletWarning(null);

    try {
      ALU.provider = new ethers.BrowserProvider(window.ethereum);
      const accounts = await ALU.provider.send("eth_requestAccounts", []);
      ALU.address = accounts[0];
      ALU.signer = await ALU.provider.getSigner();

      ALU.registry = new ethers.Contract(
        cfg.aluAssetRegistry.address,
        cfg.aluAssetRegistry.abi,
        ALU.signer
      );
      ALU.token = new ethers.Contract(
        cfg.aluLogoToken.address,
        cfg.aluLogoToken.abi,
        ALU.signer
      );

      await refreshWalletUI();
      window.dispatchEvent(new CustomEvent("alu:walletConnected"));
    } catch (err) {
      console.error(err);
      showWalletWarning("Wallet connection was rejected or failed: " + (err.message || err));
    }
  }

  function disconnectWallet() {
    ALU.provider = null;
    ALU.signer = null;
    ALU.address = null;
    ALU.registry = null;
    ALU.token = null;
    document.getElementById("connect-btn").style.display = "inline-block";
    document.getElementById("disconnect-btn").style.display = "none";
    document.getElementById("address-pill").style.display = "none";
    document.getElementById("balance-pill").style.display = "none";
    document.getElementById("network-pill").style.display = "none";
    window.dispatchEvent(new CustomEvent("alu:walletDisconnected"));
  }

  async function refreshWalletUI() {
    if (!ALU.address) return;

    const addressPill = document.getElementById("address-pill");
    const balancePill = document.getElementById("balance-pill");
    const networkPill = document.getElementById("network-pill");

    addressPill.textContent = short(ALU.address);
    addressPill.style.display = "inline-block";

    try {
      const balance = await ALU.token.balanceOf(ALU.address);
      balancePill.textContent = ethers.formatUnits(balance, 18) + " ALUT";
      balancePill.style.display = "inline-block";
    } catch (e) {
      balancePill.textContent = "ALUT balance unavailable";
      balancePill.style.display = "inline-block";
    }

    try {
      const network = await ALU.provider.getNetwork();
      networkPill.textContent = "Chain ID " + network.chainId.toString();
      networkPill.style.display = "inline-block";
    } catch (e) {
      /* ignore */
    }

    document.getElementById("connect-btn").style.display = "none";
    document.getElementById("disconnect-btn").style.display = "inline-block";
  }

  // Handle account switching / network switching gracefully (A1 requirement)
  function attachProviderEvents() {
    if (!window.ethereum) return;
    window.ethereum.on("accountsChanged", async (accounts) => {
      if (accounts.length === 0) {
        disconnectWallet();
      } else {
        await connectWallet();
      }
    });
    window.ethereum.on("chainChanged", () => {
      // Safest approach on a chain switch is to reload contract state.
      window.location.reload();
    });
  }

  document.addEventListener("DOMContentLoaded", () => {
    initReadOnly();
    attachProviderEvents();
    document.getElementById("connect-btn").addEventListener("click", connectWallet);
    document.getElementById("disconnect-btn").addEventListener("click", disconnectWallet);

    if (cfg.aluAssetRegistry.address === "0x0000000000000000000000000000000000000000") {
      showWalletWarning(
        "Contracts are not deployed yet. Run `npm run deploy:local` after starting a local Hardhat node, then reload this page."
      );
    }
  });

  ALU.connectWallet = connectWallet;
  ALU.disconnectWallet = disconnectWallet;
  ALU.refreshWalletUI = refreshWalletUI;
})();
