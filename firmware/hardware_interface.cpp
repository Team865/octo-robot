#include "hardware_interface.h"

const std::unordered_map<HWI::Pin,std::string,EnumHash> HWI::pinNames = {
    { Pin::MOTOR0_PIN0,   "Motor0 Pin0" },
    { Pin::MOTOR0_PIN1,   "Motor0 Pin1" },
};

const std::unordered_map<HWI::PinState,std::string,EnumHash> HWI::pinStateNames = {
  { PinState::MOTOR_POS,            "Voltage On"      },
  { PinState::MOTOR_NEG,            "Voltage Off"     },
  { PinState::DUMMY_INACTIVE,       "Temporary Dummy Value" },
};

const std::unordered_map<HWI::PinIOMode,std::string,EnumHash> HWI::pinIOModeNames = {
    { PinIOMode::M_INPUT,        "Input" },
    { PinIOMode::M_OUTPUT,       "Output" }
};

