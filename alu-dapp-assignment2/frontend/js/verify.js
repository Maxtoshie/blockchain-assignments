// verify.js — Part C: public logo verification, no wallet connection needed.
// verifyLogoIntegrity() is a `view` function, so it costs no gas and needs
// no signature — a plain read-only provider is enough to call it.

(function () {
  function renderResult(isValid, message, meta) {
    const result = document.getElementById("verify-result");
    const icon = document.getElementById("verify-icon");
    const headline = document.getElementById("verify-headline");
    const subline = document.getElementById("verify-subline");
    const metaBox = document.getElementById("verify-meta");

    result.classList.remove("authentic", "fake");
    result.classList.add("show", isValid ? "authentic" : "fake");

    icon.textContent = isValid ? "✅" : "⚠️";
    headline.textContent = isValid
      ? "Logo Verified: This is the authentic ALU logo"
      : "Warning: This logo has been modified.";
    subline.textContent = message || "";

    if (isValid && meta) {
      metaBox.innerHTML =
        "<strong>Asset name:</strong> " + escapeHtml(meta.assetName) + "<br>" +
        "<strong>Registered:</strong> " + new Date(Number(meta.timestamp) * 1000).toLocaleString() + "<br>" +
        "<strong>Registered by:</strong> " + meta.registeredBy;
    } else {
      metaBox.innerHTML = "";
    }
  }

  function escapeHtml(str) {
    const div = document.createElement("div");
    div.textContent = str;
    return div.innerHTML;
  }

  async function runVerification(tokenId, hash, statusFallbackEl) {
    if (!ALU.readOnlyRegistry) {
      alert("Verification service is not ready yet. Please reload the page.");
      return;
    }
    try {
      const [isValid, message] = await ALU.readOnlyRegistry.verifyLogoIntegrity(tokenId, hash);
      let meta = null;
      if (isValid) {
        meta = await ALU.readOnlyRegistry.getAsset(tokenId);
      }
      renderResult(isValid, message, meta);
    } catch (err) {
      console.error(err);
      const reason = (err && err.reason) || (err && err.shortMessage) || (err && err.message) || "Unknown error";
      renderResult(false, "Could not verify: " + reason, null);
    }
  }

  document.addEventListener("DOMContentLoaded", () => {
    const fileBtn = document.getElementById("verify-file-btn");
    const hashBtn = document.getElementById("verify-hash-btn");

    fileBtn.addEventListener("click", async () => {
      const fileInput = document.getElementById("verify-file");
      const tokenId = document.getElementById("verify-tokenid").value;
      const file = fileInput.files[0];
      if (!file) {
        alert("Please choose a file to verify.");
        return;
      }
      fileBtn.disabled = true;
      fileBtn.textContent = "Hashing file…";
      try {
        const hash = await ALU.hashFileToBytes32(file);
        await runVerification(tokenId, hash);
      } finally {
        fileBtn.disabled = false;
        fileBtn.textContent = "Verify File";
      }
    });

    hashBtn.addEventListener("click", async () => {
      const tokenId = document.getElementById("verify-tokenid-2").value;
      const hash = document.getElementById("verify-hash-input").value.trim();
      if (!hash || !hash.startsWith("0x")) {
        alert("Please paste a valid 0x-prefixed SHA-256 hash.");
        return;
      }
      await runVerification(tokenId, hash);
    });
  });
})();
