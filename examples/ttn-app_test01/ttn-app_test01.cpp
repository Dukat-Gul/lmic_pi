
/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello, world!", that
 * will be processed by The Things Network server.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in g1, 
*  0.1% in g2). 
 *
 * Change DEVADDR to a unique address! 
 * See http://thethingsnetwork.org/wiki/AddressSpace
 *
 * Do not forget to define the radio type correctly in config.h, default is:
 *   #define CFG_sx1272_radio 1
 * for SX1272 and RFM92, but change to:
 *   #define CFG_sx1276_radio 1
 * for SX1276 and RFM95.
 *
 *******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include <lmic.h>
#include <hal.h>
#include <local_hal.h>

void debug_event(ev_t e) {
  if ( e==EV_SCAN_TIMEOUT   ) { fprintf(stdout, "DEBUG: event EV_SCAN_TIMEOUT\n"   ); }
  if ( e==EV_BEACON_FOUND   ) { fprintf(stdout, "DEBUG: event EV_BEACON_FOUND\n"   ); }
  if ( e==EV_BEACON_MISSED  ) { fprintf(stdout, "DEBUG: event EV_BEACON_MISSED\n"  ); }
  if ( e==EV_BEACON_TRACKED ) { fprintf(stdout, "DEBUG: event EV_BEACON_TRACKED\n" ); }
  if ( e==EV_JOINING        ) { fprintf(stdout, "DEBUG: event EV_JOINING\n"        ); }
  if ( e==EV_JOINED         ) { fprintf(stdout, "DEBUG: event EV_JOINED\n"         ); }
  if ( e==EV_RFU1           ) { fprintf(stdout, "DEBUG: event EV_RFU1\n"           ); }
  if ( e==EV_JOIN_FAILED    ) { fprintf(stdout, "DEBUG: event EV_JOIN_FAILED\n"    ); }
  if ( e==EV_REJOIN_FAILED  ) { fprintf(stdout, "DEBUG: event EV_REJOIN_FAILED\n"  ); }
  if ( e==EV_TXCOMPLETE     ) { fprintf(stdout, "DEBUG: event EV_TXCOMPLETE\n"     ); }
  if ( e==EV_LOST_TSYNC     ) { fprintf(stdout, "DEBUG: event EV_LOST_TSYNC\n"     ); }
  if ( e==EV_RESET          ) { fprintf(stdout, "DEBUG: event EV_RESET\n"          ); }
  if ( e==EV_RXCOMPLETE     ) { fprintf(stdout, "DEBUG: event EV_RXCOMPLETE\n"     ); }
  if ( e==EV_LINK_DEAD      ) { fprintf(stdout, "DEBUG: event EV_LINK_DEAD\n"      ); }
  if ( e==EV_LINK_ALIVE     ) { fprintf(stdout, "DEBUG: event EV_LINK_ALIVE\n"     ); }
  fflush(stdout);
}

void debug_txrxFlags(u1_t f) {
  if ( ( f & TXRX_ACK    ) == TXRX_ACK    ) { fprintf(stdout, "DEBUG: txrxFlags TXRX_ACK\n"   ); };
  if ( ( f & TXRX_NACK   ) == TXRX_NACK   ) { fprintf(stdout, "DEBUG: txrxFlags TXRX_NACK\n"  ); };
  if ( ( f & TXRX_NOPORT ) == TXRX_NOPORT ) { fprintf(stdout, "DEBUG: txrxFlags TXRX_NOPORT\n");  };
  if ( ( f & TXRX_PORT   ) == TXRX_PORT   ) { fprintf(stdout, "DEBUG: txrxFlags TXRX_PORT\n"  );  };
  if ( ( f & TXRX_DNW1   ) == TXRX_DNW1   ) { fprintf(stdout, "DEBUG: txrxFlags TXRX_DNW1\n"  );  };
  if ( ( f & TXRX_DNW2   ) == TXRX_DNW2   ) { fprintf(stdout, "DEBUG: txrxFlags TXRX_DNW2\n"  );  };
  if ( ( f & TXRX_PING   ) == TXRX_PING   ) { fprintf(stdout, "DEBUG: txrxFlags TXRX_PING\n"  );  };
  fflush(stdout);
}

void debug_lmic(lmic_t l) {
  fprintf(stdout, "DEBUG: LMIC:\n" );
  fprintf(stdout, "DEBUG: - frame ........ ?\n" );
  fprintf(stdout, "DEBUG: - dataLen ...... %u\n", l.dataLen );
  fprintf(stdout, "DEBUG: - dataBeg ...... %u\n", l.dataBeg );
  fprintf(stdout, "DEBUG: - txCnt ........ %u\n", l.txCnt );
  fprintf(stdout, "DEBUG: - txrxFlags .... %u\n", l.txrxFlags );

  fprintf(stdout, "DEBUG: - pendTxPort ... %u\n", l.pendTxPort );
  fprintf(stdout, "DEBUG: - pendTxConf ... %u\n", l.pendTxConf );
  fprintf(stdout, "DEBUG: - pendTxLen .... %u\n", l.pendTxLen );
  fprintf(stdout, "DEBUG: - pendTxData ... ?\n" );
  fprintf(stdout, "DEBUG: - bcnChnl ...... %u\n", l.bcnChnl );
  fprintf(stdout, "DEBUG: - bcnRxsyms .... %u\n", l.bcnRxsyms );
  fprintf(stdout, "DEBUG: - bcnRxtime .... ?\n" );
  fprintf(stdout, "DEBUG: - bcnInfo ...... ?\n" ); 
  fflush(stdout);
}

