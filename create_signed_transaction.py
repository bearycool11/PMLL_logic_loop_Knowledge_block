from bitcoinlib.wallets import Wallet
from bitcoinlib.transactions import Transaction
from bitcoinlib.keys import Key

# Load the private key from the file
private_key_path = '/mnt/data/privat_key.asc'
with open(private_key_path, 'r') as file:
    private_key_wif = file.read().strip()

# Initialize a Bitcoin key using the private key
key = Key(import_key=private_key_wif)

# Create a new transaction
tx = Transaction()
tx.add_input(prev_txid='<txid>', output_n=0, key=key)  # Placeholder for the originating UTXO
tx.add_output('3FLvnNm9zXqgCgj8zhkBdwz8QsPEe3YJbt', 0.001)  # The receiving address and amount in BTC

# Sign the transaction
tx.sign(key)

# Print the signed transaction hex (ready to broadcast)
signed_tx_hex = tx.as_hex()
print("Signed Transaction:", signed_tx_hex)
