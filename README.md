# EnhancedHTTPUpdate

This is a fork of ESP32 core HTTPUpdate library. The original library do not allow OTA on FAT partitions, while the base Update.h does. This library implement a modification to HTTPUpdate::handleUpdate in order to allow OTA on FAT partitions.

NOTE: The binary file downloaded from server MUST ALREADY HAVE 4096 bytes set to 0 before the actual filesystem binary, otherwise the file system will fail to mount!

[Original HTTPUpdate library](https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPUpdate) by Markus Sattler.
