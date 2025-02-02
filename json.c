#include "json.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Parses a JSON string into a cJSON object.
 * @param json_str The input JSON string.
 * @return A pointer to the parsed cJSON object or NULL on error.
 */
cJSON* json_parse(const char* json_str) {
    if (!json_str) {
        fprintf(stderr, "Input JSON string is NULL.\n");
        return NULL;
    }

    cJSON* json_obj = cJSON_Parse(json_str);
    if (!json_obj) {
        fprintf(stderr, "JSON Parsing Error: %s\n", cJSON_GetErrorPtr());
    }
    return json_obj;
}

/**
 * Converts a cJSON object into a JSON string.
 * @param json_obj The input cJSON object.
 * @return A JSON string or NULL on error. Must be freed by the caller.
 */
char* json_stringify(const cJSON* json_obj) {
    if (!json_obj) {
        fprintf(stderr, "JSON Object is NULL. Cannot stringify.\n");
        return NULL;
    }

    char* json_str = cJSON_Print(json_obj);
    if (!json_str) {
        fprintf(stderr, "Error during JSON stringification.\n");
    }
    return json_str;
}

/**
 * Retrieves the value of a string from a cJSON object by key.
 * @param json_obj The cJSON object.
 * @param key The key to retrieve.
 * @return The string value or NULL if the key is not found or not a string.
 */
const char* json_get_string(const cJSON* json_obj, const char* key) {
    if (!json_obj || !key) {
        fprintf(stderr, "Invalid parameters to json_get_string.\n");
        return NULL;
    }

    cJSON* item = cJSON_GetObjectItemCaseSensitive(json_obj, key);
    if (cJSON_IsString(item) && (item->valuestring != NULL)) {
        return item->valuestring;
    }

    fprintf(stderr, "Key '%s' not found or not a string.\n", key);
    return NULL;
}

/**
 * Retrieves a JSON object corresponding to a logic scope.
 * Supports PMLL, ARLL, and EFLL scopes.
 * @param json_obj The cJSON object.
 * @param scope The scope to retrieve (e.g., "PMLL", "ARLL", "EFLL").
 * @return A pointer to the corresponding JSON object or NULL if not found.
 */
cJSON* json_get_scope(const cJSON* json_obj, const char* scope) {
    if (!json_obj || !scope) {
        fprintf(stderr, "Invalid parameters to json_get_scope.\n");
        return NULL;
    }

    cJSON* scope_obj = cJSON_GetObjectItemCaseSensitive(json_obj, scope);
    if (scope_obj && cJSON_IsObject(scope_obj)) {
        return scope_obj;
    }

    fprintf(stderr, "Scope '%s' not found or not a JSON object.\n", scope);
    return NULL;
}

/**
 * Sets a string value for a key in a specific logic scope.
 * @param json_obj The cJSON object.
 * @param scope The logic scope (e.g., "PMLL", "ARLL", "EFLL").
 * @param key The key to set.
 * @param value The string value to set.
 * @return 1 on success, 0 on failure.
 */
int json_set_scope_string(cJSON* json_obj, const char* scope, const char* key, const char* value) {
    if (!json_obj || !scope || !key || !value) {
        fprintf(stderr, "Invalid parameters to json_set_scope_string.\n");
        return 0;
    }

    cJSON* scope_obj = cJSON_GetObjectItemCaseSensitive(json_obj, scope);
    if (!scope_obj) {
        fprintf(stderr, "Scope '%s' not found. Creating a new scope.\n", scope);
        scope_obj = cJSON_CreateObject();
        if (!scope_obj) {
            fprintf(stderr, "Failed to create scope '%s'.\n", scope);
            return 0;
        }
        cJSON_AddItemToObject(json_obj, scope, scope_obj);
    }

    return json_set_string(scope_obj, key, value);
}

/**
 * Deletes a logic scope from a JSON object.
 * @param json_obj The cJSON object.
 * @param scope The logic scope to delete (e.g., "PMLL", "ARLL", "EFLL").
 * @return 1 on success, 0 on failure.
 */
int json_delete_scope(cJSON* json_obj, const char* scope) {
    if (!json_obj || !scope) {
        fprintf(stderr, "Invalid parameters to json_delete_scope.\n");
        return 0;
    }

    cJSON_DeleteItemFromObject(json_obj, scope);
    fprintf(stderr, "Deleted scope '%s'.\n", scope);
    return 1;
}

/**
 * Validates the structure of a logic scope (e.g., PMLL, ARLL, EFLL).
 * Ensures required keys exist and have valid types.
 * @param scope_obj The scope object to validate.
 * @return 1 if valid, 0 otherwise.
 */
int json_validate_scope(const cJSON* scope_obj) {
    if (!scope_obj || !cJSON_IsObject(scope_obj)) {
        fprintf(stderr, "Invalid or non-object scope provided for validation.\n");
        return 0;
    }

    const char* required_keys[] = {"name", "status", "details"};
    for (int i = 0; i < sizeof(required_keys) / sizeof(required_keys[0]); i++) {
        const char* key = required_keys[i];
        cJSON* item = cJSON_GetObjectItemCaseSensitive(scope_obj, key);
        if (!item || !cJSON_IsString(item)) {
            fprintf(stderr, "Validation failed: Key '%s' is missing or invalid.\n", key);
            return 0;
        }
    }

    return 1;
}

/**
 * Deletes a cJSON object.
 * @param json_obj The cJSON object to delete.
 */
void json_delete(cJSON* json_obj) {
    if (!json_obj) {
        fprintf(stderr, "Invalid JSON object for deletion.\n");
        return;
    }
    cJSON_Delete(json_obj);
}
