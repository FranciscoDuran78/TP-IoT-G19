#pragma once

#include <HardwareController.h>

class ThingSpeakService {
 public:
  ThingSpeakService();

  bool publishNow(const TelemetryData &data);
  void publishIfDue(const TelemetryData &data);
  String lastError() const;

 private:
  unsigned long lastPublishMs_;
  String lastError_;
};
