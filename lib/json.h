#include "cJSON.h"

int JsonGetBool(cJSON *root, const char *key, char *value);

int JsonGetString(cJSON *root, const char *key, char **value);

int JsonGetArray(cJSON *root, const char *key, cJSON **array);

int JsonGetInt64(cJSON *root, const char *key, long long *value);