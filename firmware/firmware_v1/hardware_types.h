#ifndef __HARDWARE_TYPES_H__
#define __HARDWARE_TYPES_H__

#include <unordered_map>
#include "basic_types.h"      // For UrbanRobot::advance

/// @brief Lightweight type class for hardware.
///
/// Examples:  Pin Enums, functions that manipulate the same
///

struct EnumHash
{
  // @brief Type convert any enum type to a size_t for hashing
  template <class T>
  std::size_t operator()(T enum_val) const
  {
    return static_cast<std::size_t>(enum_val);
  }
};

namespace HW {

enum class Pin {
  START_OF_PINS = 0,
  MOTOR0_PIN0 = 0,
  MOTOR0_PIN1,
  MOTOR1_PIN0,
  MOTOR1_PIN1,
  ENCODER0_PIN0,
  ENCODER0_PIN1,
  ENCODER1_PIN0,
  ENCODER1_PIN1,
  SR04_TRIG,
  SR04_ECHO,
  LED_PIN,
  END_OF_PINS 
};

enum class PinState {
  START_OF_PIN_STATES = 0,
  MOTOR_POS = 0,    // Apply voltage to pin
  MOTOR_NEG,        // Don't apply voltage
  INPUT_LOW,        // Some input is 0            
  INPUT_HIGH,       // Some input is 1
  ECHO_OFF,         // sr04 echo is disabled
  ECHO_ON,          // sr04 echo is pulsing
  END_OF_PIN_STATES
};

// Arduino.h uses #define to redefine OUTPUT and INPUT,  so use
//           M_OUTPUT and M_INPUT here.
enum class PinIOMode {
  START_OF_PIN_IO_MODES = 0,
  M_OUTPUT = 0,
  M_INPUT = 1,
  END_OF_IO_MODES
};

extern const std::unordered_map<Pin,std::string,EnumHash> pinNames;
extern const std::unordered_map<PinState,std::string,EnumHash> pinStateNames;
extern const std::unordered_map<PinIOMode,std::string,EnumHash> pinIOModeNames;

}; // end HW namespace

// @brief Increment operator for Hardware Interface Pin
//
// @param[in] pin - The pin to increment. 
// @return - The next value in the Enum.
//
inline HW::Pin& operator++( HW::Pin &pin ) 
{
  return UrbanRobot::advance< HW::Pin, HW::Pin::END_OF_PINS >( pin );
}

// @brief Increment operator for Hardware Interface Pin State
//
// @param[in] pinState - The pin state to increment. 
// @return - The next value in the Enum.
// 
inline HW::PinState& operator++( HW::PinState &pin ) 
{
  return UrbanRobot::advance< HW::PinState, HW::PinState::END_OF_PIN_STATES >( pin );
}

// @brief Increment operator for Hardware Interface Pin IO Mode
//
// @param[in] pinIoMode - The pin IO Mode to increment. 
// @return - The next value in the Enum.
// 
inline HW::PinIOMode& operator++( HW::PinIOMode &pin ) 
{
  return UrbanRobot::advance< HW::PinIOMode, HW::PinIOMode::END_OF_IO_MODES >( pin );
}

#endif

