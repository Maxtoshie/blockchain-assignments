// register.js — Part B1 (upload/hash/preview) + Part B2 (registerAsset tx)

(function () {
  let currentHash = null;

  function setStatus(el, type, message) {
    el.className = "status-box " + type + " show";
    el.textContent = message;
  }

  function clearStatus(el) {
    el.className = "status-box";
    el.textContent = "";
  }

  document.addEventListener("DOMContentLoaded", () => {
    const fileInput = document.getElementById("reg-file");
    const preview = document.getElementById("reg-preview");
    const hashBox = document.getElementById("reg-hash");
    const submitBtn = document.getElementById("reg-submit");
    const statusBox = document.getElementById("reg-status");

    fileInput.addEventListener("change", async () => {
      const file = fileInput.files[0];
      currentHash = null;
      if (!file) {
        hashBox.textContent = "No file selected yet.";
        preview.style.display = "none";
        return;
      }

      // Show image preview (B1: "so the user can confirm they selected the right file")
      const reader = new FileReader();
      reader.onload = (e) => {
        preview.src = e.target.result;
        preview.style.display = "block";
      };
      reader.readAsDataURL(file);

      hashBox.textContent = "Computing SHA-256 hash…";
      try {
        currentHash = await ALU.hashFileToBytes32(file);
        hashBox.textContent = currentHash;
      } catch (err) {
        hashBox.textContent = "Failed to hash file: " + err.message;
      }
    });

    submitBtn.addEventListener("click", async () => {
      clearStatus(statusBox);

      if (!ALU.registry || !ALU.address) {
        setStatus(statusBox, "error", "Please connect your wallet first.");
        return;
      }
      if (!currentHash) {
        setStatus(statusBox, "error", "Please select and hash a file first.");
        return;
      }
      const name = document.getElementById("reg-name").value.trim();
      const fileType = document.getElementById("reg-type").value.trim();
      if (!name || !fileType) {
        setStatus(statusBox, "error", "Please provide both an asset name and a file type.");
        return;
      }

      try {
        submitBtn.disabled = true;
        setStatus(statusBox, "info", "Please approve the transaction in your wallet…");

        const tx = await ALU.registry.registerAsset(name, fileType, currentHash);
        setStatus(statusBox, "info", "Transaction submitted. Waiting for confirmation…");

        const receipt = await tx.wait();

        // Pull the new token ID out of the AssetRegistered event
        let newTokenId = "unknown";
        for (const log of receipt.logs) {
          try {
            const parsed = ALU.registry.interface.parseLog(log);
            if (parsed && parsed.name === "AssetRegistered") {
              newTokenId = parsed.args.tokenId.toString();
            }
          } catch (e) {
            /* not our event, ignore */
          }
        }

        setStatus(
          statusBox,
          "success",
          "Success! Asset registered on-chain with token ID #" + newTokenId + "."
        );
        await ALU.refreshWalletUI();
      } catch (err) {
        console.error(err);
        const reason =
          (err && err.reason) ||
          (err && err.shortMessage) ||
          (err && err.message) ||
          "Transaction failed.";
        if (reason.includes("already registered")) {
          setStatus(
            statusBox,
            "error",
            "This exact file has already been registered on-chain (duplicate hash rejected)."
          );
        } else {
          setStatus(statusBox, "error", "Error: " + reason);
        }
      } finally {
        submitBtn.disabled = false;
      }
    });
  });
})();
