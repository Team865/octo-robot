#ifndef __TEMP_INTERFACE_H__
#define __TEMP_INTERFACE_H__

class TempInterface {
  public:

  virtual float readTemperature() = 0;
  virtual float readHumidity() = 0;
};

#endif


