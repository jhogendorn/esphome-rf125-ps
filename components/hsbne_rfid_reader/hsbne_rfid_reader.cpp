#include "hsbne_rfid_reader.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include <stdint.h>

namespace esphome {
namespace hsbne_rfid_reader {

  static const char *const TAG = "hsbne_rfid_reader";

  /**
   * @brief setup() is called once for the fuction. Currently unused.
   * 
   */
  void HsbneRfidReaderComponent::setup() {

  }


  /**
   * @brief Displays a log of the config.
   * 
   */
  void HsbneRfidReaderComponent::dump_config() {
    ESP_LOGCONFIG(TAG, "HSBNE_RFID_READER:");
    LOG_I2C_DEVICE(this);
    LOG_UPDATE_INTERVAL(this);
  }

  float HsbneRfidReaderComponent::get_setup_priority() const { return setup_priority::DATA; }

  /**
   * @brief Update() is run a set frequency as part of the PollingComponent parent
   * The frequency can be changed in the configuration yaml for a esphome device.
   */
  void HsbneRfidReaderComponent::update() {

    uint8_t data[8];
    if (!this->read_bytes_raw(data, 8)) {
      ESP_LOGD(TAG, "error");
      this->status_set_warning();
      return;
    }

    uint64_t data_buffer = 
      (((uint64_t) data[7]) << 0) |
      (((uint64_t) data[6]) << 8) |
      (((uint64_t) data[5]) << 16) |
      (((uint64_t) data[4]) << 24) |
      (((uint64_t) data[3]) << 32) |
      (((uint64_t) data[2]) << 40) |
      (((uint64_t) data[1]) << 48) |
      (((uint64_t) data[0]) << 56);

    if (check_parity(data_buffer) == 1) {
        uint32_t card_number = extract_card_number(data_buffer);
        publish_state(card_number);
        ESP_LOGD(TAG, "Rfid read: %i", (int) card_number);
    }

  }

  /**
   * @brief Perform parity check on incoming rfid data
   * The data contains an 8 bit customer id header for the rfid card
   * followed by a 32-bit card number. This function checks both row
   * and column parity of this data. Every 5th bit is the even parity
   * bit for the preceding 4 bits. The last 5 bits consists of 4 even
   * column parity bits followed by a stop bit.
   * @param data the 64-bits read in from the rfid reader
   */
  uint8_t HsbneRfidReaderComponent::check_parity(uint64_t data) {
    if (((data >> 56) & 0xFF) != 0xFF) {
      return 0;
    }

    //Extract data buffer
    for (int i = 1; i <= 10; i++) {
      uint8_t parity_bit = 0;
      for (int j = 1; j < 5; j++) {
        parity_bit ^= ((data >> (5 * i + j)) & 1);
      }

      // If parity bit does not match, reset read and return
      if (parity_bit != ((data >> (5 * i)) & 1)) {
        return 0;
      }
    }

    // Return true if column parity check is correct
    return (((data >> 51) & 0xF)
          ^ ((data >> 46) & 0xF)
          ^ ((data >> 41) & 0xF)
          ^ ((data >> 36) & 0xF)
          ^ ((data >> 31) & 0xF)
          ^ ((data >> 26) & 0xF)
          ^ ((data >> 21) & 0xF)
          ^ ((data >> 16) & 0xF)
          ^ ((data >> 11) & 0xF)
          ^ ((data >> 6) & 0xF)
          ^ ((data >> 1) & 0xF)) == 0;
  }

  /**
   * @brief Extract the card number from the full rfid data
   * Takes the full data package and extract the card_number by removing the header
   * and parity bits.
   * @param raw_data full rfid data
   * @return uint32_t card number
   */

  uint32_t HsbneRfidReaderComponent::extract_card_number(uint64_t raw_data) {
  return ((raw_data >> 41) & 0xF) << 28
        | ((raw_data >> 36) & 0xF) << 24
        | ((raw_data >> 31) & 0xF) << 20
        | ((raw_data >> 26) & 0xF) << 16
        | ((raw_data >> 21) & 0xF) << 12
        | ((raw_data >> 16) & 0xF) << 8
        | ((raw_data >> 11) & 0xF) << 4
        | ((raw_data >> 6) & 0xF);
  }

  /**
   * @brief Subscribes on_value calls to the card read callback.
   * This is part of the autonomy funcionality of esphome.
   * @param callback
   */
  void HsbneRfidReaderComponent::add_on_state_callback(std::function<void(int)> &&callback) {
    this->callback_.add(std::move(callback));
  }

  /**
   * @brief Calls the callback function for a successful card read
   * 
   * @param state 
   */
  void HsbneRfidReaderComponent::publish_state(int state) {
    this->callback_.call(state);
  }


}  // namespace hsbne_machine_interlock
}  // namespace esphome