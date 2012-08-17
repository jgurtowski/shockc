#ifndef SHOCK_CONFIG_H
#define SHOCK_CONFIG_H

typedef struct ShockConfig_struct{
  char *server;
  char *username;
  char *password;
  int debug;
}ShockConfig;

int getShockConfigFromEnvironment(ShockConfig *config);

#endif

