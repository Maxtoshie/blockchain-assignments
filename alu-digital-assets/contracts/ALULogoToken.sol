// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

import "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import "@openzeppelin/contracts/access/Ownable.sol";

/**
 * @title ALULogoToken
 * @dev Splits ownership of the ALU logo into 1,000,000 fractional ERC-20 governance shares.
 */
contract ALULogoToken is ERC20, Ownable {
    // Fixed total supply of 1,000,000 tokens
    uint256 private constant TOTAL_SUPPLY_CAP = 1_000_000 * 10**18;

    /**
     * @notice Initializes the contract, names the token, and mints the entire supply to the initial logo owner.
     * @param initialOwner The wallet address representing the university/original owner receiving full initial shares.
     */
    constructor(address initialOwner) 
        ERC20("ALU Logo Token", "ALUT") 
        Ownable(initialOwner) 
    {
        // Mint all 1,000,000 tokens (adjusted for 18 decimals) directly to the initial owner
        _mint(initialOwner, TOTAL_SUPPLY_CAP);
    }

    /**
     * @notice Distributes specific ownership shares to an authorized stakeholder.
     * @dev Restricted to the contract owner via the onlyOwner modifier.
     * @param recipient The wallet address of the faculty member, student, or admin.
     * @param amount The standard base token amount to send (inclusive of 18 decimal places).
     */
    function distributeShares(address recipient, uint256 amount) public onlyOwner {
        require(amount > 0, "Error: Distribution amount must be greater than zero.");
        
        // Transfers tokens from the owner's balance to the recipient
        _transfer(msg.sender, recipient, amount);
    }

    /**
     * @notice Calculates the whole number percentage share owned by a single wallet address.
     * @param account The address to check.
     * @return A whole number representing the user's integer percentage of total token supply.
     */
    function ownershipPercentage(address account) public view returns (uint256) {
        // Balance * 100 / Total Supply yields the clean integer percentage
        return (balanceOf(account) * 100) / totalSupply();
    }
}
