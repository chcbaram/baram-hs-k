#include "keyscan.h"


#ifdef _USE_HW_KEYSCAN
#include "button.h"
#include "qbuffer.h"
#include "cli.h"


typedef enum
{
  KEYSCAN_NODE_IDLE,
  KEYSCAN_NODE_PRESSED_CHECK,
  KEYSCAN_NODE_RELEASED_CHECK,
  KEYSCAN_NODE_PRESSED,
  KEYSCAN_NODE_RELEASED,
} KeyscanNodeState_t;


typedef struct
{
  uint8_t  index;
  uint8_t  state;
  uint32_t time_exe; 
  bool     pin_cur;
  bool     pin_pre;
  uint8_t  pressed; 
} keyscan_event_q_t;


typedef struct
{
  KeyscanNodeState_t state_cur;
  KeyscanNodeState_t state_pre;

  uint32_t time_cur;
  uint32_t time_pre;  
  uint32_t time_exe; 
  bool     pin_cur;
  bool     pin_pre;
  uint8_t  pressed; 
} keyscan_node_t;

typedef struct
{
  uint32_t key_cnt;
  uint32_t key_update_cnt;
  keyscan_node_t node[HW_KEYSCAN_MAX_CH];
} keyscan_info_t;


#if CLI_USE(HW_KEYSCAN)
static void cliCmd(cli_args_t *args);
#endif


static keyscan_info_t    info;
static qbuffer_t         keyscan_event_q;
static keyscan_event_q_t keyscan_event_buf[100];




bool keyscanInit(void)
{
  qbufferCreateBySize(&keyscan_event_q, (uint8_t *)keyscan_event_buf, sizeof(keyscan_event_q_t), 100);

  info.key_cnt = HW_KEYSCAN_MAX_CH;
  info.key_update_cnt = 0;

  for (int i=0; i<info.key_cnt; i++)
  {
    info.node[i].state_cur = KEYSCAN_NODE_IDLE;
    info.node[i].state_pre = KEYSCAN_NODE_IDLE;
    info.node[i].pressed = false;
  }
#if CLI_USE(HW_KEYSCAN)
  cliAdd("keyscan", cliCmd);
#endif

  return true;
}

// From button.c
//
void buttonUpdateEvent(void)
{
  keyscanUpdate();
}

void keyscanUpdate(void)
{
  uint32_t time_cur;

  info.key_update_cnt++;
  time_cur = micros();

  for (int i=0; i<info.key_cnt; i++)
  {
    bool is_changed = false;
    info.node[i].time_cur = time_cur;
    info.node[i].pin_pre = info.node[i].pin_cur;
    info.node[i].pin_cur = buttonGetPressed(i);

    switch(info.node[i].state_cur)
    {
      case KEYSCAN_NODE_IDLE:
        info.node[i].pin_pre = info.node[i].pin_cur;
        info.node[i].time_pre = time_cur;
        if (info.node[i].pin_cur == true)
          info.node[i].state_cur = KEYSCAN_NODE_PRESSED_CHECK;
        else
          info.node[i].state_cur = KEYSCAN_NODE_RELEASED_CHECK;
        break;

      case KEYSCAN_NODE_PRESSED_CHECK:
        break;

      case KEYSCAN_NODE_RELEASED_CHECK:
        break;

      case KEYSCAN_NODE_PRESSED:
        break;

      case KEYSCAN_NODE_RELEASED:
        break;
    }
    info.node[i].state_pre = info.node[i].state_cur;
    info.node[i].time_exe = time_cur - info.node[i].time_pre;

    if (info.node[i].pin_pre != info.node[i].pin_cur)
    {
      is_changed = true;
      info.node[i].time_pre = time_cur;
    }

    if (is_changed)
    {
      keyscan_event_q_t event_q;

      event_q.index = i;
      event_q.pin_cur = info.node[i].pin_cur;
      event_q.pin_pre = info.node[i].pin_pre;
      event_q.time_exe = info.node[i].time_exe;

      qbufferWrite(&keyscan_event_q, (uint8_t *)&event_q, 1);
    }
  }
}


#if CLI_USE(HW_KEYSCAN)
void cliCmd(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("update cnt : %d\n", info.key_update_cnt);
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "test"))
  {
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "event"))
  {
    uint32_t event_cnt;

    event_cnt = qbufferAvailable(&keyscan_event_q);
    cliPrintf("event_cnt : %d\n", event_cnt);

    for (int i=0; i<event_cnt; i++)
    {
      keyscan_event_q_t event_q;

      qbufferRead(&keyscan_event_q, (uint8_t *)&event_q, 1);

      cliPrintf("%d \n", i);
      cliPrintf("  index : %d \n", event_q.index);
      cliPrintf("  pin   : %d -> %d\n", event_q.pin_pre, event_q.pin_cur);
      cliPrintf("  time  : %d us, %d ms\n", event_q.time_exe, event_q.time_exe/1000);
    }

    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("keyscan info\n");
    cliPrintf("keyscan test\n");
    cliPrintf("keyscan event\n");
  }
}
#endif

#endif