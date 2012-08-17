#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>
#include "json.h"

#include "shock_connection.h"
#include "shock_config.h"
#include "shock_error.h"


static int shockJPrintCallback(void *userdata, const char *s, uint32_t len){
  ShockConnection *conn = (ShockConnection *)userdata;
  fwrite(s,sizeof(char),len,conn->output_settings->output_handle);
  return 0;
}

static int shockJParseCallback(void *userdata, int type, const char *data, uint32_t len){
  ShockConnection *conn = (ShockConnection *)userdata;
  json_print_pretty(&conn->json_printer, type, data,len);
}

static size_t shockCurlWriteHandler(char *ptr, size_t size,
				    size_t nmemb, void *userdata){
  size_t realsize = size * nmemb;
  ShockConnection *conn = (ShockConnection *)userdata;
  switch(conn->output_settings->output_type){
  case SHOCK_JSON:
    json_parser_string(&conn->json_parser, ptr, realsize,NULL);
    break;
  default:
    fwrite(ptr,sizeof(char),realsize,conn->output_settings->output_handle);
  }
  return realsize;
}

static int shockCurlExecute(ShockConnection *conn){
  assert(NULL != conn);
  CURLcode res;
  res = curl_easy_perform(conn->curl_handle);
  if(res != CURLE_OK){
    if(conn->shock_config.debug)
      fprintf(stderr, "Failed to fetch %s\n", curl_easy_strerror(res));
    return SHOCK_CURL_PERFORM_ERROR;
  }
  return 0;
}

int shockConnectionNew(ShockConnection *conn, ShockConfig *conf){
  assert(NULL != conn);
  memcpy(&conn->shock_config, conf, sizeof(ShockConfig));
  conn -> output_settings = shockOutputSettingsNewDefault();
  
  if( NULL == (conn -> curl_handle = curl_easy_init()))
    return SHOCK_CURL_INIT_ERROR;
  
  if(json_print_init(&conn->json_printer, shockJPrintCallback, conn))
    return SHOCK_JSON_PRINTER_INIT_ERROR;
  
  if(json_parser_init(&conn->json_parser, NULL, shockJParseCallback, conn))
    return SHOCK_JSON_PARSER_INIT_ERROR;
  
  curl_easy_setopt(conn->curl_handle, CURLOPT_USERNAME, conf->username);
  curl_easy_setopt(conn->curl_handle, CURLOPT_PASSWORD, conf->password);
  curl_easy_setopt(conn->curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  curl_easy_setopt(conn->curl_handle, CURLOPT_WRITEFUNCTION, shockCurlWriteHandler);
  curl_easy_setopt(conn->curl_handle, CURLOPT_WRITEDATA, conn);
    
  return 0;
}

void shockSetDataHandler(ShockConnection *conn, ShockDataType data_type, FILE *dataDest){
  assert( NULL != conn );
  conn -> output_settings -> output_type = data_type;
  conn -> output_settings -> output_handle = dataDest;
}


int shockGetNodes(ShockConnection *conn){
  assert(NULL != conn);
  char *server = conn->shock_config.server;
  char *node_s = "/node";
  char *url = malloc(strlen(server)+strlen(node_s)+1);
  strcat(url,server);
  strcat(url,node_s);

  if(conn->shock_config.debug)
    fprintf(stderr,"DEBUG shock url : %s\n", url);

  curl_easy_setopt(conn->curl_handle, CURLOPT_URL, url);
  int ret = shockCurlExecute(conn);
  free(url);
  return ret;
}

int shockGetUsers(ShockConnection *conn){
  assert( NULL != conn);
  
  char *user_s = "/user";
  char *server = conn->shock_config.server;
  char *url = malloc(strlen(server) + strlen(user_s) + 1);
  strcat(url,server);
  strcat(url,user_s);
  
  if(conn->shock_config.debug)
    fprintf(stderr,"DEBUG shock url : %s\n", url);

  curl_easy_setopt(conn->curl_handle, CURLOPT_URL, url);
  int ret = shockCurlExecute(conn);
  free(url);
  return ret;
}

int shockGetNode(ShockConnection *conn, const char *node_id){
  assert( NULL != conn);
  
  char *node_s = "/node/";
  char *server = conn->shock_config.server;
  char *url = malloc(strlen(node_s) + 
		     strlen(server) + 
		     strlen(node_id) + 1);
  strcat(url,server);
  strcat(url,node_s);
  strcat(url,node_id);
  
  if(conn->shock_config.debug)
    fprintf(stderr,"DEBUG shock url : %s\n", url);

  curl_easy_setopt(conn->curl_handle, CURLOPT_URL, url);
  int ret = shockCurlExecute(conn);
  free(url);
  return ret;
}

int shockQueryNode(ShockConnection *conn, const char *queries[], int num_queries){
  assert( NULL != conn);
  assert(num_queries > 0);

  char *server = conn->shock_config.server;
  char *nodeq_s = "/node/?query&";
  int query_lengths = 0;
  int i;
  for(i=0; i<num_queries; ++i){
    query_lengths += strlen(queries[i]);
  }
  
  char *url = malloc( strlen(server) + 
		      strlen(nodeq_s) +
		      query_lengths +
		      (num_queries - 1) + // for the '&'
		      1);
  strcat(url, server);
  strcat(url, nodeq_s);
  for(i=0;i<num_queries;++i){
    strcat(url, queries[i]);
    if( (num_queries-1) != i)
      strcat(url, "&");
  }

  if(conn->shock_config.debug)
    fprintf(stderr,"DEBUG shock url : %s\n", url);
  
  curl_easy_setopt(conn->curl_handle, CURLOPT_URL, url);
  int ret = shockCurlExecute(conn);
  free(url);
  return ret;

}

int shockDownloadFile(ShockConnection *conn, const char *node_id){
  assert( NULL != conn );
  assert( NULL != node_id );
  
  char *server = conn -> shock_config.server;
  char *node_s = "/node/";
  char *download_s = "?download";
  char *url = malloc( strlen(server) + 
		      strlen(node_s) + 
		      strlen(download_s) + 
		      strlen(node_id) + 
		      1);
  strcat(url,server);
  strcat(url,node_s);
  strcat(url,node_id);
  strcat(url,download_s);

  if(conn->shock_config.debug)
    fprintf(stderr,"DEBUG shock url : %s\n", url);
  
  curl_easy_setopt(conn->curl_handle, CURLOPT_URL, url);
  int ret = shockCurlExecute(conn);
  free(url);
  return ret;
}


int shockConnectionFree(ShockConnection *conn){
  assert(NULL != conn);
  shockOutputSettingsFree(conn -> output_settings);
  curl_easy_cleanup(conn->curl_handle);
  json_print_free(&conn->json_printer);
  json_parser_free(&conn->json_parser);
  return 0;
}

ShockOutputSettings *shockOutputSettingsNewDefault(){
  ShockOutputSettings *settings = malloc(sizeof(ShockOutputSettings));
  settings -> output_type =  SHOCK_JSON;
  settings -> output_handle = stdout;
  return settings;
}

void shockOutputSettingsFree(ShockOutputSettings *settings){
  assert(NULL != settings);
  free(settings);
}
