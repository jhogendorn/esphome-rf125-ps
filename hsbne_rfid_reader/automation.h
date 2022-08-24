#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "hsbne_rfid_reader.h"

namespace esphome {
namespace hsbne_rfid_reader {

class HsbneRfidReaderTrigger : public Trigger<int> {
 public:
  explicit HsbneRfidReaderTrigger(HsbneRfidReaderComponent *parent) {
    parent->add_on_state_callback([this](int value) { this->trigger(value); });
  }
};

template<typename... Ts> class RfidReaderPublishAction : public Action<Ts...> {
 public:
  RfidReaderPublishAction(HsbneRfidReaderTrigger *rfid_read) : hsbne_rfid_reader_(rfid_read) {}
  CallbackManager<void(int)> callback_;      ///< Storage for filtered state callbacks.
  TEMPLATABLE_VALUE(int, state)

  void play(Ts... x) override { this->hsbne_rfid_reader_->publish_state(this->state_.value(x...)); }

 protected:
  HsbneRfidReaderComponent *hsbne_rfid_reader_;
};

}  // namespace hsbne_rfid_reader
}  // namespace esphome