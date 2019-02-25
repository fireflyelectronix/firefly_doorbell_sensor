/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title><p style=\"text-align:center\"><img src=\"data:image/png;base64,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\" /></p>";
const char HTTP_STYLE[] PROGMEM           = "<style>h3{text-align:center} .c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#717174;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure Device</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>save</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Credentials Saved<br />Trying to connect ESP to network.<br />If it fails reconnect to AP to try again</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter {
  public:
    /**
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~WiFiManagerParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();
    ~WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP

    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);

    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       configPortalHasTimeout();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    int                    _max_params;
    WiFiManagerParameter** _params;

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
