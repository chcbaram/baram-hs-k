#include "keyscan.h"


#ifdef _USE_HW_KEYSCAN
#include "cli.h"





#if CLI_USE(HW_KEYSCAN)
static void cliCmd(cli_args_t *args);
#endif







bool keyscanInit(void)
{

#if CLI_USE(HW_KEYSCAN)
  cliAdd("keyscan", cliCmd);
#endif

  return true;
}



#if CLI_USE(HW_KEYSCAN)
void cliCmd(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "info"))
  {
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "test"))
  {
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("keyscan info\n");
    cliPrintf("keyscan test\n");
  }
}
#endif

#endif