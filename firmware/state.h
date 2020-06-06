#ifndef __STATE_H__
#define __STATE_H__

#include <vector>
#include <memory>
#include <string>
#include <assert.h>
#include <unordered_map>
#include "command_parser.h"
#include "net_interface.h"
#include "hardware_interface.h"
#include "histogram.h"
#include "time_interface.h"

#ifdef GTEST_FOUND
#include <gtest/gtest_prod.h>
#endif

#include "action_interface.h"

///
/// @brief SSound Namespace
/// 
namespace FS {

/// 
/// @brief Argument payload for the StateStack class, below
/// 
class StateArg
{
  public: 

  enum class Type {
    NONE,
    INT
  };

  StateArg() : type{ Type::NONE}  {}
  StateArg( int i ) : type{Type::INT}, intArg{ i } {}
  Type getType() { return type; }
  int getInt() { assert( type==Type::INT ); return intArg; }

  private:

  Type type;

  union {
    int intArg;
  };
};

///
/// @brief Stack of StateEnum
///
/// Invariants:
///
/// - In normal operation, the stack's bottom is always an ACCEPT_COMMANDS state
/// - After construction, the stack can never be empty
/// - If a pop operation leaves the stack empty an ERROR_STATE is pushed
/// 
template< class StateEnum, StateEnum defaultState >
class StateStack {
  public:
  
  StateStack()
  {
    push( defaultState, StateArg() );
  }

  /// @brief Reset the stack to the newly initialized state.
  void reset( void )
  {
    while ( stack.size() > 1 ) pop();
  }

  /// @brief Get the top state.
  StateEnum topState( void )
  {
    return stack.back().state;
  }

  /// @brief Get the top state's argumment.
  StateArg topArg( void )
  {
    return stack.back().arg;
  }

  /// @brief Set the top state's argumment.
  void topArgSet( StateArg newVal )
  {
    stack.back().arg = newVal;
  }

  /// @brief Pop the top entry on the stack.
  void pop( void )
  {
    stack.pop_back();
    if ( stack.size() > 10 ) 
    {
      push( StateEnum::ERROR_STATE, StateArg(__LINE__) ); 
    }
    if ( stack.empty() ) 
    {
      // bug, should never happen.
      push( StateEnum::ERROR_STATE, StateArg(__LINE__) ); 
    }
  }

  /// @brief Push a new entry onto the stack
  void push( StateEnum newState, StateArg newArg = StateArg() )
  {
    stack.push_back( { newState , newArg } );
  }  

  /// @brief State to std::string Unordered Map
  using StateToString = std::unordered_map< StateEnum, const std::string, EnumHash >;
  /// @brief The names of each state
  static const StateToString stateNames;

  ///
  /// @brief Does a particular incoming command interrupt the current state
  ///
  /// Example 1.  A "Status" Command will not interrupt a move sequence
  /// Example 2.  A "Home" Command will interrupt a focuser's move sequence
  ///
  
  private:

  typedef struct 
  {
    StateEnum state;   
    StateArg arg; 
  } CommandPacket;

  std::vector< CommandPacket > stack;
};

/// @brief Output StateArg
template <class T,
  typename = my_enable_if_t<is_beefocus_sink<T>::value>>
T& operator<<( T& sink, StateArg sA )
{
  switch (sA.getType() )
  {
    case StateArg::Type::NONE:
      sink << "NoArg";
      break;
    case StateArg::Type::INT:
      sink << sA.getInt();
      break;
    default:
      assert(0);
  }
  return sink; 
}

}

#endif

