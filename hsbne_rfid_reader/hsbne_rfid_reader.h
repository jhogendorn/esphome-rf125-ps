#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace hsbne_rfid_reader {

/// This class implements support for the BME280 Temperature+Pressure+Humidity i2c sensor.
class HsbneRfidReaderComponent : public PollingComponent, public i2c::I2CDevice {
  public:
    CallbackManager<void(int)> callback_;      ///< Storage for filtered state callbacks.

    // ========== INTERNAL METHODS ==========
    // (In most use cases you won't need these)
    void setup() override;
    void dump_config() override;
    float get_setup_priority() const override;
    void update() override;
    void publish_state(int state);
    void add_on_state_callback(std::function<void(int)> &&callback);

  private:
    uint32_t extract_data(uint64_t raw_data);
    uint8_t check_parity(uint64_t data);
};

}  // namespace hsbne_rfid_reader
}  // namespace esphome