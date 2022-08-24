#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "hsbne_rfid_reader.h"

namespace esphome {
namespace hsbne_rfid_reader {

/**
 * @brief Creates trigger for successful read
 * This is used by the code generator to subscribe callbacks defined in the .yaml file.
 * 
 */
class HsbneRfidReaderTrigger : public Trigger<int> {
 public:
  explicit HsbneRfidReaderTrigger(HsbneRfidReaderComponent *parent) {
    parent->add_on_state_callback([this](int value) { this->trigger(value); });
  }
};

}  // namespace hsbne_rfid_reader
}  // namespace esphome