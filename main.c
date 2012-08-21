#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "shock_error.h"
#include "shock_config.h"
#include "shock_connection.h"

int printUsage(void){
  /** put usage **/
  return 1;
}


int printError(int err, FILE *file){
  fprintf(file,"Error %d : %s\n", err, ShockErrorString[err]);
  return err;
}

int main(int argc, char *argv[]){
  if(argc < 2)    
    return printUsage();

  int err;
  ShockConfig s_config;
  memset(&s_config,0,sizeof(ShockConfig));
  
  if(err = getShockConfigFromEnvironment(&s_config))
    return printError(err,stderr);

  ShockDataType data_type = SHOCK_JSON;

  int c;
  char *filename = NULL;
  while((c = getopt(argc, argv, "djrf:")) != -1){
    switch(c){
    case 'd':
      s_config.debug = 1;      
      break;
    case 'j':
      data_type = SHOCK_JSON;
      break;
    case 'r':
      data_type = SHOCK_RAW;
      break;
    case 'f':
      filename = optarg;
      break;
    case '?':
      if(optopt == 'f')
	fprintf(stderr, "Option -f requires an argument\n");
      return printUsage();
    }
  }

  int index;
  for(index = optind; index<argc; index++){
    printf("found: %s",argv[index]);
  }
  

  return 0;
  ShockConnection s_conn;
  if(err = shockConnectionNew(&s_conn,&s_config))
    return printError(err,stderr);
    
  shockSetDataHandler(&s_conn, SHOCK_RAW, stdout);

  if(err = shockDownloadFile(&s_conn,"dcf55d7ed1b2988ece91e7e7d119c30a"))
    return printError(err,stdout);

  //if(err = shockCreateNode(&s_conn, "test.data", NULL))
  //return printError(err, stdout);
  
  shockConnectionFree(&s_conn );
  
  printf("\n");
  return 0;
}
