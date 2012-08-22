#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shock_error.h"
#include "shock_config.h"
#include "shock_connection.h"

int printUsage(void){
  printf("Usage: shockc [options] command [parameter1, parameter2...]\n"
	 "   Global Options: \n"
	 "      -d   Debug   \n"
	 "      -j   JSON Output <default>\n"
	 "      -r   RAW Output \n"
	 "      -o   Ouput file <default: stdout> \n"
	 "   Action Commands: \n"
	 "      list-nodes   List all nodes \n"
	 "      list-users   List all users \n"
	 "      create-node  Create a new node \n"
	 "      get-node     Get node by id \n"
	 "      get-file     Download file associated with node \n"
	 "      query-node   Search for a node with given attributes \n"
	 "\n"
	 );
  return 1;
}


int printError(int err, FILE *file){
  fprintf(file,"Error %d : %s\n", err, ShockErrorString[err]);
  return err;
}

int main(int argc, char *argv[]){
  if(argc < 2)    
    return printUsage();

  int err = 0; //default is no error 
  ShockConfig s_config;
  memset(&s_config,0,sizeof(ShockConfig));
  
  if((err = getShockConfigFromEnvironment(&s_config)))
    return printError(err,stderr);

  ShockDataType data_type = SHOCK_UNSET;
  
  int c;
  FILE *outfile = stdout;
  while((c = getopt(argc, argv, "djro:")) != -1){
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
    case 'o':
      outfile = fopen(optarg, "w");
      if(NULL == outfile){
	fprintf(stderr, "Error opening file %s\n", optarg);
	return printUsage();
      }
      break;
    case '?':
      if(optopt == 'o')
	fprintf(stderr, "Option -o requires an argument\n");
      return printUsage();
    }
  }

  int first_optind = optind;
  optind = 1;
  if(first_optind >= argc){
    fprintf(stderr, "Missing command\n");
    return printUsage();
  }

  ShockConnection s_conn;
  if((err = shockConnectionNew(&s_conn,&s_config)))
    return printError(err,stderr);

  if(data_type != SHOCK_UNSET)
    shockSetDataHandlerType(&s_conn, data_type);
  shockSetDataHandlerFile(&s_conn, outfile);
  
  if(0 == strcmp(argv[first_optind],"list-nodes")){
    if((err =shockGetNodes(&s_conn)))
      printError(err,stderr);
  }else if(0 == strcmp(argv[first_optind],"list-users")){
    if((err = shockGetUsers(&s_conn)))
      printError(err,stderr);
  }else if(0 == strcmp(argv[first_optind],"create-node")){
    if( argc - first_optind <= 1 ){
      fprintf(stderr,"shockc [options] create-node "
	      "[-e (for empty node) ][-a attributes.json] [-u upload_file.txt]");
      err = -1;
    }else{
      char *attributes = NULL;
      char *upload_file = NULL;
      int t;
      int parse_error = 0;
      while((t = getopt(argc-first_optind, argv+first_optind, "ea:u:")) != -1){
	switch(t){
	case 'a':
	  attributes = optarg;
	  break;
	case 'u':
	  upload_file = optarg;
	  break;
	case '?':
	  parse_error = 1;
	  err = -1;
	}
      }
      if(!parse_error){
	if((err = shockCreateNode(&s_conn, upload_file, attributes)))
	  printError(err,stderr);
      }
    }
  }else if(0 == strcmp(argv[first_optind], "query-node")){
    const char **queries = malloc(sizeof(char *));
    int num_queries = 0;
    if( argc - first_optind <= 1 ){
      fprintf(stderr,"shockc [options] query-node -q query1,query2,query3...\n");
      err = -1;
    }else{
      int t;
      int parse_error;
      char *my_token;
      while((t = getopt(argc-first_optind, argv+first_optind, "q:")) != -1){
	switch(t){
	case 'q':
	  my_token = strtok(optarg,",");
	  while(NULL != my_token){
	    queries[num_queries++] = my_token;
	    queries = realloc(queries, sizeof(char *) * num_queries + 1);
	    my_token = strtok(NULL, ",");
	  }
	  break;
	case '?':
	  parse_error = 1;
	  err = -1;
	}
      }
      if(!parse_error)
	if((err = shockQueryNode(&s_conn, queries, num_queries)))
	  printError(err,stderr);
      free(queries);
    }
  }else if(0 == strcmp(argv[first_optind],"get-file")){
    //unless otherwise specified, file download is of type raw rather than json
    if(data_type == SHOCK_UNSET)
      shockSetDataHandlerType(&s_conn, SHOCK_RAW);
    
    if( argc - first_optind <= 1 ){
      fprintf(stderr,"shockc [options] get-file -i node_id\n");
      err = -1;
    }else{
      int t, parse_error;
      char *node_id;
      while((t = getopt(argc-first_optind, argv+first_optind, "i:")) != -1){
	switch(t){
	case 'i':
	  node_id = optarg;
	  break;
	case '?':
	  parse_error = 1;
	  err = -1;
	}
      }
      if(!parse_error)
	if((err = shockDownloadFile(&s_conn,node_id)))
	  printError(err,stdout);
    }
  }else if(0 == strcmp(argv[first_optind],"get-node")){
      if( argc - first_optind <= 1 ){
	fprintf(stderr,"shockc [options] get-node -i node_id\n");
	err = -1;
      }else{
	int t, parse_error;
	char *node_id;
	while((t = getopt(argc-first_optind, argv+first_optind, "i:")) != -1){
	  switch(t){
	  case 'i':
	    node_id = optarg;
	    break;
	  case '?':
	    parse_error = 1;
	    err = -1;
	  }
	}
	if(!parse_error)
	  if((err = shockGetNode(&s_conn,node_id)))
	    printError(err,stdout);
      }
  }
  
  if(s_conn.output_settings->output_type == SHOCK_JSON &&
     s_conn.output_settings->output_handle == stdout)
    printf("\n");
  
  shockConnectionFree(&s_conn);
 
  if(stdout != outfile)
    fclose(outfile);

  return err;
}
