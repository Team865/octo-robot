#ifndef __TIME_INTERFACE_H__
#define __TIME_INTERFACE_H__

class TimeInterface {
  public:

  virtual unsigned int secondsSince1970() = 0;
  virtual unsigned int msSinceDeviceStart() = 0;
};

#endif


