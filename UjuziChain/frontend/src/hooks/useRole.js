import { useEffect, useState } from "react";
import { useWeb3 } from "./useWeb3";

/**
 * Determines the connected wallet's role by querying the contract directly.
 * Returns "admin", "institution", or "guest" (student/employer — anyone
 * without special on-chain permissions still has full read access).
 */
export function useRole() {
  const { account, readOnlyContract } = useWeb3();
  const [role, setRole] = useState("guest");
  const [isLoading, setIsLoading] = useState(false);

  useEffect(() => {
    let cancelled = false;

    async function detectRole() {
      if (!account || !readOnlyContract) {
        setRole("guest");
        return;
      }

      setIsLoading(true);
      try {
        const owner = await readOnlyContract.owner();
        if (owner.toLowerCase() === account.toLowerCase()) {
          if (!cancelled) setRole("admin");
          return;
        }

        const isInstitution = await readOnlyContract.isInstitutionRegistered(account);
        if (!cancelled) setRole(isInstitution ? "institution" : "guest");
      } catch (err) {
        if (!cancelled) setRole("guest");
      } finally {
        if (!cancelled) setIsLoading(false);
      }
    }

    detectRole();
    return () => {
      cancelled = true;
    };
  }, [account, readOnlyContract]);

  return { role, isLoading };
}
