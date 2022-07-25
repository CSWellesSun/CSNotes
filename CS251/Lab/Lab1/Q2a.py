from sys import exit
from bitcoin.core.script import *

from lib.utils import *
from lib.config import (my_private_key, my_public_key, my_address,
                        faucet_address, network_type)
from Q1 import send_from_P2PKH_transaction


######################################################################
# TODO: Complete the scriptPubKey implementation for Exercise 2
Q2a_txout_scriptPubKey = [
    OP_2DUP, OP_ADD, 32001, OP_EQUALVERIFY, OP_SUB, 2339, OP_EQUALVERIFY
]
######################################################################

if __name__ == '__main__':
    ######################################################################
    # TODO: set these parameters correctly
    amount_to_send = 0.00024  # amount of BTC in the output you're sending minus fee
    txid_to_spend = (
        'a289f7a896cadc4dd7e2c8e0bae527094af15df010ea5436b577b5bf425e27ed')
    utxo_index = 2  # index of the output you are spending, indices start at 0
    ######################################################################

    response = send_from_P2PKH_transaction(
        amount_to_send, txid_to_spend, utxo_index,
        Q2a_txout_scriptPubKey, my_private_key, network_type)
    print(response.status_code, response.reason)
    print(response.text)