// LoRaWAN Application identifier (AppEUI)
// Not used in this example
static const u1_t APPEUI[8]  = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x03, 0xFE, 0x74 };

// LoRaWAN DevEUI, unique device ID (LSBF)
// Not used in this example
static const u1_t DEVEUI[8]  = { 0x00, 0x60, 0x59, 0xE0, 0xEF, 0xF9, 0x49, 0xA3 };

// LoRaWAN NwkSKey, network session key 
// Use this key for The Things Network
static const u1_t DEVKEY[16] = { 0x0C, 0x00, 0xF9, 0x0B, 0x50, 0x16, 0x07, 0xE0, 0xCB, 0x00, 0x90, 0x5C, 0x00, 0x10, 0xEE, 0x02 };

// LoRaWAN AppSKey, application session key
// Use this key to get your data decrypted by The Things Network
static const u1_t ARTKEY[16] = { 0x0A, 0x50, 0x50, 0x01, 0x40, 0x07, 0x06, 0x00, 0xE3, 0x0D, 0xE0, 0xF2, 0x09, 0xD0, 0x4D, 0x04 };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
static const u4_t DEVADDR = 0x2600212B; // <-- Change this address for every node!

//////////////////////////////////////////////////
// APPLICATION CALLBACKS
//////////////////////////////////////////////////

// provide application router ID (8 bytes, LSBF)
void os_getArtEui (u1_t* buf) {
    memcpy(buf, APPEUI, 8);
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) {
    memcpy(buf, DEVEUI, 8);
}

// provide device key (16 bytes)
void os_getDevKey (u1_t* buf) {
    memcpy(buf, DEVKEY, 16);
}

u4_t cntr=0;
u1_t mydata[] = {"Hello, world!                               "};
static osjob_t sendjob;

// Pin mapping
lmic_pinmap pins = {
  .nss = 6,
  .rxtx = UNUSED_PIN, // Not connected on RFM92/RFM95
  .rst = 0,  // Needed on RFM92/RFM95
  .dio = {7,4,5}
};

void onEvent (ev_t ev) {
  debug_event(ev);

  switch(ev) {
    // scheduled data sent (optionally data received)
    // note: this includes the receive window!
    case EV_TXCOMPLETE:
      // use this event to keep track of actual transmissions
      debug_lmic( LMIC );
      fprintf(stdout, "Event EV_TXCOMPLETE, time: %d\n", millis() / 1000);
      fflush(stdout);
      if(LMIC.dataLen) { // data received in rx slot after tx
        //debug_buf(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
        fprintf(stdout, "Data Received!\n");
        fflush(stdout);
      }
      fprintf( stdout, "DEBUG: txrxFlags %u\n", LMIC.txrxFlags );
      debug_txrxFlags( LMIC.txrxFlags );
      break;
    default:
      break;
  }
}

static void do_send(osjob_t* j){
  time_t t=time(NULL);
  fprintf(stdout, "[%x] (%ld) %s\n", hal_ticks(), t, ctime(&t));
  // Show TX channel (channel numbers are local to LMIC)
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & (1 << 7)) {
    fprintf(stdout, "OP_TXRXPEND, not sending\n");
  } else {
    // Prepare upstream data transmission at the next possible time.
    char buf[100];
    sprintf(buf, "%ld", t );
    //sprintf(buf, "Hello World! [%d]",  cntr++ );
    int i=0;
    while(buf[i]) {
      mydata[i]=buf[i];
      i++;
    }
    mydata[i]='\0';
    //LMIC.pendTxConf = 1;
    fprintf(stdout, "DEBUG: pendTxConf %u\n", LMIC.pendTxConf);
    LMIC_setTxData2(1, mydata, strlen(buf), 0);
  }
  // Schedule a timed job to run at the given timestamp (absolute system time)
  os_setTimedCallback(j, os_getTime()+sec2osticks(20), do_send);
         
}

void setup() {
  // LMIC init
  wiringPiSetup();

  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  // Set static session parameters. Instead of dynamically establishing a session 
  // by joining the network, precomputed session parameters are be provided.
  LMIC_setSession (0x1, DEVADDR, (u1_t*)DEVKEY, (u1_t*)ARTKEY);
  // Disable data rate adaptation
  LMIC_setAdrMode(0);
  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // Disable beacon tracking
  LMIC_disableTracking ();
  // Stop listening for downstream data (periodical reception)
  LMIC_stopPingable();
  // Set data rate and transmit power (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7,14);
  //
}

void loop() {
  do_send(&sendjob);
  while(1) {
    os_runloop();
    //  os_runloop_once();
  }
}


int main() {
  setup();
  while (1) {
    loop(); // MAIN LOOP
  }
  // NEVER EXECUTES
  return 0;
}

