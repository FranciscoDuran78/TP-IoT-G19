#pragma once

#include <HardwareController.h>

class ThingSpeakService {
 public:
  ThingSpeakService();

  bool publishNow(const TelemetryData &data);
  void publishIfDue(const TelemetryData &data);

 private:
  unsigned long lastPublishMs_;
};
