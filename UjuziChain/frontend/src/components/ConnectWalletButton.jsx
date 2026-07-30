import { useWeb3 } from "../hooks/useWeb3";
import { shortenAddress } from "../utils/format";

export default function ConnectWalletButton() {
  const { account, isConnecting, connectWallet, disconnectWallet, isWrongNetwork } = useWeb3();

  if (account) {
    return (
      <div style={{ display: "flex", alignItems: "center", gap: 10 }}>
        {isWrongNetwork && (
          <span className="status-pill status-revoked">Switch to Sepolia</span>
        )}
        <button className="btn btn-ghost mono" onClick={disconnectWallet}>
          {shortenAddress(account)}
        </button>
      </div>
    );
  }

  return (
    <button className="btn btn-gold" onClick={connectWallet} disabled={isConnecting}>
      {isConnecting ? "Connecting..." : "Connect Wallet"}
    </button>
  );
}
