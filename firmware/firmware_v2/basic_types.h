#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__

#include <array>

//#define OCTO_ESP8266_DEBUG

// Building on a PC, for test purposes.  Blank out the flag that tells the
// compiler to put a function into ram
#ifdef PC_BUILD
#define OCTO_INTERRUPT_FUNC( a ) a
#else
// Build target is the ESP8266.  Bring in the hardware_esp8266.h include to 
// make sure ICACHE_RAM_ATTR is available then #define it to the flag we'll
// use to say that a function or method has to be implemented in ram.
#include "ESP8266WiFi.h"
#define OCTO_INTERRUPT_FUNC( a ) a ICACHE_RAM_ATTR
#endif

namespace UrbanRobot
{
	const int noValue= -1;
	using IpAddress = std::array<int,4>;

  ///
  /// @brief Add 1 to an enum from an enum class (wee hacky)
  ///
  /// @param[in] ENUM     - the Enum Type
  /// @param[in] MAX_VAL  - the maximum value in the enum class
  /// @param[in] e        - the enum to increment.
  ///
  template< typename ENUM, ENUM MAX_VAL >
  ENUM& advance(ENUM& e )
  {
    if ( e != MAX_VAL )
    {
      // wee hacky
      e = static_cast<ENUM>( static_cast<int>(e) + 1 );
    }
    return e;
  }

  ///
  /// @brief For making sure your number based types are actually typesafe
  ///
  /// This was motivated by variables that represent a time of some kind.
  /// These variables are all integers of some kind, but, ideally, you want
  /// the compiler to stop you from assignment a variable that represents
  /// "time in milliseconds" to a variable that represents "time in seconds"
  /// That's probably a bug 
  ///
  /// @param[in] BaseNumber   The type of the number we're wrapping
  /// @param[in] tag          A tag that uniquely identifies the new class
  ///
  /// Usage example:
  ///
  /// struct DeviceTimeMSTag {};
  /// using DeviceTimeMS = 
  ///     UrbanRobot::TypeSafeNumber< unsigned int, deviceTimeMSTag >;
  ///
  /// Creates a new type, "DeviceTimeMS", that mostly acts like an unsigned int,
  /// but won't implicitly convert to and from unsigned int, or another 
  /// guarded type.  DeviceTimeMSTag is a structure who's sole job is to help
  /// distinguish "DeviceTimeMS" from other instantiations of the 
  /// TypeSafeNumber template - it's a common C++ meta programm tecnnique
  ///
  /// TODO - I wonder if I could specify an base template and inherit
  /// cast functions.  
  /// 
  template< typename BaseNumberArg, class tag >
  class TypeSafeNumber
  {
    public:

    using BaseNumber = BaseNumberArg;
    TypeSafeNumber( ) : num{ 0 } {}

    /// @brief Constructor.  Don't allow implicit conversions. 
    explicit TypeSafeNumber( BaseNumber numArg ) : num{ numArg } {}

    /// @brief Get the wrapped number.
    BaseNumber get() const { return num; }

    // Wrapper McWrapperson Functions.  TODO - incomlete

    bool operator<( TypeSafeNumber rhs ) const { return num < rhs.num; }
    // todo - should probably be size_t
    BaseNumber operator-( TypeSafeNumber rhs ) const { return num - rhs.num; }
    TypeSafeNumber operator+( BaseNumber rhs ) const { return TypeSafeNumber( num + rhs ); }
    bool operator==( TypeSafeNumber rhs ) const { return num == rhs.num; };
    bool operator!=( TypeSafeNumber rhs ) const { return num != rhs.num; };
    bool operator>(TypeSafeNumber rhs ) const { return num > rhs.num; };
    bool operator>=(TypeSafeNumber rhs ) const { return num >= rhs.num; };
    TypeSafeNumber& operator+=(TypeSafeNumber rhs ) { num += rhs.num; return *this; }
    TypeSafeNumber operator*( int rhs ) const { return TypeSafeNumber( num * rhs ); }

    private:
      BaseNumber num;
  };
};



#endif

