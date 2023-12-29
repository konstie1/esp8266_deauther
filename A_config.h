#define ENABLE_DEBUG
#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200

#define DEFAULT_ESP8266

#define FORMAT_SPIFFS
#define FORMAT_EEPROM

#define RESET_SETTINGS

#define USE_LED true
#define LED_DIGITAL
#define LED_RGB
#define LED_NEOPIXEL

#define LED_ANODE false

#define LED_PIN_R 16
#define LED_PIN_G 255
#define LED_PIN_B 2

#define LED_NEOPIXEL_RGB
#define LED_NEOPIXEL_GRB

#define LED_NUM 1
#define LED_NEOPIXEL_PIN 255

#define LED_MODE_OFF 0, 0, 0
#define LED_MODE_SCAN 0, 0, 255
#define LED_MODE_ATTACK 255, 0, 0
#define LED_MODE_IDLE 0, 255, 0
#define LED_MODE_BRIGHTNESS 10

#define USE_DISPLAY false
#define DISPLAY_TIMEOUT 600
#define FLIP_DIPLAY false

#define SSD1306_I2C
#define SSD1306_SPI
#define SH1106_I2C
#define SH1106_SPI

#define I2C_ADDR 0x3C
#define I2C_SDA 5
#define I2C_SCL 4

#define SPI_RES 5
#define SPI_DC 4
#define SPI_CS 15

#define BUTTON_UP 255
#define BUTTON_DOWN 255
#define BUTTON_A 255
#define BUTTON_B 255

#define RESET_BUTTON 5

#define WEB_IP_ADDR (192, 168, 4, 1)
#define WEB_URL "deauth.me"

#define DEAUTHER_VERSION "2.6.1"
#define DEAUTHER_VERSION_MAJOR 2
#define DEAUTHER_VERSION_MINOR 6
#define DEAUTHER_VERSION_REVISION 1

#define EEPROM_SIZE 4095
#define BOOT_COUNTER_ADDR 1
#define SETTINGS_ADDR 100

#if LED_MODE_BRIGHTNESS == 0
#error LED_MODE_BRIGHTNESS must not be zero!
#endif /* if LED_MODE_BRIGHTNESS == 0 */
