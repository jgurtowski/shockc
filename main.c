#include <stdio.h>
#include <string.h>

#include "shock_error.h"
#include "shock_config.h"
#include "shock_connection.h"

int printError(int err, FILE *file){
  fprintf(file,"Error %d : %s\n", err, ShockErrorString[err]);
  return err;
}

int main(void){

  int err;
  ShockConfig s_config;
  memset(&s_config,0,sizeof(ShockConfig));
  s_config.debug = 1;
  if(err = getShockConfigFromEnvironment(&s_config))
    return printError(err,stderr);
  
  ShockConnection s_conn;
  if(err = shockConnectionNew(&s_conn,&s_config))
    return printError(err,stderr);
    
  shockSetDataHandler(&s_conn, SHOCK_RAW, stdout);
  
  if(err = shockDownloadFile(&s_conn, "95bfc6a3b46f0b903490f00ac3b4be75"))
    return printError(err,stderr);
  
  shockConnectionFree(&s_conn);
  
  printf("\n");
  return 0;
}
