from bitcoin.core.script import *

######################################################################
# These functions will be used by Alice and Bob to send their respective
# coins to a utxo that is redeemable either of two cases:
# 1) Recipient provides x such that hash(x) = hash of secret
#    and recipient signs the transaction.
# 2) Sender and recipient both sign transaction
#
# TODO: Fill these in to create scripts that are redeemable by both
#       of the above conditions.
# See this page for opcode documentation: https://en.bitcoin.it/wiki/Script

# This is the ScriptPubKey for the swap transaction
def coinExchangeScript(public_key_sender, public_key_recipient, hash_of_secret):
    return [
        OP_IF, 2, public_key_sender, public_key_recipient, 2, OP_CHECKMULTISIG,
        OP_ELSE, public_key_recipient, OP_CHECKSIGVERIFY, OP_HASH160, hash_of_secret, OP_EQUAL, OP_ENDIF
    ]

# This is the ScriptSig that the receiver will use to redeem coins
def coinExchangeScriptSig1(sig_recipient, secret):
    return [
        secret, sig_recipient, OP_FALSE
    ]

# This is the ScriptSig for sending coins back to the sender if unredeemed
def coinExchangeScriptSig2(sig_sender, sig_recipient):
    return [
        OP_FALSE, sig_sender, sig_recipient, OP_TRUE
    ]
######################################################################

######################################################################
#
# Configured for your addresses
#
# TODO: Fill in all of these fields
# NOTE: the txid and index are fabricated
#
alice_txid_to_spend = "a289f7a896cadc4dd7e2c8e0bae527094af15df010ea5436b577b5bf425e27ed"
alice_utxo_index = 0
alice_amount_to_send = 0.001

bob_txid_to_spend = "a289f7a896cadc4dd7e2c8e0bae527094af15df010ea5436b577b5bf425e27ed"
bob_utxo_index = 1
bob_amount_to_send = 0.001

# Get current block height (for locktime) in 'height' parameter for each blockchain (will be used in swap.py):
#  curl https://api.blockcypher.com/v1/btc/test3
btc_test3_chain_height = 1579945

#  curl https://api.blockcypher.com/v1/bcy/test
bcy_test_chain_height = 2548698

# Parameter for how long Alice/Bob should have to wait before they can take back their coins
# alice_locktime MUST be > bob_locktime
alice_locktime = 5
bob_locktime = 3

tx_fee = 0.0001

# While testing your code, you can edit these variables to see if your
# transaction can be broadcasted succesfully.
broadcast_transactions = False
alice_redeems = False

######################################################################
