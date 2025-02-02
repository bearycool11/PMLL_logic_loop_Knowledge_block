from fastapi import FastAPI, HTTPException, Query
from pydantic import BaseModel
import requests

app = FastAPI()

# Global configuration for proxy settings
proxy_settings = {
    "http": None,
    "https": None,
}

# ---- Proxy Management Endpoints ----
@app.get("/configure_proxy")
async def configure_proxy(http_proxy: str = Query(None), https_proxy: str = Query(None)):
    """
    Configure HTTP and HTTPS proxy settings dynamically.
    """
    global proxy_settings
    if http_proxy:
        proxy_settings["http"] = http_proxy
    if https_proxy:
        proxy_settings["https"] = https_proxy
    return {
        "message": "Proxy configuration updated successfully.",
        "current_proxy_settings": proxy_settings,
    }

@app.get("/test_proxy")
async def test_proxy(url: str = Query("http://example.com")):
    """
    Test connectivity through the configured proxy.
    """
    try:
        response = requests.get(url, proxies=proxy_settings, timeout=5)
        return {
            "message": "Proxy connection successful.",
            "url": url,
            "status_code": response.status_code,
            "content": response.text[:100],  # Show a snippet of the content
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Proxy test failed: {str(e)}")

# ---- IP Configuration Endpoints ----
class IPConfig(BaseModel):
    ipv4: str
    subnet_mask: str
    router: str

@app.post("/update_ip_config")
async def update_ip_config(config: IPConfig):
    """
    Update the IP configuration dynamically.
    """
    # Simulated update logic (you can extend this with actual OS-level commands)
    return {
        "message": "IP configuration updated successfully.",
        "config": config,
    }

@app.get("/current_ip_config")
async def get_current_ip_config():
    """
    Retrieve current IP configuration.
    """
    # Replace with dynamic logic to fetch actual network settings
    mock_config = {
        "ipv4": "192.168.12.24",
        "subnet_mask": "255.255.255.0",
        "router": "192.168.12.1",
        "proxy": proxy_settings,
    }
    return mock_config

# ---- Root Endpoint ----
@app.get("/")
async def root():
    return {
        "message": "Welcome to Finn's Network Orchestration Framework.",
        "resources": {
            "Configure Proxy": "/configure_proxy?http_proxy=http://fatbabs",
            "Test Proxy": "/test_proxy?url=http://example.com",
        },
    }
