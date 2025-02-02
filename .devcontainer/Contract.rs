use cosmwasm_std::{to_binary, Binary, Deps, DepsMut, Env, MessageInfo, Response, StdError, StdResult};
use schemars::JsonSchema;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct InstantiateMsg {
    pub employee_name: String,
    pub base_salary: u128,
    pub ppu_equity: u8,
    pub vesting_months: u16,
    pub performance_bonus: u8,
    pub healthcare: String,
    pub pto_policy: String,
    pub retirement_plan: String,
    pub professional_stipend: u128,
    pub bitcoin_wallet: String, // Added Bitcoin wallet field
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct ExecuteMsg {
    pub update_salary: Option<u128>,
    pub update_vesting_schedule: Option<u16>,
    pub update_bitcoin_wallet: Option<String>, // Allow updating the Bitcoin wallet
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct QueryMsg {
    pub employee_details: bool,
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct EmployeeDetails {
    pub employee_name: String,
    pub base_salary: u128,
    pub ppu_equity: u8,
    pub vesting_months: u16,
    pub performance_bonus: u8,
    pub healthcare: String,
    pub pto_policy: String,
    pub retirement_plan: String,
    pub professional_stipend: u128,
    pub bitcoin_wallet: String, // Include Bitcoin wallet in state
}

pub fn instantiate(
    deps: DepsMut,
    _env: Env,
    _info: MessageInfo,
    msg: InstantiateMsg,
) -> StdResult<Response> {
    let data = EmployeeDetails {
        employee_name: msg.employee_name,
        base_salary: msg.base_salary,
        ppu_equity: msg.ppu_equity,
        vesting_months: msg.vesting_months,
        performance_bonus: msg.performance_bonus,
        healthcare: msg.healthcare,
        pto_policy: msg.pto_policy,
        retirement_plan: msg.retirement_plan,
        professional_stipend: msg.professional_stipend,
        bitcoin_wallet: msg.bitcoin_wallet, // Initialize Bitcoin wallet
    };

    deps.storage.set(b"employee_details", &to_binary(&data)?);

    Ok(Response::new()
        .add_attribute("action", "instantiate")
        .add_attribute("employee", data.employee_name))
}

pub fn execute(
    deps: DepsMut,
    _env: Env,
    _info: MessageInfo,
    msg: ExecuteMsg,
) -> StdResult<Response> {
    let mut details: EmployeeDetails = deps
        .storage
        .get(b"employee_details")
        .ok_or_else(|| StdError::not_found("EmployeeDetails"))?
        .and_then(|data| bincode::deserialize(&data).map_err(|_| StdError::parse_err("EmployeeDetails", "Invalid data")))?;

    if let Some(new_salary) = msg.update_salary {
        details.base_salary = new_salary;
    }

    if let Some(new_vesting) = msg.update_vesting_schedule {
        details.vesting_months = new_vesting;
    }

    if let Some(new_bitcoin_wallet) = msg.update_bitcoin_wallet {
        details.bitcoin_wallet = new_bitcoin_wallet; // Update Bitcoin wallet
    }

    deps.storage.set(b"employee_details", &to_binary(&details)?);

    Ok(Response::new().add_attribute("action", "update"))
}

pub fn query(
    deps: Deps,
    _env: Env,
    msg: QueryMsg,
) -> StdResult<Binary> {
    if msg.employee_details {
        let details: EmployeeDetails = deps
            .storage
            .get(b"employee_details")
            .ok_or_else(|| StdError::not_found("EmployeeDetails"))?
            .and_then(|data| bincode::deserialize(&data).map_err(|_| StdError::parse_err("EmployeeDetails", "Invalid data")))?;

        return to_binary(&details);
    }

    Err(StdError::generic_err("Unknown query"))
}

use cosmwasm_std::{to_binary, Binary, Deps, DepsMut, Env, MessageInfo, Response, StdError, StdResult};
use schemars::JsonSchema;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct InstantiateMsg {
    pub employee_name: String,
    pub base_salary: u128,
    pub ppu_equity: u8,
    pub vesting_months: u16,
    pub performance_bonus: u8,
    pub healthcare: String,
    pub pto_policy: String,
    pub retirement_plan: String,
    pub professional_stipend: u128,
    pub bitcoin_wallet: String, // Bitcoin wallet field
    pub cosmos_wallet: String, // Cosmos wallet field
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct ExecuteMsg {
    pub update_salary: Option<u128>,
    pub update_vesting_schedule: Option<u16>,
    pub update_bitcoin_wallet: Option<String>, // Allow updates to Bitcoin wallet
    pub update_cosmos_wallet: Option<String>, // Allow updates to Cosmos wallet
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct QueryMsg {
    pub employee_details: bool,
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct EmployeeDetails {
    pub employee_name: String,
    pub base_salary: u128,
    pub ppu_equity: u8,
    pub vesting_months: u16,
    pub performance_bonus: u8,
    pub healthcare: String,
    pub pto_policy: String,
    pub retirement_plan: String,
    pub professional_stipend: u128,
    pub bitcoin_wallet: String, // Include Bitcoin wallet in state
    pub cosmos_wallet: String, // Include Cosmos wallet in state
}

pub fn instantiate(
    deps: DepsMut,
    _env: Env,
    _info: MessageInfo,
    msg: InstantiateMsg,
) -> StdResult<Response> {
    let data = EmployeeDetails {
        employee_name: msg.employee_name,
        base_salary: msg.base_salary,
        ppu_equity: msg.ppu_equity,
        vesting_months: msg.vesting_months,
        performance_bonus: msg.performance_bonus,
        healthcare: msg.healthcare,
        pto_policy: msg.pto_policy,
        retirement_plan: msg.retirement_plan,
        professional_stipend: msg.professional_stipend,
        bitcoin_wallet: msg.bitcoin_wallet, // Initialize Bitcoin wallet
        cosmos_wallet: msg.cosmos_wallet, // Initialize Cosmos wallet
    };

    deps.storage.set(b"employee_details", &to_binary(&data)?);

    Ok(Response::new()
        .add_attribute("action", "instantiate")
        .add_attribute("employee", data.employee_name))
}

pub fn execute(
    deps: DepsMut,
    _env: Env,
    _info: MessageInfo,
    msg: ExecuteMsg,
) -> StdResult<Response> {
    let mut details: EmployeeDetails = deps
        .storage
        .get(b"employee_details")
        .ok_or_else(|| StdError::not_found("EmployeeDetails"))?
        .and_then(|data| bincode::deserialize(&data).map_err(|_| StdError::parse_err("EmployeeDetails", "Invalid data")))?;

    if let Some(new_salary) = msg.update_salary {
        details.base_salary = new_salary;
    }

    if let Some(new_vesting) = msg.update_vesting_schedule {
        details.vesting_months = new_vesting;
    }

    if let Some(new_bitcoin_wallet) = msg.update_bitcoin_wallet {
        details.bitcoin_wallet = new_bitcoin_wallet; // Update Bitcoin wallet
    }

    if let Some(new_cosmos_wallet) = msg.update_cosmos_wallet {
        details.cosmos_wallet = new_cosmos_wallet; // Update Cosmos wallet
    }

    deps.storage.set(b"employee_details", &to_binary(&details)?);

    Ok(Response::new().add_attribute("action", "update"))
}

pub fn query(
    deps: Deps,
    _env: Env,
    msg: QueryMsg,
) -> StdResult<Binary> {
    if msg.employee_details {
        let details: EmployeeDetails = deps
            .storage
            .get(b"employee_details")
            .ok_or_else(|| StdError::not_found("EmployeeDetails"))?
            .and_then(|data| bincode::deserialize(&data).map_err(|_| StdError::parse_err("EmployeeDetails", "Invalid data")))?;

        return to_binary(&details);
    }

    Err(StdError::generic_err("Unknown query"))
}

pub fn instantiate(
    deps: DepsMut,
    _env: Env,
    _info: MessageInfo,
    _msg: InstantiateMsg,
) -> StdResult<Response> {
    let data = EmployeeDetails {
        employee_name: "Josef Kurk Edwards".to_string(),
        base_salary: 300000,
        ppu_equity: 10,
        vesting_months: 48,
        performance_bonus: 20,
        healthcare: "Comprehensive medical, dental, and vision plans".to_string(),
        pto_policy: "Unlimited PTO policy and 18 days annually".to_string(),
        retirement_plan: "401(k) with 5% company match".to_string(),
        professional_stipend: 10000,
        bitcoin_wallet: "35eNZo1A2RTWKkiFVCjxJeYdhUCVBSVL4d".to_string(),
        cosmos_wallet: "cosmos1erfvxk96wdjcure56lx5awqulhy3fk3vnwg0rd".to_string(),
    };

    deps.storage.set(b"employee_details", &to_binary(&data)?);

    Ok(Response::new()
        .add_attribute("action", "instantiate")
        .add_attribute("employee", data.employee_name))
}


