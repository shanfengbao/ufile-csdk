#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void help(char *program_name) {
  printf("Usage: %s <bucket_name> [prefix=""]\n", program_name);
}

int main(int argc, char *argv[]){
  if(argc < 2 || argc > 3){
    help(argv[0]);
    return 1;
  }

  const char *bucket_name = argv[1];
  const char *prefix = "";
  if (argc > 2) {
    prefix = argv[2];
  }

  struct ufile_config cfg;
  cfg.public_key = getenv("UFILE_PUBLIC_KEY");
  cfg.private_key = getenv("UFILE_PRIVATE_KEY");
  cfg.bucket_host = "api.ucloud.cn";
  cfg.file_host = "cn-bj.ufileos.com";

  struct ufile_error error;
  error = ufile_sdk_initialize(cfg, 1);
  if(UFILE_HAS_ERROR(error.code)){
    ufile_sdk_cleanup();
    printf("Initialize SDK failed: %s\n", error.message);
    return 1;
  }

  int once_list_count = 5;
  char *marker = "";
  char is_truncated = 1;
  struct ufile_list_result result;
  memset(&result, 0, sizeof(result));
  while (is_truncated) {
    error = ufile_list(bucket_name, prefix, "", once_list_count, marker, &result);
    if(UFILE_HAS_ERROR(error.code)){
      printf("List failed, error message: %s\n", error.message);
      return 1;
    } else{
      int i = 0;
      for (i = 0; i < result.entry_num; i++) {
        printf("key: %s, size: %ld, mime_type: %s, last_modified: %ld, create_time: %ld, etag: %s, storage_class: %s\n",
               result.entries[i].filename, result.entries[i].size, result.entries[i].mime_type, result.entries[i].last_modified,
               result.entries[i].create_time, result.entries[i].etag, result.entries[i].storage_class);
      }
      marker = result.next_marker;
      is_truncated = result.is_truncated;
    }
  }

  ufile_free_list_result(result);

  ufile_sdk_cleanup();
}