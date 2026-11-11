#pragma once

#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

#include <HardwareController.h>
#include <ThingSpeakService.h>

class TelegramBotService {
 public:
  TelegramBotService(HardwareController &hardware, ThingSpeakService &thingSpeak);

  void begin();
  void poll();

 private:
  void handleMessage(const String &chatId, const String &messageText);
  void sendStartMenu(const String &chatId);
  void sendStatus(const String &chatId);
  void sendReply(const String &chatId, const String &text);

  WiFiClientSecure secureClient_;
  UniversalTelegramBot bot_;
  HardwareController &hardware_;
  ThingSpeakService &thingSpeak_;
  unsigned long lastPollMs_;
};
