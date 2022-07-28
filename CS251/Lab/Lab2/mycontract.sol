// SPDX-License-Identifier: GPL-3.0
pragma solidity ^0.8.9;

contract Splitwise {
    mapping (address => mapping (address => uint32)) internal IOUs;

    function lookup(address debtor, address creditor) public view returns (uint32 ret) {
        return IOUs[debtor][creditor];
    }

    function add_IOU(address creditor, uint32 amount, address[] memory path, uint32 min) public {
        require(msg.sender != creditor);
        IOUs[msg.sender][creditor] = add(IOUs[msg.sender][creditor], amount);
        if (path.length >= 2) {
            IOUs[msg.sender][creditor] = sub(IOUs[msg.sender][creditor], min);
            for (uint i = 0; i < path.length - 1; i++) {
                IOUs[path[i]][path[i+1]] = sub(IOUs[path[i]][path[i+1]], min);
            }
        }
    }
    
    // SafeMath
    function add(uint32 a, uint32 b) pure internal returns (uint32) {
        uint32 c = a + b;
        require(c >= a);
        return c;
    }

    function sub(uint32 a, uint32 b) pure internal returns (uint32) {
        uint32 c = a - b;
        require(c <= a);
        return c;
    }
}