import requests
import json

# Constants
BLOCKCYPHER_API_URL = "https://api.blockcypher.com/v1/cosmoshub-4/txs/new"
TOKEN = "8bd4fa2488614e509a677103b88b95fc"

# Reward Claim JSON
reward_claim_payload = {
    "typeUrl": "/cosmos.distribution.v1beta1.MsgWithdrawDelegatorReward",
    "value": {
        "delegatorAddress": "cosmos1erfvxk96wdjcure56lx5awqulhy3fk3vnwg0rd",
        "validatorAddress": "cosmosvaloper1interchainioaddress"
    }
}

# Reinvestment JSON
reinvestment_payload = {
    "typeUrl": "/cosmos.staking.v1beta1.MsgDelegate",
    "value": {
        "delegatorAddress": "cosmos1erfvxk96wdjcure56lx5awqulhy3fk3vnwg0rd",
        "validatorAddress": "cosmosvaloper1interchainioaddress",
        "amount": {
            "denom": "uatom",
            "amount": "75000000"  # 75 ATOM
        }
    }
}

# Function to broadcast a transaction
def broadcast_transaction(payload):
    try:
        headers = {"Content-Type": "application/json"}
        response = requests.post(
            f"{BLOCKCYPHER_API_URL}?token={TOKEN}",
            headers=headers,
            json=payload
        )
        if response.status_code == 201:
            print("Transaction broadcasted successfully!")
            print(json.dumps(response.json(), indent=4))
        else:
            print("Failed to broadcast transaction:")
            print(response.status_code, response.text)
    except Exception as e:
        print(f"Error broadcasting transaction: {e}")

# Main Workflow
if __name__ == "__main__":
    print("Broadcasting reward claim transaction...")
    broadcast_transaction(reward_claim_payload)

    print("\nBroadcasting reinvestment transaction...")
    broadcast_transaction(reinvestment_payload)
