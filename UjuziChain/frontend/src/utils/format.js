// Shared formatting helpers for addresses, dates, and roles.

export function shortenAddress(address) {
  if (!address) return "";
  return `${address.slice(0, 6)}...${address.slice(-4)}`;
}

export function formatTimestamp(unixSeconds) {
  const seconds = Number(unixSeconds);
  if (!seconds) return "Unknown date";
  return new Date(seconds * 1000).toLocaleDateString(undefined, {
    year: "numeric",
    month: "long",
    day: "numeric"
  });
}

export function truncateMiddle(text, length = 24) {
  if (!text || text.length <= length) return text;
  const half = Math.floor(length / 2);
  return `${text.slice(0, half)}...${text.slice(-half)}`;
}
