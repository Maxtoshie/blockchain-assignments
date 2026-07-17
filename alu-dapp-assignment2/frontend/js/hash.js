// hash.js — Part B1: in-browser SHA-256 hashing using the Web Crypto API.
// No file is ever sent to a server; everything happens locally in the
// browser sandbox.

window.ALU = window.ALU || {};

(function () {
  /**
   * Reads a File object and returns its SHA-256 hash formatted as a
   * bytes32 hex string (0x-prefixed, 64 hex characters) ready to be
   * passed straight into registerAsset() / verifyLogoIntegrity().
   */
  async function hashFileToBytes32(file) {
    const arrayBuffer = await file.arrayBuffer();
    const digest = await crypto.subtle.digest("SHA-256", arrayBuffer);
    return bufferToHex(digest);
  }

  function bufferToHex(buffer) {
    const bytes = new Uint8Array(buffer);
    let hex = "0x";
    for (let i = 0; i < bytes.length; i++) {
      hex += bytes[i].toString(16).padStart(2, "0");
    }
    return hex;
  }

  ALU.hashFileToBytes32 = hashFileToBytes32;
})();
