import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome import automation
from esphome.const import (
    CONF_ID,
    CONF_ON_VALUE,
    CONF_TRIGGER_ID
)

# Define the i2c component as a dependency
DEPENDENCIES = ["i2c"]

#Create namespace object for RFID reader
hsbne_rfid_reader_ns = cg.esphome_ns.namespace("hsbne_rfid_reader")

#Create object for class
HsbneRfidReaderComponent = hsbne_rfid_reader_ns.class_(
    "HsbneRfidReaderComponent", cg.PollingComponent, i2c.I2CDevice
)

# Define rfid read trigger
HsbneRfidReaderTrigger = hsbne_rfid_reader_ns.class_(
    "HsbneRfidReaderTrigger", automation.Trigger.template(cg.int_)
)

#Define configuration schema
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HsbneRfidReaderComponent),
            # Add on_value: to the schema as an autonomy function to be triggered by HsbneRfidReaderTrigger
            cv.Optional(CONF_ON_VALUE) : automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(HsbneRfidReaderTrigger),
            })
        }
    )
    .extend(cv.polling_component_schema("250ms")) #Add polling component options to schema
    .extend(i2c.i2c_device_schema(0x1B) ) # Add i2c options to schema
)

# Code generation function
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Generate the i2c code
    await i2c.register_i2c_device(var, config)

    for conf in config.get(CONF_ON_VALUE, []):
        # Get the specific instance of the trigger in the configuration file
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)

        # Generate and register the automation callback for the specific trigger
        await automation.build_automation(trigger, [(int, "x")], conf)