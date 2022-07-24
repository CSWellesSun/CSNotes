from bitcoin import SelectParams
from bitcoin.base58 import decode
from bitcoin.core import x
from bitcoin.wallet import CBitcoinAddress, CBitcoinSecret, P2PKHBitcoinAddress


SelectParams('testnet')

faucet_address = CBitcoinAddress('mv4rnyY3Su5gjcDNzbMLKBQkBicCtHUtFB')

# For questions 1-3, we are using 'btc-test3' network. For question 4, you will
# set this to be either 'btc-test3' or 'bcy-test'
# network_type = 'btc-test3'
network_type = 'bcy-test'


######################################################################
# This section is for Questions 1-3
# TODO: Fill this in with your private key.
#
# Create a private key and address pair in Base58 with keygen.py
# Send coins at https://testnet-faucet.mempool.co/

my_private_key = CBitcoinSecret(
    'cT2hqRPvS8HCASPAhkNk1VyLVv2VxZJ7cuhjqhGRsDtXt8eBSpD8')

my_public_key = my_private_key.pub
my_address = P2PKHBitcoinAddress.from_pubkey(my_public_key) # mqREKq6kx82i3daj12ALt49k7tvUptNZ9E
######################################################################


######################################################################
# NOTE: This section is for Question 4
# TODO: Fill this in with address secret key for BTC testnet3
#
# Create address in Base58 with keygen.py
# Send coins at https://testnet-faucet.mempool.co/

# Only to be imported by alice.py
# Alice should have coins!!
alice_secret_key_BTC = CBitcoinSecret(
    'cPUqpshccRN3XLhfJ99Umnyp73spqfYQnA24wWHg7FVU8ACDsEqZ')

# Only to be imported by bob.py
bob_secret_key_BTC = CBitcoinSecret(
    'cUS8vtFyY49qYj1GeAWoUL1UKjTw2NvrdVnXXunqzZD2Q5soiDy9')

# Can be imported by alice.py or bob.py
alice_public_key_BTC = alice_secret_key_BTC.pub
alice_address_BTC = P2PKHBitcoinAddress.from_pubkey(alice_public_key_BTC) # mnJ7mTLG6ptZh6hYuEpYxhgno1A9hUkYzS

bob_public_key_BTC = bob_secret_key_BTC.pub
bob_address_BTC = P2PKHBitcoinAddress.from_pubkey(bob_public_key_BTC) # mxbGnQZEFBTBXnTRHosT5d2xu7C7Cx3zDT
######################################################################


######################################################################
# NOTE: This section is for Question 4
# TODO: Fill this in with address secret key for BCY testnet
#
# Create address in hex with
# curl -X POST https://api.blockcypher.com/v1/bcy/test/addrs?token=YOURTOKEN
# This request will return a private key, public key and address. Make sure to save these.
#
# Send coins with
# curl -d '{"address": "BCY_ADDRESS", "amount": 1000000}' https://api.blockcypher.com/v1/bcy/test/faucet?token=YOURTOKEN
# This request will return a transaction reference. Make sure to save this.

# Only to be imported by alice.py
alice_secret_key_BCY = CBitcoinSecret.from_secret_bytes(
    x('b6a609dabac9925ba54573d6f3bd634d9c725347f042e704716d17d21dc7c6e7'))

# Only to be imported by bob.py
# Bob should have coins!!
bob_secret_key_BCY = CBitcoinSecret.from_secret_bytes(
    x('3026e7145c80154566b1d5374813fc81b3a1386d2363bceaf3959ce7d1376a38'))

# Can be imported by alice.py or bob.py
alice_public_key_BCY = alice_secret_key_BCY.pub # 0304499700aa4b623a1bf9f4ea3b0abbbc5aa1833873c037ec616f3dd865775c4f
alice_address_BCY = P2PKHBitcoinAddress.from_pubkey(alice_public_key_BCY) # C5kWjUfJYxjsydgJUxvCHA3XhhiXLVin2U

bob_public_key_BCY = bob_secret_key_BCY.pub # 02c3040932a6b6d4abc391cc528f8dd6e171bb22317dd21cd7f916da593a138cdf
bob_address_BCY = P2PKHBitcoinAddress.from_pubkey(bob_public_key_BCY) # C4zDGsBqLfXbxn2KWzq24E6LF2u33aKdSc
######################################################################

# TxID
# my_address 0.002 8451c7c103cb2570f34f5888e1305df41971f6fa7b8e5f347373f1bfb03292f7
# alice_address_BTC 0.001 ad284c30d16c2e01ad8b180b90cc4a1c72fa341151f666f529b442fce9b1feaa
# bob_address_BTC 0.001 1ebf3ab0f0f95fca6e8bb36a6eabdabd61bd9aaedf2c73ebe258f69ffd7246bd
# bob_address_BCY 0.01 6a8d8c822e3b3d9397dcb232f922e5e706702eabf8cec2f8b25982805fb8b1b5