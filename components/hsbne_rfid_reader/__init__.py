import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome import automation
from esphome.const import (
    CONF_ID,
    CONF_ON_VALUE,
    CONF_TRIGGER_ID
)

DEPENDENCIES = ["i2c"]

#Create namespace object for RFID reader
hsbne_rfid_reader_ns = cg.esphome_ns.namespace("hsbne_rfid_reader")

#Create object for class
HsbneRfidReaderComponent = hsbne_rfid_reader_ns.class_(
    "HsbneRfidReaderComponent", cg.PollingComponent, i2c.I2CDevice
)

# Triggers
HsbneRfidReaderTrigger = hsbne_rfid_reader_ns.class_(
    "HsbneRfidReaderTrigger", automation.Trigger.template(cg.int_)
)

HsbneRfidReaderPublishAction = hsbne_rfid_reader_ns.class_("HsbneRfidReaderPublishAction", automation.Action)

#Define configuration schema
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HsbneRfidReaderComponent),
            cv.Optional(CONF_ON_VALUE) : automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(HsbneRfidReaderTrigger),
            })
        }
    )
    .extend(cv.polling_component_schema("250ms"))
    .extend(i2c.i2c_device_schema(0x1B) )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    for conf in config.get(CONF_ON_VALUE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(int, "x")], conf)