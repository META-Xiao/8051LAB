/*
   ESP8266 WIFI + DS18B20 温度透传
   ESP8266: VCC-5V  GND-GND  RXD-P0.4  TXD-P0.5  RST-P0.7
   DS18B20: DQ-P0.2  VCC/GND-3.3V/GND
*/
#include "config.h"
#include "stdio.h"

#define WIFI_SSID      "YourWiFiName"
#define WIFI_PWD       "YourWiFiPassword"
#define SERVER_IP      "192.168.1.100"
#define SERVER_PORT    8080
#define SEND_INTERVAL  3000

static void delayMs(uint ms)
{
    uchar j;
    while (ms--) {
        for (j = 0; j < 250; j++);
    }
}

void main(void)
{
    uint temp;
    uint lastTemp = 0xFFFF;
    uint sendCount = 0;
    uchar ret;
    WiFiConfig xdata cfg;

    sysInit();
    ds18b20Init();
    esp8266Init(115200);
    delayMs(1000);

    printf("\r\n=== ESP8266 WiFi + DS18B20 ===\r\n");

    printf("AT test...\r\n");
    ret = esp8266TestAT();
    if (!ret) {
        printf("AT FAIL! Hardware reset...\r\n");
        esp8266HardReset();
        ret = esp8266TestAT();
        if (!ret) {
            printf("No response from ESP8266. Halted.\r\n");
            while (1);
        }
    }
    printf("AT OK\r\n");

    cfg.ssid       = WIFI_SSID;
    cfg.password   = WIFI_PWD;
    cfg.serverIP   = SERVER_IP;
    cfg.serverPort = SERVER_PORT;

    printf("WiFi: %s\r\n", WIFI_SSID);
    printf("Server: %s:%d\r\n", SERVER_IP, SERVER_PORT);

    ret = esp8266Config(&cfg);
    if (!ret) {
        printf("WiFi config FAILED!\r\n");
        while (1);
    }
    printf("=== Transparent Mode ON ===\r\n\r\n");

    while (1) {
        ds18b20Start();
        delayMs(800);
        temp = ds18b20Read();

        if (temp != lastTemp || sendCount % 10 == 0) {
            printf("{\"t\":%u.%02u}\r\n", temp / 100, temp % 100);
            lastTemp = temp;
        }
        sendCount++;
        delayMs(SEND_INTERVAL - 800);
    }
}
