#ifndef _LORAWAN_H
#define _LORAWAN_H

#include "../../ESPEasy_common.h"
#include "../LoRa/LoRa_common.h"

// LMIC-Arduino LoRaWAN Stack
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <arduino_lmic_hal_boards.h>

// Needed for 24AA02E64, does not hurt anything if included and not used
#ifdef MCP_24AA02E64_I2C_ADDRESS
# include <Wire.h>
#endif // ifdef MCP_24AA02E64_I2C_ADDRESS


// table of LORAWAN MAC commands
typedef struct {
  const uint8_t opcode;
  const char    cmdname[20];
  const uint8_t params;
} mac_t;

class LMIC_Handler_t {
public:

  LMIC_Handler_t();

  ~LMIC_Handler_t();

  void        reset();

  bool        isInitialized() const;

  esp_err_t   lora_stack_init();

private:
  static void lora_setupForNetwork(bool preJoin);
  static void lmictask(void *pvParameters);
  void        gen_lora_deveui(uint8_t *pdeveui);
  void        RevBytes(unsigned char *b,
                       size_t         c);
  void        get_hard_deveui(uint8_t *pdeveui);
  void        os_getDevKey(u1_t *buf);
  void        os_getArtEui(u1_t *buf);
  void        os_getDevEui(u1_t *buf);
  void        showLoraKeys(void);
  static void lora_send(void *pvParameters);


public:


  static void lora_enqueuedata(MessageBuffer_t *message);
  void        lora_queuereset(void);

private:
  static void myEventCallback(void *pUserData,
                              ev_t  ev);
  static void myRxCallback(void          *pUserData,
                           uint8_t        port,
                           const uint8_t *pMsg,
                           size_t         nMsg);
  static void myTxCallback(void *pUserData,
                           int   fSuccess);
  static void mac_decode(const uint8_t cmd[],
                         const uint8_t cmdlen,
                         const mac_t   table[],
                         const uint8_t tablesize);
  uint8_t     getBattLevel(void);

public:
  const char* getSfName(rps_t rps);
  const char* getBwName(rps_t rps);
  const char* getCrName(rps_t rps);

#if (TIME_SYNC_LORAWAN)
  void        user_request_network_time_callback(void *pVoidUserUTCTime,
                                                 int   flagSuccess);
#endif // if (TIME_SYNC_LORAWAN)

private:

  // FIXME TD-er:  Make this a member, which is being set at init.
  const uint8_t DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  const uint8_t APPEUI[8] = { 0x70, 0xB3, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00 };

  const uint8_t APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
};

#endif // ifndef _LORAWAN_H
