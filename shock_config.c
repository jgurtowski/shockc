#include <stdlib.h>
#include <assert.h>

#include "shock_config.h"
#include "shock_error.h"

int getShockConfigFromEnvironment(ShockConfig *config){
  assert(NULL != config);
  if(NULL == (config->username = getenv("SHOCK_USERNAME")))
    return SHOCK_MISSING_USERNAME;
  if(NULL == (config->password = getenv("SHOCK_PASSWORD")))
    return SHOCK_MISSING_PASSWORD;
  if(NULL == (config->server = getenv("SHOCK_SERVER")))
    return SHOCK_MISSING_SERVER;

  return 0;
}
