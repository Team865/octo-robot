#ifndef __WIFI_SECRETS_H__
#define __WIFI_SECRETS_H__

//
// This file contains wifi credentials.  Separate file because this 
// information will need to be modified during development, but wifi
// details shouldn't be checked into a public repository.
// 

namespace WifiSecrets {
  // Hostname
	constexpr const char* hostname = "urban-octo";	     
  // Default SSID for the robot's Access Point
	constexpr const char* ssid     = "urbanocto";	     
  // Default Password for the Robot's Access Point
  // 
  // TODO - Creating some kind of default web page so custom configurations
  //        could be done would be nice.
  //
	constexpr const char* password = "graciousp";   
}

#endif

