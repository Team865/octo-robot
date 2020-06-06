#include "hardware_interface.h"

const std::unordered_map<HWI::Pin,std::string,EnumHash> HWI::pinNames = {
    { Pin::MICROPHONE, "Microphone" },
};

const std::unordered_map<HWI::PinState,std::string,EnumHash> HWI::pinStateNames = {
    //{ PinState::STEP_ACTIVE,     "Step Active" },
  { PinState::DUMMY_INACTIVE,     "place holder" }
};

const std::unordered_map<HWI::PinIOMode,std::string,EnumHash> HWI::pinIOModeNames = {
    { PinIOMode::M_INPUT,        "Input" },
    { PinIOMode::M_OUTPUT,       "Output" }
};

