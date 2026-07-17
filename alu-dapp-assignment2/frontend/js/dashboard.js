// dashboard.js — Part D: ownership overview + owner-only distribution form

(function () {
  // A handful of example addresses to illustrate the ownership breakdown
  // (D1: "A list of at least three example wallet addresses and their
  // ownership percentages"). These are the well-known default Hardhat
  // test accounts #1-#3, useful for local demoing.
  const EXAMPLE_HOLDERS = [
    "0x70997970C51812dc3A010C7d01b50e0d17dc79C",
    "0x3C44CdDdB6a900fa2b585dd299e03d12FA4293BC",
    "0x90F79bf6EB2c4f870365E785982E1f101E93b906"
  ];

  function setStatus(el, type, message) {
    el.className = "status-box " + type + " show";
    el.textContent = message;
  }
  function clearStatus(el) {
    el.className = "status-box";
    el.textContent = "";
  }

  async function loadOverview() {
    const cfg = window.ALU_CONTRACT_CONFIG;
    const readToken = new ethers.Contract(
      cfg.aluLogoToken.address,
      cfg.aluLogoToken.abi,
      ALU.readOnlyProvider
    );

    try {
      const totalSupply = await readToken.totalSupply();
      document.getElementById("stat-total-supply").textContent =
        Number(ethers.formatUnits(totalSupply, 18)).toLocaleString() + " ALUT";
    } catch (e) {
      document.getElementById("stat-total-supply").textContent = "N/A";
    }

    if (ALU.address && ALU.token) {
      try {
        const bal = await ALU.token.balanceOf(ALU.address);
        const pct = await ALU.token.ownershipPercentage(ALU.address);
        document.getElementById("stat-my-balance").textContent =
          Number(ethers.formatUnits(bal, 18)).toLocaleString() + " ALUT";
        document.getElementById("stat-my-pct").textContent = pct.toString() + "%";
      } catch (e) {
        document.getElementById("stat-my-balance").textContent = "N/A";
        document.getElementById("stat-my-pct").textContent = "N/A";
      }

      // Owner-only distribution form visibility
      try {
        const ownerAddress = await readToken.owner();
        const isOwner = ownerAddress.toLowerCase() === ALU.address.toLowerCase();
        document.getElementById("distribute-card").style.display = isOwner ? "block" : "none";
        document.getElementById("not-owner-card").style.display = isOwner ? "none" : "block";
      } catch (e) {
        /* ignore */
      }
    } else {
      document.getElementById("stat-my-balance").textContent = "—";
      document.getElementById("stat-my-pct").textContent = "—";
      document.getElementById("distribute-card").style.display = "none";
      document.getElementById("not-owner-card").style.display = "block";
    }

    // Example holders table
    const tbody = document.getElementById("holders-table-body");
    tbody.innerHTML = "";
    for (const addr of EXAMPLE_HOLDERS) {
      let balText = "—", pctText = "—";
      try {
        const bal = await readToken.balanceOf(addr);
        const pct = await readToken.ownershipPercentage(addr);
        balText = Number(ethers.formatUnits(bal, 18)).toLocaleString() + " ALUT";
        pctText = pct.toString() + "%";
      } catch (e) {
        /* leave as dashes if the read fails (e.g. contracts not deployed yet) */
      }
      const row = document.createElement("tr");
      row.innerHTML =
        "<td>" + ALU.shortAddress(addr) + "</td><td>" + balText + "</td><td>" + pctText + "</td>";
      tbody.appendChild(row);
    }
  }

  document.addEventListener("DOMContentLoaded", () => {
    window.addEventListener("alu:walletConnected", loadOverview);
    window.addEventListener("alu:walletDisconnected", loadOverview);

    // Refresh dashboard data whenever its tab becomes active
    document.querySelector('[data-tab="dashboard"]').addEventListener("click", loadOverview);

    const submitBtn = document.getElementById("dist-submit");
    const statusBox = document.getElementById("dist-status");

    submitBtn.addEventListener("click", async () => {
      clearStatus(statusBox);

      if (!ALU.token || !ALU.address) {
        setStatus(statusBox, "error", "Please connect the owner wallet first.");
        return;
      }

      const recipient = document.getElementById("dist-address").value.trim();
      const amountStr = document.getElementById("dist-amount").value.trim();
      const amountNum = Number(amountStr);

      if (!ethers.isAddress(recipient)) {
        setStatus(statusBox, "error", "Please enter a valid recipient wallet address.");
        return;
      }
      if (!amountStr || amountNum <= 0) {
        setStatus(statusBox, "error", "Amount must be greater than zero.");
        return;
      }

      try {
        submitBtn.disabled = true;

        const amountWei = ethers.parseUnits(amountStr, 18);
        const myBalance = await ALU.token.balanceOf(ALU.address);
        if (myBalance < amountWei) {
          setStatus(statusBox, "error", "Insufficient balance: you do not hold enough ALUT for this distribution.");
          submitBtn.disabled = false;
          return;
        }

        setStatus(statusBox, "info", "Please approve the transaction in your wallet…");
        const tx = await ALU.token.distributeShares(recipient, amountWei);
        setStatus(statusBox, "info", "Transaction submitted. Waiting for confirmation…");
        await tx.wait();

        setStatus(statusBox, "success", "Success! " + amountStr + " ALUT sent to " + ALU.shortAddress(recipient) + ".");
        await ALU.refreshWalletUI();
        await loadOverview();
      } catch (err) {
        console.error(err);
        const reason = (err && err.reason) || (err && err.shortMessage) || (err && err.message) || "Transaction failed.";
        if (reason.includes("OwnableUnauthorizedAccount") || reason.toLowerCase().includes("owner")) {
          setStatus(statusBox, "error", "Only the contract owner wallet can distribute shares.");
        } else {
          setStatus(statusBox, "error", "Error: " + reason);
        }
      } finally {
        submitBtn.disabled = false;
      }
    });
  });
})();
