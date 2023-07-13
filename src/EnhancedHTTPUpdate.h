/**
 *
 * This is a fork of ESP32 core HTTPUpdate library. The original library do not allow OTA on FAT 
 * partitions, while the base Update.h does. This library implement a modification to 
 * HTTPUpdate::handleUpdate in order to allow OTA on FAT partitions.
 *
 * IMPORTANT NOTE: The binary file downloaded from server MUST ALREADY HAVE 4096 bytes set to 0 before
 * the actual filesystem binary, otherwise the file system will fail to mount!
 *
 * Original HTTPUpdate library by Markus Sattler.
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPUpdate
 *
 */

#ifndef ___ENHANCED_HTTP_UPDATE_H___
#define ___ENHANCED_HTTP_UPDATE_H___

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <Update.h>

/// note we use HTTP client errors too so we start at 100
#define HTTP_UE_TOO_LESS_SPACE              (-100)
#define HTTP_UE_SERVER_NOT_REPORT_SIZE      (-101)
#define HTTP_UE_SERVER_FILE_NOT_FOUND       (-102)
#define HTTP_UE_SERVER_FORBIDDEN            (-103)
#define HTTP_UE_SERVER_WRONG_HTTP_CODE      (-104)
#define HTTP_UE_SERVER_FAULTY_MD5           (-105)
#define HTTP_UE_BIN_VERIFY_HEADER_FAILED    (-106)
#define HTTP_UE_BIN_FOR_WRONG_FLASH         (-107)
#define HTTP_UE_NO_PARTITION                (-108)

enum HTTPUpdateResult {
    HTTP_UPDATE_FAILED,
    HTTP_UPDATE_NO_UPDATES,
    HTTP_UPDATE_OK
};

typedef HTTPUpdateResult t_httpUpdate_return; // backward compatibility

using HTTPUpdateStartCB = std::function<void()>;
using HTTPUpdateRequestCB = std::function<void(HTTPClient*)>;
using HTTPUpdateEndCB = std::function<void()>;
using HTTPUpdateErrorCB = std::function<void(int)>;
using HTTPUpdateProgressCB = std::function<void(int, int)>;

class HTTPUpdate
{
public:
    HTTPUpdate(void);
    HTTPUpdate(int httpClientTimeout);
    ~HTTPUpdate(void);

    void rebootOnUpdate(bool reboot)
    {
        _rebootOnUpdate = reboot;
    }
    
    /**
      * set redirect follow mode. See `followRedirects_t` enum for avaliable modes.
      * @param follow
      */
    void setFollowRedirects(followRedirects_t follow)
    {
        _followRedirects = follow;
    }

    void setLedPin(int ledPin = -1, uint8_t ledOn = HIGH)
    {
        _ledPin = ledPin;
        _ledOn = ledOn;
    }

    t_httpUpdate_return update(WiFiClient& client, const String& url, const String& currentVersion = "", HTTPUpdateRequestCB requestCB = NULL);

    t_httpUpdate_return update(WiFiClient& client, const String& host, uint16_t port, const String& uri = "/",
                               const String& currentVersion = "", HTTPUpdateRequestCB requestCB = NULL);

    t_httpUpdate_return updateSpiffs(WiFiClient& client, const String& url, const String& currentVersion = "", HTTPUpdateRequestCB requestCB = NULL);

    t_httpUpdate_return update(HTTPClient& httpClient,
                               const String& currentVersion = "", 
                               HTTPUpdateRequestCB requestCB = NULL);

    t_httpUpdate_return updateSpiffs(HTTPClient &httpClient, const String &currentVersion = "", HTTPUpdateRequestCB requestCB = NULL);

    // Notification callbacks
    void onStart(HTTPUpdateStartCB cbOnStart)          { _cbStart = cbOnStart; }
    void onEnd(HTTPUpdateEndCB cbOnEnd)                { _cbEnd = cbOnEnd; }
    void onError(HTTPUpdateErrorCB cbOnError)          { _cbError = cbOnError; }
    void onProgress(HTTPUpdateProgressCB cbOnProgress) { _cbProgress = cbOnProgress; }

    int getLastError(void);
    String getLastErrorString(void);

protected:
    t_httpUpdate_return handleUpdate(HTTPClient& http, const String& currentVersion, bool spiffs = false, HTTPUpdateRequestCB requestCB = NULL);
    bool runUpdate(Stream& in, uint32_t size, String md5, int command = U_FLASH);

    // Set the error and potentially use a CB to notify the application
    void _setLastError(int err) {
        _lastError = err;
        if (_cbError) {
            _cbError(err);
        }
    }
    int _lastError;
    bool _rebootOnUpdate = true;
private:
    int _httpClientTimeout;
    followRedirects_t _followRedirects;

    // Callbacks
    HTTPUpdateStartCB    _cbStart;
    HTTPUpdateEndCB      _cbEnd;
    HTTPUpdateErrorCB    _cbError;
    HTTPUpdateProgressCB _cbProgress;

    int _ledPin;
    uint8_t _ledOn;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_HTTPUPDATE)
extern HTTPUpdate httpUpdate;
#endif

#endif /* ___ENHANCED_HTTP_UPDATE_H___ */
