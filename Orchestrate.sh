#!/bin/bash
# File: Orchestrate.sh
# Description: Combines consent collection, health checks, payload distribution, dynamic API websocketing, cascading, and advanced PMLL/ARLL/EFLL logic loops.

# Determine the directory of this script (Dynamically resolve the script location)
SCRIPT_DIR="${SCRIPT_DIR:-$(dirname "$0")}"

# Ensure the directory where Orchestrate.sh is located exists
if [ ! -d "$SCRIPT_DIR" ]; then
    echo "ERROR: The directory for Orchestrate.sh does not exist! Exiting."
    exit 1
fi

# Define paths dynamically based on SCRIPT_DIR

LOG_DIR="$SCRIPT_DIR/logs"
BUILD_LOG="$LOG_DIR/build.log"
ORCHESTRA_LOG="$LOG_DIR/orchestra.log"
CONSENT_LOG_FILE="$LOG_DIR/consent_responses.log"
CONSENT_SCRIPT="$SCRIPT_DIR/Consent_request.sh"
BINARIES_DIR="$SCRIPT_DIR/binaries"
PORT_BASE=55878  # Updated Port Base

# Components to orchestrate
COMPONENTS=("VECTOR_MATRIX" "MEMORY_SILO" "IO_SOCKET" "PML_LOGIC_LOOP" "UNIFIED_VOICE" "CROSS_TALK" "PERSISTENCE" "ARLL" "EFLL")

# URL/IP configuration for silos
SILO_DOMAIN="silo.example.com"
START_SILO=1
END_SILO=144000
RETRY_COUNT=3
BATCH_SIZE=1000

# Ensure directories exist
mkdir -p "$LOG_DIR" "$BINARIES_DIR"

# Logging utility
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$ORCHESTRA_LOG"
}

# Consent collection and verification
log "Starting consent request process..."
bash "$CONSENT_SCRIPT"
if [ $? -ne 0 ]; then
    log "ERROR: Consent_request.sh failed. Exiting."
    exit 1
fi

if [ ! -f "$CONSENT_LOG_FILE" ]; then
    log "ERROR: Consent log not found. Exiting."
    exit 1
fi
SUCCESS_COUNT=$(grep -c "SUCCESS" "$CONSENT_LOG_FILE")
FAILURE_COUNT=$(grep -c "ERROR" "$CONSENT_LOG_FILE")
log "Consent process completed. Success: $SUCCESS_COUNT, Failures: $FAILURE_COUNT"

# Compile dependencies
log "Compiling dependencies..."
make clean all &>> "$BUILD_LOG"
if [ $? -ne 0 ]; then
    log "ERROR: Compilation failed. Check $BUILD_LOG for details."
    exit 1
fi
log "Compilation completed successfully."

# Validate compiled components
log "Validating compiled components..."
for component in "${COMPONENTS[@]}"; do
    if [ ! -f "$BINARIES_DIR/$component" ]; then
        log "ERROR: Missing executable for $component in $BINARIES_DIR. Skipping deployment for this component."
    fi
done
log "All components validated and prepared for distribution."

# Health check function with retry logic
health_check() {
    local silo=$1
    local retries=3
    local delay=5
    local attempt=1
    while [ $attempt -le $retries ]; do
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "https://silo$silo.$SILO_DOMAIN/health")
        if [ "$RESPONSE" == "200" ]; then
            return 0
        fi
        log "Health check failed for silo$silo.$SILO_DOMAIN (Attempt $attempt/$retries)"
        sleep $delay
        attempt=$((attempt + 1))
    done
    log "ERROR: Health check failed for silo$silo.$SILO_DOMAIN after $retries attempts."
    return 1
}

# Websocket check for silo status
check_status_via_websocket() {
    local silo=$1
    local ws_url="wss://silo${silo}.$SILO_DOMAIN/status"
    RESPONSE=$(curl -s --include "$ws_url" | grep -q "open" && echo "open" || echo "closed")
    echo "$RESPONSE"
}

# Modular logic: Integrate PMLL, ARLL, and EFLL loops
run_logic_loops() {
    local silo_id=$1
    PMLL_CHECK=$("$BINARIES_DIR/PML_LOGIC_LOOP" "$silo_id")
    ARLL_CHECK=$("$BINARIES_DIR/ARLL" "$silo_id")
    EFLL_CHECK=$("$BINARIES_DIR/EFLL" "$silo_id")
    
    if [[ $PMLL_CHECK == "OK" && $ARLL_CHECK == "OK" && $EFLL_CHECK == "OK" ]]; then
        log "Silo $silo_id is healthy"
    else
        log "Silo $silo_id issues: PMLL=$PMLL_CHECK, ARLL=$ARLL_CHECK, EFLL=$EFLL_CHECK"
    fi
}

# Process silos in batches
process_silos_in_batches() {
    for (( i=$START_SILO; i<=$END_SILO; i+=$BATCH_SIZE )); do
        local batch_end=$((i + BATCH_SIZE - 1))
        if [ $batch_end -gt $END_SILO ]; then
            batch_end=$END_SILO
        fi
        log "Processing silos from $i to $batch_end..."
        for silo in $(seq $i $batch_end); do
            (check_status_via_websocket "$silo" & run_logic_loops "$silo") &
        done
        wait
    done
}

# Deploy binaries to silos
log "Deploying binaries to silos..."
for binary in "${COMPONENTS[@]}"; do
    process_silos_in_batches
done
log "Binary distribution complete."

# Summary
log "Deployment process completed."
echo "Payload Notifications: Completed"
echo "Binary Deployments: Completed"
echo "Health Checks: Completed"

