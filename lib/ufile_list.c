#include "api.h"

#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "http.h"
#include "json.h"


#define UFILE_LIST_RESULT_BUFFER_SIZE (1024 * 1024)

struct ufile_error
ufile_parse_list_response(char *body, struct ufile_list_result *result) {
  struct ufile_error error = NO_ERROR;

  struct ufile_list_result out_result;
  memset(&out_result, 0, sizeof(struct ufile_list_result));

  cJSON *root = cJSON_Parse(body);
  if (!root) {
    error.code = UFILE_MULTIPLE_INIT_ERROR_CODE;
    error.message = cJSON_GetErrorPtr();
    return error;
  }

  int ret = 0;

  // Parse Contents
  cJSON *contents = NULL;
  cJSON *common_prefixes = NULL;
  ret = JsonGetArray(root, "Contents", &contents);
  if (ret) {
    goto err;
  }

  int num_contents = cJSON_GetArraySize(contents);
  ret = JsonGetArray(root, "CommonPrefixes", &common_prefixes);
  if (ret) {
      goto err;
  }

  int num_common_prefixes = cJSON_GetArraySize(common_prefixes);

  int entry_num = num_contents + num_common_prefixes;
  if (entry_num > 0) {
    out_result.entries = (struct ufile_list_result_entry *)malloc(entry_num * sizeof(struct ufile_list_result_entry));
  }
  out_result.entry_num = entry_num;

  int i;
  for (i = 0; i < num_contents; i++) {
    cJSON *content = cJSON_GetArrayItem(contents, i);
    struct ufile_list_result_entry *entry = &out_result.entries[i];

    ret = JsonGetString(content, "Key", &entry->filename);
    if (ret) {
      goto err;
    }

    ret = JsonGetString(content, "MimeType", &entry->mime_type);
    if (ret) {
      goto err;
    }

    ret = JsonGetString(content, "ETag", &entry->etag);
    if (ret) {
      goto err;
    }

    char *size_str = NULL;
    ret = JsonGetString(content, "Size", &size_str);
    if (ret) {
      goto err;
    }
    entry->size = atoll(size_str);
    free(size_str);

    ret = JsonGetString(content, "StorageClass", &entry->storage_class);
    if (ret) {
      goto err;
    }

    ret = JsonGetInt64(content, "LastModified", &entry->last_modified);
    if (ret) {
      goto err;
    }

    ret = JsonGetInt64(content, "CreateTime", &entry->create_time);
    if (ret) {
      goto err;
    }
  }

  int j;
  for (j = 0; j < num_common_prefixes; j++) {
    cJSON *common_prefix = cJSON_GetArrayItem(common_prefixes, j);
    struct ufile_list_result_entry *entry = &out_result.entries[i + j];

    ret = JsonGetString(common_prefix, "Prefix", &entry->filename);
    if (ret) {
      goto err;
    }
  }

  // Parse IsTruncated
  ret = JsonGetBool(root, "IsTruncated", &out_result.is_truncated);
  if (ret) {
    goto err;
  }

  // Parse NextMarker
  ret = JsonGetString(root, "NextMarker", &out_result.next_marker);
  if (ret) {
    goto err;
  }

  *result = out_result;

  cJSON_Delete(root);
  return error;

err:
  cJSON_Delete(root);
  error.code = UFILE_MULTIPLE_INIT_ERROR_CODE;
  error.message = cJSON_GetErrorPtr();
  return error;
}

extern struct ufile_error
ufile_list(const char *bucket_name, const char *prefix, const char *delimiter,
           int count, const char *marker, struct ufile_list_result *result) {
  struct ufile_error error = NO_ERROR;
  if(!bucket_name || *bucket_name == '\0') {
    error.code = UFILE_PARAM_ERROR_CODE;
    error.message = "bucket_name cannot be nil.";
    return error;
  }

  if(!prefix) {
    error.code = UFILE_PARAM_ERROR_CODE;
    error.message = "prefix cannot be nil.";
    return error;
  }

  if (!delimiter) {
    error.code = UFILE_PARAM_ERROR_CODE;
    error.message = "delimiter cannot be nil.";
    return error;
  }

  if (!marker) {
    error.code = UFILE_PARAM_ERROR_CODE;
    error.message = "marker cannot be nil.";
    return error;
  }

  if (count > UFILE_LIST_MAX_COUNT) {
    error.code = UFILE_PARAM_ERROR_CODE;
    error.message = "list count too large.";
    return error;
  }

  if (!result) {
    error.code = UFILE_PARAM_ERROR_CODE;
    error.message = "result pointer cannot be nil.";
    return error;
  }

  CURL *curl = curl_easy_init();
  if(curl == NULL){
    error.code = CURL_ERROR_CODE;
    error.message = CURL_INIT_ERROR_MSG;
    return error;
  }

  struct http_options opt;
  error = set_http_options_for_list(&opt, "GET", "", bucket_name, "", prefix, marker, count, delimiter);
  if(UFILE_HAS_ERROR(error.code)){
    http_cleanup(curl, &opt);
    return error;
  }
  set_curl_options(curl, &opt);
  struct http_body body;
  size_t buffer_size = UFILE_LIST_RESULT_BUFFER_SIZE;
  char *result_buf = (char *)malloc(buffer_size);
  if (result_buf == NULL) {
    error.code = NO_MEMORY_ERROR_CODE;
    error.message = "no memory.";
    return error;
  }
  memset(&body, 0, sizeof(struct http_body));
  body.buffer = result_buf;
  if(body.buffer != NULL){
    body.buffer_size = buffer_size;
    body.pos_n = 0;
  }
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_write_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

  error = curl_do(curl);

  if (UFILE_HAS_ERROR(error.code)) {
    http_cleanup(curl, &opt);
    free(result_buf);
    return error;
  }

  error = ufile_parse_list_response(result_buf, result);
  http_cleanup(curl, &opt);
  free(result_buf);

  return error;
}