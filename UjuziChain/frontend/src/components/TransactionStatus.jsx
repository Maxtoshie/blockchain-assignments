// status: "pending" | "success" | "error" | null
export default function TransactionStatus({ status, message }) {
  if (!status) return null;

  const classNames = {
    pending: "tx-banner tx-pending",
    success: "tx-banner tx-success",
    error: "tx-banner tx-error"
  };

  return (
    <div className={classNames[status]}>
      {status === "pending" && <span className="spinner" />}
      <span>{message}</span>
    </div>
  );
}
