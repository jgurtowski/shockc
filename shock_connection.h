#ifndef SHOCK_CONNECTION_H
#define SHOCK_CONNECTION_H

#include <curl/curl.h>
#include "json.h"

#include "shock_config.h"

/**
 * Output types handled by the shock client
 */
typedef enum ShockDataType_enum{
  SHOCK_UNSET,
  SHOCK_JSON,
  SHOCK_RAW
}ShockDataType;

/**
 * Output settings for the client,
 * Default is SHOCK_JSON to stdout
 */
typedef struct ShockOutputSettings_struct{
  ShockDataType output_type;
  FILE *output_handle;
}ShockOutputSettings;

/**
 * Structure that holds the ShockConnection information
 */
typedef struct ShockConnection_struct{
  CURL *curl_handle;
  ShockOutputSettings *output_settings;
  ShockConfig shock_config;
  json_printer json_printer;
  json_parser json_parser;
}ShockConnection;


/**
 *Creates a new Shock Connection, should be freed with shockConnectionFree
 */
int shockConnectionNew(ShockConnection *conn, ShockConfig *conf);


/**
 *Frees memory associated with ShockConnection object
 */
int shockConnectionFree(ShockConnection *conn);

/**
 *Gets all nodes accessible by the current user
 */
int shockGetNodes(ShockConnection *conn);


/**
 *Gets all users accessible by the current user
 */
int shockGetUsers(ShockConnection *conn);

/**
 *Given a node id, fetch json associated with the node
 */
int shockGetNode(ShockConnection *conn, const char *node_id);

/**
 *Query a node, an array of queries can be given, they will be combined into an "and" query
 */
int shockQueryNode(ShockConnection *conn, const char **queries, int num_queries);

/**
 * Given a node id, download the contents of the file associated with the node
 */
int shockDownloadFile(ShockConnection *conn, const char *node_id);

/**
 * Change how server responses will be handled by the client.
 * @data_type how the output will be displayed on the client
 */
void shockSetDataHandlerType(ShockConnection *conn, ShockDataType data_type);


/**
 * Change how server responses will be handled by the client.
 * @data_dest the output file where data will be stored
 */
void shockSetDataHandlerFile(ShockConnection *conn, FILE *data_dest);

/**
 *Create a node in shock
 *@upload_file_name string path to a local file that will be uploaded to the node (can be NULL)
 *@json_file_name json attributes file that will be used to create the node attributes (can be NULL)
 */
int shockCreateNode(ShockConnection *conn, const char *upload_file_name, const char *json_file_name);

/**
 *Resets all settings from previous reqests, allows ShockConnection handle reuse
 */
void shockHandleReset(ShockConnection *conn);

/**
 * Creates a new output setting structure on the heap
 * Should be paired with shockOutputSettingsFree
 */
ShockOutputSettings *shockOutputSettingsNewDefault();

/**
 * Frees output settings structure
 */
void shockOutputSettingsFree(ShockOutputSettings *settings);

#endif
