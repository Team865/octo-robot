#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__

#include <array>

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
  /// extern const char deviceTimeMSTag[];
  /// using DeviceTimeMS = 
  ///     UrbanRobot::TypeSafeNumber< unsigned int, deviceTimeMSTag >;
  ///
  /// Creates a new type, "DeviceTimeMS", that mostly acts like an unsigned int,
  /// but won't implicitly convert to and from unsigned int, or another 
  /// guarded type.
  ///
  /// TODO - I should probably just make tag a class, and do something like
  ///
  /// struct DeviceTimeMSTag {};
  /// using DeviceTimeMS = 
  ///     UrbanRobot::TypeSafeNumber< unsigned int, deviceTimeMSTag >;
  ///
  /// TODO - I wonder if I could specify an base template and inherit
  /// cast functions.  
  /// 
  template< typename BaseNumber, const char* tag >
  class TypeSafeNumber
  {
    public:

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
    TypeSafeNumber& operator+=(TypeSafeNumber rhs ) { num += rhs.num; return *this; }

    private:
      BaseNumber num;
  };
};



#endif

