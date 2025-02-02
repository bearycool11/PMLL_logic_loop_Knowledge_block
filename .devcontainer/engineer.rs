use cosmwasm_std::{to_binary, Binary, Deps, DepsMut, Env, MessageInfo, Response, StdError, StdResult};
use schemars::JsonSchema;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct InstantiateMsg {
    pub employee_name: String,
    pub base_salary: u128,
    pub equity_percentage: u8,
    pub github_token: String,
    pub interchain_responsibilities: Vec<String>,
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct ExecuteMsg {
    pub update_salary: Option<u128>,
    pub add_responsibility: Option<String>,
    pub update_github_token: Option<String>,
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct QueryMsg {
    pub employee_details: bool,
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct EmployeeDetails {
    pub employee_name: String,
    pub base_salary: u128,
    pub equity_percentage: u8,
    pub github_token: String,
    pub interchain_responsibilities: Vec<String>,
}

pub fn instantiate(
    deps: DepsMut,
    _env: Env,
    _info: MessageInfo,
    msg: InstantiateMsg,
) -> StdResult<Response> {
    let employee = EmployeeDetails {
        employee_name: msg.employee_name,
        base_salary: msg.base_salary,
        equity_percentage: msg.equity_percentage,
        github_token: msg.github_token,
        interchain_responsibilities: msg.interchain_responsibilities,
    };

    deps.storage.set(b"employee_details", &to_binary(&employee)?);

    Ok(Response::new().add_attribute("action", "instantiate").add_attribute("employee", employee.employee_name))
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

    if let Some(new_responsibility) = msg.add_responsibility {
        details.interchain_responsibilities.push(new_responsibility);
    }

    if let Some(new_github_token) = msg.update_github_token {
        details.github_token = new_github_token;
    }

    deps.storage.set(b"employee_details", &to_binary(&details)?);

    Ok(Response::new().add_attribute("action", "update"))
}

pub fn query(deps: Deps, _env: Env, msg: QueryMsg) -> StdResult<Binary> {
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
    pub equity_percentage: u8,
    pub github_token: String,
    pub interchain_responsibilities: Vec<String>,
    pub address: String,             // New field for employee address
    pub date_of_birth: String,       // New field for date of birth
    pub position: String,            // New field for employee position
    pub start_date: String,          // New field for employment start date
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct ExecuteMsg {
    pub update_salary: Option<u128>,
    pub add_responsibility: Option<String>,
    pub update_github_token: Option<String>,
    pub update_address: Option<String>,       // New field for address update
    pub update_position: Option<String>,      // New field for position update
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct QueryMsg {
    pub employee_details: bool,
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct EmployeeDetails {
    pub employee_name: String,
    pub base_salary: u128,
    pub equity_percentage: u8,
    pub github_token: String,
    pub interchain_responsibilities: Vec<String>,
    pub address: String,
    pub date_of_birth: String,
    pub position: String,
    pub start_date: String,
}

pub fn instantiate(
    deps: DepsMut,
    _env: Env,
    _info: MessageInfo,
    msg: InstantiateMsg,
) -> StdResult<Response> {
    let employee = EmployeeDetails {
        employee_name: msg.employee_name,
        base_salary: msg.base_salary,
        equity_percentage: msg.equity_percentage,
        github_token: msg.github_token,
        interchain_responsibilities: msg.interchain_responsibilities,
        address: msg.address,
        date_of_birth: msg.date_of_birth,
        position: msg.position,
        start_date: msg.start_date,
    };

    deps.storage.set(b"employee_details", &to_binary(&employee)?);

    Ok(Response::new()
        .add_attribute("action", "instantiate")
        .add_attribute("employee", employee.employee_name))
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

    if let Some(new_responsibility) = msg.add_responsibility {
        details.interchain_responsibilities.push(new_responsibility);
    }

    if let Some(new_github_token) = msg.update_github_token {
        details.github_token = new_github_token;
    }

    if let Some(new_address) = msg.update_address {
        details.address = new_address;
    }

    if let Some(new_position) = msg.update_position {
        details.position = new_position;
    }

    deps.storage.set(b"employee_details", &to_binary(&details)?);

    Ok(Response::new().add_attribute("action", "update"))
}

pub fn query(deps: Deps, _env: Env, msg: QueryMsg) -> StdResult<Binary> {
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
    pub equity_percentage: u8,
    pub github_token: String,
    pub interchain_responsibilities: Vec<String>,
    pub profile_picture: String, // Base64 encoded image
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct ExecuteMsg {
    pub update_salary: Option<u128>,
    pub add_responsibility: Option<String>,
    pub update_github_token: Option<String>,
    pub update_profile_picture: Option<String>, // Option to update profile picture
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct QueryMsg {
    pub employee_details: bool,
}

#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, JsonSchema)]
pub struct EmployeeDetails {
    pub employee_name: String,
    pub base_salary: u128,
    pub equity_percentage: u8,
    pub github_token: String,
    pub interchain_responsibilities: Vec<String>,
    pub profile_picture: String, // Store the profile picture in Base64 format
}

pub fn instantiate(
    deps: DepsMut,
    _env: Env,
    _info: MessageInfo,
    msg: InstantiateMsg,
) -> StdResult<Response> {
    let employee = EmployeeDetails {
        employee_name: msg.employee_name,
        base_salary: msg.base_salary,
        equity_percentage: msg.equity_percentage,
        github_token: msg.github_token,
        interchain_responsibilities: msg.interchain_responsibilities,
        profile_picture: msg.profile_picture,
    };

    deps.storage.set(b"employee_details", &to_binary(&employee)?);

    Ok(Response::new()
        .add_attribute("action", "instantiate")
        .add_attribute("employee", employee.employee_name))
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

    if let Some(new_responsibility) = msg.add_responsibility {
        details.interchain_responsibilities.push(new_responsibility);
    }

    if let Some(new_github_token) = msg.update_github_token {
        details.github_token = new_github_token;
    }

    if let Some(new_profile_picture) = msg.update_profile_picture {
        details.profile_picture = new_profile_picture;
    }

    deps.storage.set(b"employee_details", &to_binary(&details)?);

    Ok(Response::new().add_attribute("action", "update"))
}

pub fn query(deps: Deps, _env: Env, msg: QueryMsg) -> StdResult<Binary> {
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

