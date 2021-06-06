#include "hardware_interface.h"

namespace HW {

const std::unordered_map<Pin,std::string,EnumHash> pinNames = {
    { Pin::MOTOR0_PIN0,     "Motor0 Pin0" },
    { Pin::MOTOR0_PIN1,     "Motor0 Pin1" },
    { Pin::MOTOR1_PIN0,     "Motor1 Pin0" },
    { Pin::MOTOR1_PIN1,     "Motor1 Pin1" },
    { Pin::ENCODER0_PIN0,   "Encoder0 Pin0" },
    { Pin::ENCODER0_PIN1,   "Encoder0 Pin1" },
    { Pin::ENCODER1_PIN0,   "Encoder1 Pin0" },
    { Pin::ENCODER1_PIN1,   "Encoder1 Pin1" },
    { Pin::SR04_TRIG,       "SR04 Trigger" },
    { Pin::SR04_ECHO,       "SR04 Echo" },
    { Pin::LED_PIN,         "WS2812 Out" },
};

const std::unordered_map<PinState,std::string,EnumHash> pinStateNames = {
  { PinState::MOTOR_POS,            "Voltage On"      },
  { PinState::MOTOR_NEG,            "Voltage Off"     },
  { PinState::INPUT_LOW,            "Input GND"       },
  { PinState::INPUT_HIGH,           "Input VCC"       },
  { PinState::ECHO_OFF,             "SR04 Echo Off"   },
  { PinState::ECHO_ON,              "SR04 Echo Pulse" }
};

const std::unordered_map<PinIOMode,std::string,EnumHash> pinIOModeNames = {
    { PinIOMode::M_INPUT,        "Input" },
    { PinIOMode::M_OUTPUT,       "Output" }
};

}; // end namespace HW

