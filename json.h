#ifndef JSON_H
#define JSON_H

#include "cJSON.h" // Dependency for JSON handling

// Function Prototypes

/**
 * Parses a JSON string into a cJSON object.
 * @param json_str The input JSON string.
 * @return A pointer to the parsed cJSON object or NULL on error.
 */
cJSON* json_parse(const char* json_str);

/**
 * Converts a cJSON object into a JSON string.
 * @param json_obj The input cJSON object.
 * @return A JSON string or NULL on error. Must be freed by the caller.
 */
char* json_stringify(const cJSON* json_obj);

/**
 * Retrieves the value of a string from a cJSON object by key.
 * @param json_obj The cJSON object.
 * @param key The key to retrieve.
 * @return The string value or NULL if the key is not found or not a string.
 */
const char* json_get_string(const cJSON* json_obj, const char* key);

/**
 * Sets a string value for a key in a cJSON object.
 * @param json_obj The cJSON object.
 * @param key The key to set.
 * @param value The string value to set.
 * @return 1 on success, 0 on failure.
 */
int json_set_string(cJSON* json_obj, const char* key, const char* value);

/**
 * Retrieves a JSON object corresponding to a logic scope.
 * Supports PMLL, ARLL, and EFLL scopes.
 * @param json_obj The cJSON object.
 * @param scope The scope to retrieve (e.g., "PMLL", "ARLL", "EFLL").
 * @return A pointer to the corresponding JSON object or NULL if not found.
 */
cJSON* json_get_scope(const cJSON* json_obj, const char* scope);

/**
 * Sets a string value for a key in a specific logic scope.
 * @param json_obj The cJSON object.
 * @param scope The logic scope (e.g., "PMLL", "ARLL", "EFLL").
 * @param key The key to set.
 * @param value The string value to set.
 * @return 1 on success, 0 on failure.
 */
int json_set_scope_string(cJSON* json_obj, const char* scope, const char* key, const char* value);

/**
 * Deletes a logic scope from a JSON object.
 * @param json_obj The cJSON object.
 * @param scope The logic scope to delete (e.g., "PMLL", "ARLL", "EFLL").
 * @return 1 on success, 0 on failure.
 */
int json_delete_scope(cJSON* json_obj, const char* scope);

/**
 * Validates the structure of a logic scope (e.g., PMLL, ARLL, EFLL).
 * Ensures required keys exist and have valid types.
 * @param scope_obj The scope object to validate.
 * @return 1 if valid, 0 otherwise.
 */
int json_validate_scope(const cJSON* scope_obj);

/**
 * Deletes a cJSON object.
 * @param json_obj The cJSON object to delete.
 */
void json_delete(cJSON* json_obj);

#endif // JSON_H
