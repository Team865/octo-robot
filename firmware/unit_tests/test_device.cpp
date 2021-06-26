#include <gtest/gtest.h>

#include "../firmware_v1/hardware_interface.h"
#include "../firmware_v2/wifi_secrets.h"

/// @brief Every Pin in the Pins enum should have a debug name
TEST( DEVICE, should_have_complete_pin_names )
{
  for( HW::Pin pin = HW::Pin::START_OF_PINS; 
       pin < HW::Pin::END_OF_PINS;
       ++pin )
  {
    ASSERT_NE(HW::pinNames.find( pin ), HW::pinNames.end() );
  }
}

/// @brief Every Pin State the PinsState enum should have a debug name
TEST( DEVICE, should_have_complete_pin_state_names )
{
  for( HW::PinState pinState = HW::PinState::START_OF_PIN_STATES; 
       pinState < HW::PinState::END_OF_PIN_STATES;
       ++pinState )
  {
    ASSERT_NE(HW::pinStateNames.find( pinState ), HW::pinStateNames.end() );
  }
}

/// @brief Every IO Mode should have a debug name
TEST( DEVICE, should_have_complete_pin_io_modes )
{
  for( HW::PinIOMode i= HW::PinIOMode::START_OF_PIN_IO_MODES; 
       i < HW::PinIOMode::END_OF_IO_MODES;
       ++i )
  {
    ASSERT_NE(HW::pinIOModeNames.find( i ), HW::pinIOModeNames.end() );
  }
}


