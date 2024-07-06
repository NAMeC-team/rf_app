#pragma once

#include <mbed.h>
#include <nrf24l01.h>
#include <radio_command.pb.h>

class RF_app {
   public:

    /**
     * Starting mode for the antenna
     */
    enum class RFAppMode : uint8_t { TX = 0, RX = 1 };

    /**
     * Interrupts to enable when activating antenna
     */
    enum class RFAppInterrupt : uint8_t { on_RX = 0, on_TX = 1, on_RX_TX = 2, None = 3 };

    RF_app(NRF24L01 *device, RF_app::RFAppMode rf_mode = RFAppMode::TX,
           uint16_t frequency = 2402, uint8_t *Tx_addr = NULL,
           uint8_t packet_size = 32);

    // virtual ~RF_app();

    void setup(RFAppMode rf_mode,RFAppInterrupt enable_interrupt,
               uint16_t frequency, uint8_t *Tx_addr, uint8_t packet_size);

    /**
     * Runs the private event queue in a separate thread
     */
    void run(void);

    /**
     * Stops the private event queue
     */
    void exit(void);

    void print_setup();

    /**
     * Change the callback to call when new data has been
     * received by the antenna (RX_DR is asserted)
     */
    void attach_rx_callback(
        Callback<void(uint8_t *packet, size_t length)> rx_callback);

    /**
     * Set a new callback function to call once transmission
     * data has been sent
     */
    void attach_tx_ds_callback(
        Callback<void()> tx_ds_callback);

    /**
     * Reads the oldest packet loaded in the antenna's RX data fifo
     */
    void get_rx_packet();

   protected:
    void _process(void);

    /**
     * Callback function directly attached to the underlying
     * NRF24L01 library upon creation
     */
    void _rf_callback(void);

    /**
     * Used as a TX_DS (Transmission Data Sent) callback,
     * sets CE=0 to switch to Standby-1 mode,
     * then sets PRIM_RX=1 and CE=1, and waits 130μs to
     * get into RX mode. Maintains CE=1 when finished
     */
    void _switch_to_rx(void);

   private:

    /**
     * Current mode of the antenna
     */
    RFAppMode _mode;

    /**
     * Underlying library object for the NRF24L01 module
     */
    NRF24L01 *_device;

    /**
     * Thread used for the private event queue handling the antenna
     */
    Thread _thread;

    /**
     * Event queue used for events coming from the antenna
     */
    EventQueue _event_queue;

    /**
     * Callback that is called once a packet has been
     * received by the antenna (RX_DR asserted)
     * This function is not called directly, a packet is read first
     * then we call it using the newly read packet
     */
    Callback<void(uint8_t *packet, size_t length)> _rx_callback;

    /**
     * Callback that is called once data has been transmitted
     * by the antenna (TX_DS asserted). The function gets called directly
     */
    Callback<void()> _tx_ds_callback;

    /**
     * Packet received by the antenna
     */
    uint8_t _packet[RadioCommand_size];
};
