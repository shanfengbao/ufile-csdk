#include <string.h>
#include "json.h"

int JsonGetBool(cJSON *root, const char *key, char *value) {
  cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
  if (!cJSON_IsBool(item)) {
    return -1;  // Error
  }
  *value = cJSON_IsTrue(item) ? 1 : 0;
  return 0;
}

int JsonGetString(cJSON *root, const char *key, char **value) {
  cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
  if (!cJSON_IsString(item)) {
      return -1;  // Error
  }
  *value = strdup(item->valuestring);
  return 0;
}

int JsonGetArray(cJSON *root, const char *key, cJSON **array) {
  *array = cJSON_GetObjectItemCaseSensitive(root, key);
  if (!cJSON_IsArray(*array)) {
      return -1;  // Error
  }
  return 0;
}

int JsonGetInt64(cJSON *root, const char *key, long long *value) {
  cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
  if (!cJSON_IsNumber(item)) {
      return -1;  // Error
  }
  *value = (long long)item->valuedouble;
  return 0;
}