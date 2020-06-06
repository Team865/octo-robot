#include "hardware_interface.h"

const std::unordered_map<HWI::Pin,std::string,EnumHash> HWI::pinNames = {
    { Pin::MOTOR0_FORWARD_REVERSE,  "Motor0 Forward/ Reverse" },
    { Pin::MOTOR0_DRIVE ,           "Motor0 Drive" },
};

const std::unordered_map<HWI::PinState,std::string,EnumHash> HWI::pinStateNames = {
  { PinState::MOTOR_ON,             "Motor On"      },
  { PinState::MOTOR_OFF,            "Motor Off"     },
  { PinState::DIR_FORWARD,          "Forward"       },
  { PinState::DIR_BACKWARD,         "Backward"      },
  { PinState::DUMMY_INACTIVE,       "Temporary Dummy Value" },
};

const std::unordered_map<HWI::PinIOMode,std::string,EnumHash> HWI::pinIOModeNames = {
    { PinIOMode::M_INPUT,        "Input" },
    { PinIOMode::M_OUTPUT,       "Output" }
};

