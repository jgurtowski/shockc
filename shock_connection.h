#ifndef SHOCK_CONNECTION_H
#define SHOCK_CONNECTION_H

#include <curl/curl.h>
#include "json.h"

#include "shock_config.h"

typedef enum ShockDataType_enum{
  SHOCK_JSON,
  SHOCK_RAW
}ShockDataType;

typedef struct ShockOutputSettings_struct{
  ShockDataType output_type;
  FILE *output_handle;
}ShockOutputSettings;

typedef struct ShockConnection_struct{
  CURL *curl_handle;
  ShockOutputSettings *output_settings;
  ShockConfig shock_config;
  json_printer json_printer;
  json_parser json_parser;
}ShockConnection;


int shockConnectionNew(ShockConnection *conn, ShockConfig *conf);
int shockConnectionFree(ShockConnection *conn);
int shockGetNodes(ShockConnection *conn);
int shockGetUsers(ShockConnection *conn);
int shockGetNode(ShockConnection *conn, const char *node_id);
int shockQueryNode(ShockConnection *conn, const char *queries[], int num_queries);
int shockDownloadFile(ShockConnection *conn, const char *node_id);
void shockSetDataHandler(ShockConnection *conn, ShockDataType data_type, FILE *dataDest);

ShockOutputSettings *shockOutputSettingsNewDefault();
void shockOutputSettingsFree(ShockOutputSettings *settings);

#endif
