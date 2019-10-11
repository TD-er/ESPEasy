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


class MyHalConfig_t : public Arduino_LMIC::HalConfiguration_t {
public:

  MyHalConfig_t() {}

  virtual void begin(void) override {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  }
};


// table of LORAWAN MAC commands
typedef struct {
  const uint8_t opcode;
  const char    cmdname[20];
  const uint8_t params;
} mac_t;

struct LMIC_Handler_struct {
  LMIC_Handler_struct();

  ~LMIC_Handler_struct();

  void        reset();

  bool        isInitialized() const;

  esp_err_t   lora_stack_init();
  static void lmictask(void *pvParameters);
  void        onEvent(ev_t ev);
  void        gen_lora_deveui(uint8_t *pdeveui);
  void        RevBytes(unsigned char *b,
                       size_t         c);
  void        get_hard_deveui(uint8_t *pdeveui);
  void        os_getDevKey(u1_t *buf);
  void        os_getArtEui(u1_t *buf);
  void        os_getDevEui(u1_t *buf);
  void        showLoraKeys(void);
  static void lora_send(void *pvParameters);
  static void lora_enqueuedata(MessageBuffer_t *message);
  void        lora_queuereset(void);
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
  const char* getSfName(rps_t rps);
  const char* getBwName(rps_t rps);
  const char* getCrName(rps_t rps);

#if (TIME_SYNC_LORAWAN)
  void        user_request_network_time_callback(void *pVoidUserUTCTime,
                                                 int   flagSuccess);
#endif // if (TIME_SYNC_LORAWAN)


  MyHalConfig_t myHalConfig{};

  // LMIC pin mapping
  const lmic_pinmap lmic_pins = {
    .nss  = LORA_CS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst  = LORA_RST == NOT_A_PIN ? LMIC_UNUSED_PIN : LORA_RST,
    .dio  = { LORA_IRQ, LORA_IO1,
              LORA_IO2 == NOT_A_PIN ? LMIC_UNUSED_PIN : LORA_IO2 },

    // optional: set polarity of rxtx pin.
    .rxtx_rx_active = 0,

    // optional: set RSSI cal for listen-before-talk
    // this value is in dB, and is added to RSSI
    // measured prior to decision.
    // Must include noise guardband! Ignored in US,
    // EU, IN, other markets where LBT is not required.
    .rssi_cal       = 0,

    // optional: override LMIC_SPI_FREQ if non-zero
    .spi_freq = 0,
    .pConfig  = &myHalConfig };


  // FIXME TD-er:  Make this a member, which is being set at init.
  const uint8_t DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  const uint8_t APPEUI[8] = { 0x70, 0xB3, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00 };

  const uint8_t APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
};

#endif // ifndef _LORAWAN_H
