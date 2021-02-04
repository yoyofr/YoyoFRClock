#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *weather_appid="95b195d00bdbe160ad206143ee4a05a4";
const char *weather_city="draveil,fr";



const char* WeatherURL="http://api.openweathermap.org/data/2.5/weather?q=";//YOUR_TOWN,uk&APPID=YOUR_APP_ID";
const char* DayForcastURL="http://api.openweathermap.org/data/2.5/forecast?q=";//YOUR_TOWN,uk&APPID=YOUR_APP_ID";

// error codes, must be positive numbers
#define ERROR_WEBSITE 1
#define ERROR_BAD_DATA 2
#define ERROR_NOTENOUGHRAM 3
#define NUM_OF_WEATHERS 100          // NOT CURRENTLY 100 different types but this should cover any changes in future 
#define MPH true                     // if true then show wind speed in miles per hour else metres per second
#define NUM_DAYS_FORCAST 4            // current max is 4, a larger screen might allow more


// Settings for the websites and internet access etc.

const char* WebSiteError="Error from OpenWeather website.";
const char* DataError="Data from web site malformed.";
const char* UnknownError="Unknown error code:";

// global temp vars

float main_temp,main_temp_min,main_temp_max,WindSpeed;
int main_pressure,main_humidity,WindAngle;
String WeatherDesc;
uint16_t WeatherID;
uint16_t ForecastID[4]; // Weather IDs for next four days
uint16_t ThreeHourlyForcastID[4];           // Weather ID's for next 12 hours
String ThreeHourlyForcastTime[4];         // Time for for next 4 x 3 hour weathers (two digits of the hour)
struct tm * DateTime;
char NowTimeStr[80];                      // store human readable date of forcast

// structure to store weather descriptions and their ID's
struct Weather_Struct
{
  uint16_t ID;
  const char* Line1;
  const char* Line2;
  const unsigned char* Gfx;
};


// Graphics for rotsted smaller days of the week names
// 'Monday', 14x31px
const unsigned char MonGfx [] PROGMEM = {
  0x00, 0x00, 0x07, 0xf0, 0x0f, 0xf0, 0x08, 0x00, 0x08, 0x00, 0x0c, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 
  0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0x7f, 0xf0, 0x3e, 0x00, 0x0f, 0xc0, 0x01, 0xf0, 
  0x01, 0xf0, 0x0f, 0xc0, 0x3e, 0x00, 0x7f, 0xf0, 0x7f, 0xf0, 0x00, 0x00, 0x00, 0x00
};
// 'Tuesday', 14x31px
const unsigned char TueGfx [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x07, 0x20, 0x0f, 0x30, 0x09, 0x10, 0x09, 0x10, 0x09, 0x10, 0x0f, 0xf0, 
  0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x00, 0x30, 0x00, 0x10, 0x00, 0x10, 
  0x0f, 0xf0, 0x0f, 0xe0, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x7f, 0xf0, 
  0x7f, 0xf0, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Wednesday', 14x31px
const unsigned char WedGfx [] PROGMEM = {
  0x3f, 0xf0, 0x3f, 0xf0, 0x0c, 0x30, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x20, 0x0f, 0x30, 0x09, 0x10, 0x09, 0x10, 0x09, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 
  0x00, 0x00, 0x30, 0x00, 0x3e, 0x00, 0x0f, 0xc0, 0x01, 0xf0, 0x01, 0xf0, 0x1f, 0xc0, 0x3e, 0x00, 
  0x3e, 0x00, 0x1f, 0xc0, 0x01, 0xf0, 0x01, 0xf0, 0x0f, 0xc0, 0x3e, 0x00, 0x30, 0x00
};
// 'Thu', 14x31px
const unsigned char ThuGfx [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x00, 0x30, 0x00, 0x10, 0x00, 0x10, 0x0f, 0xf0, 
  0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xf0, 0x08, 0x00, 0x08, 0x00, 0x0c, 0x00, 
  0x3f, 0xf0, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x3f, 0xf0, 
  0x3f, 0xf0, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Fri', 14x31px
const unsigned char FriGfx [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0xf0, 0x2f, 0xf0, 
  0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x0c, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x20, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x3f, 0xf0, 0x3f, 0xf0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Sat', 14x31px
const unsigned char SatGfx [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x08, 0x10, 0x3f, 0xf0, 0x3f, 0xf0, 0x08, 0x00, 
  0x00, 0x00, 0x07, 0xf0, 0x0f, 0xf0, 0x09, 0x30, 0x09, 0x10, 0x09, 0x90, 0x0c, 0xf0, 0x04, 0x60, 
  0x00, 0x00, 0x00, 0x00, 0x11, 0xc0, 0x33, 0xe0, 0x62, 0x30, 0x42, 0x10, 0x46, 0x10, 0x44, 0x10, 
  0x64, 0x30, 0x3c, 0x60, 0x1c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Sun', 14x31px
const unsigned char SunGfx [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xf0, 0x08, 0x00, 0x08, 0x00, 0x0c, 0x00, 0x0f, 0xf0, 
  0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x00, 0x30, 0x00, 0x10, 0x00, 0x10, 
  0x0f, 0xf0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x11, 0xc0, 0x33, 0xe0, 0x62, 0x30, 0x42, 0x10, 
  0x46, 0x10, 0x44, 0x10, 0x64, 0x30, 0x3c, 0x60, 0x1c, 0x60, 0x00, 0x00, 0x00, 0x00
};


// Weather Icons
// 'HeavyRain', 31x31px
const unsigned char HeavyRain [] PROGMEM = {
  0x00, 0x3f, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x01, 0xff, 0xe0, 0x00, 
  0x01, 0xff, 0xf0, 0x00, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xe0, 
  0x3f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xfe, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x18, 0x06, 0x31, 0x98, 0x38, 0xce, 0x73, 0xb8, 0x31, 0xcc, 0x63, 0x30, 0x71, 0x8c, 0xe3, 0x30, 
  0x63, 0x80, 0xcc, 0x00, 0x07, 0x00, 0x1c, 0xc0, 0x0e, 0x30, 0xd9, 0xc0, 0x1c, 0x71, 0xd9, 0x80, 
  0x18, 0x61, 0x81, 0x80, 0x00, 0x61, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};


// 'Rain', 31x31px
const unsigned char Rain [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0xff, 0xc0, 0x00, 
  0x01, 0xff, 0xe0, 0x00, 0x01, 0xff, 0xf0, 0x00, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 
  0x0f, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xfc, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfc, 
  0x00, 0x00, 0x00, 0x00, 0x18, 0x06, 0x00, 0x60, 0x38, 0x0e, 0x00, 0xe0, 0x30, 0x0c, 0x00, 0xc0, 
  0x70, 0x0c, 0x18, 0xc0, 0x60, 0xc0, 0x38, 0x00, 0x01, 0xc0, 0x30, 0x00, 0x01, 0x80, 0x30, 0x00, 
  0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'LightRain', 31x31px
const unsigned char LightRain [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x7b, 0x80, 0x00, 0x00, 0xd5, 0xc0, 0x00, 
  0x01, 0xee, 0xe0, 0x00, 0x01, 0x55, 0x70, 0x00, 0x01, 0xbb, 0xbf, 0x80, 0x03, 0x55, 0x5d, 0xc0, 
  0x0e, 0xee, 0xee, 0xe0, 0x3d, 0x55, 0x55, 0x60, 0x7b, 0xbb, 0xbb, 0xa0, 0x55, 0x55, 0x55, 0x7c, 
  0xee, 0xee, 0xee, 0xee, 0xd5, 0x55, 0x55, 0x54, 0xbb, 0xbb, 0xbb, 0xba, 0xd5, 0x55, 0x55, 0x54, 
  0xee, 0xee, 0xee, 0xee, 0xd5, 0x55, 0x55, 0x56, 0xfb, 0xbb, 0xbb, 0xbe, 0x7f, 0xff, 0xff, 0xfc, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x30, 0x60, 0x00, 
  0x00, 0x70, 0x60, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'Cloudy', 31x31px
const unsigned char Cloudy [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0xff, 0xc0, 0x00, 
  0x01, 0xff, 0xe0, 0x00, 0x01, 0xff, 0xf0, 0x00, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 
  0x0f, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xfc, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfc, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'LightCloud', 31x31px
const unsigned char LightCloud [] PROGMEM = {
  0x00, 0x3f, 0x00, 0x00, 0x00, 0x61, 0x80, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x01, 0x80, 0x60, 0x00, 
  0x01, 0x00, 0x20, 0x00, 0x01, 0x00, 0x3f, 0x80, 0x03, 0x00, 0x10, 0x80, 0x0e, 0x00, 0x00, 0xc0, 
  0x38, 0x3f, 0x00, 0x40, 0x60, 0x61, 0x80, 0x40, 0x40, 0x40, 0xc0, 0x7c, 0xc0, 0xc0, 0x40, 0x06, 
  0x80, 0x80, 0x7e, 0x02, 0x81, 0x80, 0x02, 0x02, 0x87, 0x00, 0x02, 0x02, 0x8c, 0x00, 0x03, 0x06, 
  0xd8, 0x00, 0x01, 0xe4, 0x70, 0x00, 0x00, 0x2c, 0x30, 0x00, 0x00, 0x38, 0x10, 0x00, 0x00, 0x20, 
  0x10, 0x00, 0x00, 0x60, 0x18, 0x00, 0x00, 0xc0, 0x0f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'MostlyCloudy', 31x31px
const unsigned char MostlyCloudy [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 
  0x00, 0xff, 0x80, 0x00, 0x01, 0xff, 0x80, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x03, 0xff, 0xfc, 0x00, 
  0x0f, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0x5e, 0x00, 0x3f, 0xfe, 0xaf, 0xc0, 0x3f, 0xfd, 0x57, 0xc0, 
  0x3f, 0xfe, 0xab, 0xe0, 0x3f, 0xfd, 0x57, 0x70, 0x3f, 0xfa, 0xaa, 0xb0, 0x3f, 0xf5, 0x55, 0x50, 
  0x1f, 0xaa, 0xaa, 0xbc, 0x01, 0xd5, 0x55, 0x56, 0x01, 0xaa, 0xaa, 0xaa, 0x01, 0x55, 0x55, 0x54, 
  0x01, 0xaa, 0xaa, 0xaa, 0x01, 0x55, 0x55, 0x56, 0x01, 0xea, 0xaa, 0xae, 0x00, 0x7f, 0xff, 0xfc, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'ScatteredCloud', 31x31px
const unsigned char ScatteredCloud [] PROGMEM = {
  0x00, 0x3f, 0x00, 0x00, 0x00, 0x6b, 0x80, 0x00, 0x00, 0xd5, 0xc0, 0x00, 0x01, 0xaa, 0xe0, 0x00, 
  0x01, 0x55, 0x70, 0x00, 0x01, 0xaa, 0xbf, 0x80, 0x03, 0x55, 0x5d, 0xc0, 0x0e, 0xaa, 0xaa, 0xe0, 
  0x3d, 0x55, 0x55, 0x60, 0x6a, 0xaa, 0xaa, 0xa0, 0x55, 0x57, 0xf5, 0x7c, 0xea, 0xae, 0xba, 0xae, 
  0xd5, 0x5d, 0x5d, 0x54, 0xaa, 0xaa, 0xae, 0xaa, 0xd5, 0x5d, 0x57, 0xd4, 0xaa, 0xba, 0xae, 0xea, 
  0xd5, 0x75, 0x55, 0x76, 0xeb, 0xea, 0xaa, 0xae, 0x7f, 0x55, 0x55, 0x7c, 0x03, 0xaa, 0xaa, 0xae, 
  0x03, 0x55, 0x55, 0x56, 0x02, 0xaa, 0xaa, 0xaa, 0x03, 0x55, 0x55, 0x56, 0x02, 0xaa, 0xaa, 0xae, 
  0x03, 0xd5, 0x55, 0x5c, 0x00, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'ThunderStorm', 31x31px
const unsigned char ThunderStorm [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0xff, 0xc0, 0x00, 
  0x01, 0xff, 0xe0, 0x00, 0x01, 0xff, 0xf0, 0x00, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 
  0x0f, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xf8, 0x7c, 
  0xff, 0xff, 0xf0, 0xfe, 0xff, 0xff, 0xf0, 0xfe, 0xff, 0x0f, 0xe1, 0xfe, 0xfe, 0x1f, 0xe0, 0x7e, 
  0xfe, 0x1f, 0xc0, 0xfe, 0xfc, 0x3f, 0xf9, 0xfe, 0xfc, 0x0f, 0xf3, 0xfe, 0x78, 0x1f, 0xf3, 0xfc, 
  0x0f, 0x30, 0x26, 0x00, 0x02, 0x60, 0x6c, 0x00, 0x06, 0x40, 0x48, 0x00, 0x04, 0xc0, 0xd8, 0x00, 
  0x0d, 0x80, 0xb0, 0x00, 0x09, 0x00, 0xe0, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'Clear', 31x31px
const unsigned char Clear [] PROGMEM = {
  0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x20, 0x01, 0x00, 0x10, 0x10, 0x01, 0x00, 0x20, 
  0x08, 0x01, 0x00, 0x40, 0x04, 0x00, 0x00, 0x80, 0x02, 0x07, 0xe1, 0x00, 0x01, 0x3c, 0x3c, 0x00, 
  0x00, 0x60, 0x06, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x80, 0x01, 0x00, 0x01, 0x80, 0x01, 0x80, 
  0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x7d, 0x00, 0x00, 0xbe, 
  0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x80, 0x01, 0x00, 
  0x00, 0xc0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x01, 0x07, 0xe1, 0x00, 
  0x02, 0x00, 0x00, 0x80, 0x04, 0x01, 0x00, 0x40, 0x08, 0x01, 0x00, 0x20, 0x10, 0x01, 0x00, 0x10, 
  0x20, 0x01, 0x00, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'Fog', 31x31px
const unsigned char Fog [] PROGMEM = {
  0x00, 0x00, 0x08, 0x00, 0x20, 0x06, 0x08, 0x00, 0x24, 0x00, 0x00, 0x48, 0x02, 0x00, 0x00, 0x50, 
  0x00, 0x11, 0x8c, 0x04, 0x00, 0x11, 0x80, 0x04, 0x30, 0x00, 0x00, 0x00, 0x30, 0x60, 0x01, 0x80, 
  0x00, 0x60, 0xc0, 0x00, 0x00, 0x02, 0xc0, 0x20, 0x18, 0x02, 0x0c, 0x20, 0x03, 0x00, 0x0c, 0x00, 
  0x03, 0x18, 0x02, 0x00, 0x00, 0x18, 0x80, 0x00, 0x00, 0x00, 0x04, 0x64, 0x30, 0x00, 0x08, 0x04, 
  0x10, 0x40, 0x80, 0x00, 0x02, 0x41, 0x0c, 0x00, 0x04, 0x00, 0x04, 0x06, 0x20, 0x01, 0x00, 0x80, 
  0x20, 0x02, 0x00, 0x80, 0x16, 0x00, 0x10, 0x20, 0x00, 0x00, 0x90, 0x20, 0x20, 0x21, 0x00, 0x00, 
  0x24, 0x20, 0x00, 0x80, 0x08, 0x00, 0x08, 0x80, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01, 0x00, 0x10, 
  0x30, 0x01, 0x00, 0x48, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00
};

// 'Snow', 31x31px
const unsigned char Snow [] PROGMEM = {
  0x00, 0x33, 0x98, 0x00, 0x02, 0x1b, 0xb0, 0x00, 0x07, 0x0f, 0xe0, 0xc0, 0x07, 0x07, 0xc0, 0x80, 
  0x07, 0x03, 0xc0, 0x80, 0x47, 0x83, 0x80, 0x84, 0x77, 0x81, 0x01, 0xcc, 0x3f, 0x81, 0x01, 0xf8, 
  0x1f, 0x81, 0x01, 0xf0, 0x0f, 0x81, 0x03, 0xe0, 0x1f, 0xc3, 0x87, 0xf0, 0x3f, 0xef, 0xef, 0xf8, 
  0xf0, 0x7f, 0xff, 0x9e, 0x40, 0x3e, 0x78, 0x06, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x18, 0x18, 0x00, 
  0x00, 0x18, 0x18, 0x00, 0xf0, 0x1c, 0x38, 0x1e, 0xf0, 0x3e, 0x7c, 0x1e, 0x7e, 0xef, 0xee, 0xfc, 
  0x7f, 0xcf, 0xc7, 0xf0, 0x07, 0x83, 0x83, 0xe0, 0x0f, 0x83, 0x01, 0xf8, 0x3f, 0x83, 0x01, 0xf8, 
  0xf7, 0x83, 0x01, 0xdc, 0xf7, 0x83, 0x81, 0xc4, 0x07, 0x03, 0x81, 0xc0, 0x07, 0x07, 0xc1, 0xc0, 
  0x07, 0x0f, 0xe1, 0xc0, 0x07, 0x1d, 0xb0, 0x00, 0x00, 0x39, 0x98, 0x00
};


// Vertical time GFx
// '00', 14x31px
const unsigned char Time0 [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 
  0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0
};

// '03', 14x31px
const unsigned char Time3 [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x06, 0xe0, 0x0f, 0xf0, 0x09, 0x10, 0x09, 0x10, 0x0c, 0x30, 0x04, 0x20, 
  0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0
};
// '06', 14x31px
const unsigned char Time6 [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x04, 0xe0, 0x0d, 0xf0, 0x09, 0x10, 0x09, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 
  0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0
};
// '09', 14x31px
const unsigned char Time9 [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x90, 0x08, 0x90, 0x0f, 0xb0, 0x07, 0x20, 
  0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0
};
// '12', 14x31px
const unsigned char Time12 [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x07, 0x10, 0x0f, 0x90, 0x08, 0xd0, 0x08, 0x70, 0x0c, 0x30, 0x04, 0x10, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x06, 0x00, 0x02, 0x00
};
// '15', 14x31px
const unsigned char Time15 [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x08, 0xe0, 0x09, 0xf0, 0x09, 0x10, 0x0d, 0x10, 0x0f, 0x30, 0x03, 0x20, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x06, 0x00, 0x02, 0x00
};
// '18', 14x31px
const unsigned char Time18 [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x06, 0xe0, 0x0f, 0xf0, 0x09, 0x10, 0x09, 0x10, 0x0f, 0xf0, 0x06, 0xe0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x06, 0x00, 0x02, 0x00
};
// '21', 14x31px
const unsigned char Time21 [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xe0, 
  0x0f, 0xf0, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x06, 0x00, 0x02, 0x00, 
  0x00, 0x00, 0x07, 0x10, 0x0f, 0x90, 0x08, 0xd0, 0x08, 0x70, 0x0c, 0x30, 0x04, 0x10
};
// 'BadTime', 14x31px
const unsigned char BadTime [] PROGMEM = {
  0x06, 0x00, 0x0f, 0x00, 0x09, 0xd0, 0x08, 0xd0, 0x0c, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x00, 
  0x0f, 0x00, 0x09, 0xd0, 0x08, 0xd0, 0x0c, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 
  0x02, 0x10, 0x00, 0x00, 0x06, 0x00, 0x0f, 0x00, 0x09, 0xd0, 0x08, 0xd0, 0x0c, 0x00, 0x04, 0x00, 
  0x00, 0x00, 0x06, 0x00, 0x0f, 0x00, 0x09, 0xd0, 0x08, 0xd0, 0x0c, 0x00, 0x04, 0x00
};


// array of weather types, populates in setup (via function)
Weather_Struct Weathers[NUM_OF_WEATHERS];   // change if you use a different weather site or if they add to the number of descriptions
uint8_t WeatherCount=0;                     // number of different weathers, adds up as you add more in the function PopulateWeathers

StaticJsonDocument<1024> Doc;  // 1K is ample (returns aroiund 0.5K)

void weather_setup() {
  PopulateWeathers();
  WeatherReport();        // do the report
}

void WeatherReport()
{
  uint8_t ErrorCode;
  if ((WiFi.status() == WL_CONNECTED)) 
  {
    ErrorCode=GetWeather();
    if(ErrorCode==0)
      DisplayWeather();
    else    
      DisplayWeatherSiteError(ErrorCode);
  }  
}

int16_t IndexOfWeatherID(uint16_t ID)
{
  uint8_t Idx=0;
  while(Idx<WeatherCount)
  {
    if(Weathers[Idx].ID==ID)
      return Idx;
    else
      Idx++;
  }
  // got this far, then not found
  return -1;
}

void PopulateWeathers()
{
  // Weather strings, designed to go over two lines of the display, first is first line, second is second line max chars for the 200px display is 18chars per line
  // Using the supplied ones fed back from the website will still sometimes cause the text to be split badly over two lines and also the descriptions are 
  // sometimes "odd", Replace the text below with whatever you wish, do not change the first param - the ID code, these are unique to the weather app

  // Ids are for OpenWeathermap.org
  AddWeather(200,"Thunderstorm","with light rain",ThunderStorm);
  AddWeather(201,"Thunderstorm","with rain",ThunderStorm);
  AddWeather(202,"Thunderstorm","with heavy rain",ThunderStorm);
  AddWeather(210,"Light thunderstorm","",ThunderStorm);
  AddWeather(211,"Thunderstorm","",ThunderStorm);
  AddWeather(212,"Heavy","Thunderstorm",ThunderStorm);
  AddWeather(221,"Thunderstorm","with ragged cloud",ThunderStorm);
  AddWeather(230,"Thunderstorm","with light drizzle",ThunderStorm);
  AddWeather(231,"Thunderstorm","with drizzle",ThunderStorm);
  AddWeather(232,"Thunderstorm","with heavy drizzle",ThunderStorm);
  
  AddWeather(300,"Light drizzle","",LightRain);
  AddWeather(301,"Drizzle","",LightRain);
  AddWeather(302,"Heavy drizzle","",Rain);
  AddWeather(310,"Light drizzle with","occasional rain",Rain);
  AddWeather(311,"Drizzle with ","some rain",LightRain);
  AddWeather(312,"Heavy drizzle with","occasional rain",Rain);
  AddWeather(313,"Occasional showers","of rain & drizzle",LightRain);
  AddWeather(314,"Heavy rain showers","and some drizzle",Rain);
  AddWeather(321,"Showers of drizzle","",LightRain);

  AddWeather(500,"Light rain","",LightRain);
  AddWeather(501,"Moderate rain","",LightRain);
  AddWeather(502,"Heavy rain","",HeavyRain);
  AddWeather(503,"Very heavy rain","",HeavyRain);
  AddWeather(504,"Extreme rain","",HeavyRain);
  AddWeather(511,"Freezing rain","",HeavyRain);
  AddWeather(520,"Light rain","showers",LightRain);
  AddWeather(521,"Rain showers","",Rain);
  AddWeather(522,"Heavy rain showers","",HeavyRain);
  AddWeather(531,"Rain showers with","ragged cloud",Rain);
  
  AddWeather(600,"Light snow","",Snow);
  AddWeather(601,"Snow","",Snow);
  AddWeather(602,"Heavy snow","",Snow);
  AddWeather(611,"Sleet","",Snow);
  AddWeather(612,"Light sleet","showers",Rain);
  AddWeather(613,"Sleet showers","",Rain);
  AddWeather(615,"Light rain and","snow",Snow);
  AddWeather(616,"Rain and snow","",Snow);
  AddWeather(620,"Light snow showers","",Snow);
  AddWeather(621,"Snow showers","",Snow);
  AddWeather(622,"Heavy snow showers","",Snow);
  
  AddWeather(701,"Misty","",Fog);
  AddWeather(711,"Smoke","",Fog);
  AddWeather(721,"Hazey","",Fog);
  AddWeather(731,"Sand.dust whirls","",Fog);
  AddWeather(741,"Fog","",Fog);
  AddWeather(751,"Sand","",Fog);
  AddWeather(761,"Dust","",Fog);
  AddWeather(762,"Volcanic ash","",Fog);
  AddWeather(771,"Squalls","",Fog);
  AddWeather(781,"Tornado","",Fog);

  AddWeather(800,"Clear Skies","",Clear);
  
  AddWeather(801,"A few clouds","",LightCloud);
  AddWeather(802,"Scattered clouds","",ScatteredCloud);
  AddWeather(803,"Broken clouds but","mostly cloudy",MostlyCloudy);
  AddWeather(804,"Very overcast","clouds",Cloudy);
}

void AddWeather(uint16_t ID,const char* FirstLine,const char* SecondLine,const unsigned char* Gfx)
{
  Weathers[WeatherCount].ID=ID;
  Weathers[WeatherCount].Line1=FirstLine;
  Weathers[WeatherCount].Line2=SecondLine;
  Weathers[WeatherCount].Gfx=Gfx;
  WeatherCount++;
}

void DisplayWeatherSiteError(uint8_t ErrorCode)
{
  const char* ErrorStr;
  bool ShowErrorCode=false;
  
  switch (ErrorCode)
  {
    case ERROR_WEBSITE:ErrorStr=WebSiteError;break;
    case ERROR_BAD_DATA:ErrorStr=DataError;break;
    default: {ErrorStr=UnknownError;ShowErrorCode=true;}
  }
  // print out the error
  Serial.println(ErrorStr);
  // This should not happen unless the programmer has forgot to code an error code in!
  if(ShowErrorCode)
    Serial.println(ErrorCode);
    
}

void DisplayWeather()
{
  static char outstr[20];   // buffer for float strings
  static char convstr[8];
  int16_t WeatherIdx;
  // Print values.

  WeatherIdx=IndexOfWeatherID(WeatherID);
  display.setRotation(3);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawRect(0,0,200,200,GxEPD_BLACK);
    display.drawFastHLine(0,18,200,GxEPD_BLACK);
    display.setCursor(76, 13);
    CentreText(NowTimeStr);
    display.setCursor(2, 32);
    if(WeatherIdx!=-1)
    {
      CentreString(Weathers[WeatherIdx].Line1);
      display.setCursor(2, 48);
      CentreString(Weathers[WeatherIdx].Line2);
    }
    else    
    {
      CentreString("Unknown Weather");
      display.setCursor(2, 48);
      display.print("    Code: ");
      display.print(WeatherID);
    }
    display.setCursor(6, 64);
    outstr[0]=0;
    strcat(outstr,"T:");
    dtostrf(main_temp,1, 0, convstr);
    strcat(outstr,convstr);
    strcat(outstr,"  ");  // place holder for Deg C part
    strcat(outstr," Lo:");
    dtostrf(main_temp_min,1, 0, convstr);
    strcat(outstr,convstr);
    strcat(outstr," Hi:");
    dtostrf(main_temp_max,1, 0, convstr);
    strcat(outstr,convstr);
    uint16_t XPos=CentreString(outstr);
    // Now calculate where the DEg C should be and plot it
    display.setCursor(XPos+33, 64);
    DegC();
    display.setCursor(6, display.getCursorY()+16);
    // wind speed
    outstr[0]=0;
    strcat(outstr,"Wind:");
    dtostrf(WindSpeed,1, 0, convstr);
    strcat(outstr,convstr);
    if(MPH)
      strcat(outstr,"mph,");
    else
      strcat(outstr,"m/s,");
    GetDirectionFromBearing(WindAngle,outstr);
    CentreString(outstr);
    display.setCursor(6, display.getCursorY()+16);
    outstr[0]=0;
    strcat(outstr,"Hum:");
    dtostrf(main_humidity,1, 0, convstr);
    strcat(outstr,convstr);
    strcat(outstr,"% Pres:");
    dtostrf(main_pressure,1, 0, convstr);
    strcat(outstr,convstr);
    CentreString(outstr);

    // display immediate environment values
    display.drawFastHLine(0,200-34,200,GxEPD_BLACK);
    display.drawFastHLine(0,200-33,200,GxEPD_BLACK);
    if(BME280Found)
    {
      outstr[0]=0;
     // display.drawRect(200-34,0,200,200,GxEPD_BLACK);
     
      display.setCursor(6,200-20);
      strcat(outstr,"This room  T:");
      dtostrf(bme.readTemperature(),1, 0, convstr);
      strcat(outstr,convstr);
      display.print(outstr);
      DegC();
      outstr[0]=0;
      strcat(outstr,"Hum:");
      dtostrf(bme.readHumidity(),1, 0, convstr);
      strcat(outstr,convstr);
      strcat(outstr,"%");
      strcat(outstr," Pres:");
      dtostrf(bme.readPressure() / 100.0F,1, 0, convstr);
      strcat(outstr,convstr);
      display.setCursor(0,200-4);
      display.print(outstr);
      display.drawFastHLine(0,200-17,114,GxEPD_BLACK);
      display.drawFastVLine(114,200-33,16,GxEPD_BLACK);
    }
    else
    {
      display.setCursor(0,200-20);
      display.println("BME280 sensor not");
      display.print("found!");
    
    }
    
    // forcast
    // draw the blocks
    for(uint16_t i=0;i<2;i++)
      display.drawFastHLine(0,100+(i*33),200,GxEPD_BLACK);
    for(uint16_t i=0;i<4;i++)
      display.drawFastVLine(i*50,100,66,GxEPD_BLACK);
      
    // display forcast for next few hours (3 hour intervals)    
    for(uint8_t i=0;i<NUM_DAYS_FORCAST;i++)  //NUM_DAYS_FORCAST s same value for number of this days forecasts too
    {
    // put in time of day
      display.drawBitmap(1+(i*50), 101, ReturnTimeGfx(ThreeHourlyForcastTime[i]), 14, 31, GxEPD_BLACK);
    // The forecast gfx
      display.drawBitmap(15+(i*50), 100, Weathers[IndexOfWeatherID(ThreeHourlyForcastID[i])].Gfx, 31, 31, GxEPD_BLACK);
    }
    
    // display forcast for next few days    
    for(uint8_t i=0;i<NUM_DAYS_FORCAST;i++)
    {
    // put in days of the week names
      display.drawBitmap(1+(i*50), 135, ReturnDayGfx(i+(DateTime->tm_wday+1)), 14, 31, GxEPD_BLACK);
    // The forecast gfx
      display.drawBitmap(15+(i*50), 135, Weathers[IndexOfWeatherID(ForecastID[i])].Gfx, 31, 31, GxEPD_BLACK);
    }   
  }
  while (display.nextPage());
  
}

const unsigned char* ReturnTimeGfx(String Time)
{
  // Time will be the hour in 24 hour format, i.e. 00,01,02,etc. up to 23
  
  if(Time=="00") return Time0;
  if(Time=="03") return Time3;
  if(Time=="06") return Time6;
  if(Time=="09") return Time9;
  if(Time=="12") return Time12;
  if(Time=="15") return Time15;
  if(Time=="18") return Time18;
  if(Time=="21") return Time21;
  return BadTime;
  
}

const unsigned char* ReturnDayGfx(uint8_t DayOfWeek)
{
  // where 0 is Sunday, wraps the  number, so that 7 for example would return Sunday.
  DayOfWeek = DayOfWeek % 7;        // This performs the wrap by getting the remainder of the division
                                    // which will be the day number
  switch(DayOfWeek)
  {
    case(0):return SunGfx;break;
    case(1):return MonGfx;break;
    case(2):return TueGfx;break;
    case(3):return WedGfx;break;
    case(4):return ThuGfx;break;
    case(5):return FriGfx;break;
    case(6):return SatGfx;break;
  }
}

uint16_t CentreText(char *TheString)
{
  // works only with the font used
  uint16_t X;
  X=100-float((strlen(TheString)*11)/2);
  display.setCursor(X,display.getCursorY());
  display.print(TheString);
  return X;
}

uint16_t CentreString(String TheString)
{
  // works only with the font used
  uint16_t X;
  X=100-float((TheString.length()*11)/2);
  display.setCursor(X,display.getCursorY());
  display.print(TheString);
  return X;
}

void DegC()
{
  // displays oC at the current location
  display.setCursor(display.getCursorX(),display.getCursorY()-4);
  display.print("o");
  display.setCursor(display.getCursorX(),display.getCursorY()+4);
  display.print("C");
}

uint8_t GetWeather()
{
  // returns 0 if all went well, else an error code
  HTTPClient http;
  time_t UnixTime;
  JsonObject Forcast,WeatherDoc;
  const size_t capacity = 30000;  // enough to handle daily and forecast
 
  char *weatherURL_str;
  weatherURL_str=(char*)malloc(strlen(WeatherURL)+strlen(weather_city)+1+6+strlen(weather_appid)+1);
  if (!weatherURL_str) return ERROR_NOTENOUGHRAM;
  sprintf(weatherURL_str,"%s%s\&APPID=%s",WeatherURL,weather_city,weather_appid);

  http.begin(weatherURL_str);
  free(weatherURL_str);
  int httpCode = http.GET();                                 
  if (httpCode > 0) 
  { 
      String payload = http.getString();      
      
      DynamicJsonDocument  Doc(capacity);                           // create the JSON object passing in the memory required      
      DeserializationError error = deserializeJson(Doc, payload);   // Generate the data from the serialized text data returned from the website
      if (error) {
       // Serial.println(error.c_str());                            // uncomment for debugging data issues
        return ERROR_BAD_DATA;
      }
      WeatherDoc=Doc["weather"][0];
      //WeatherDesc=WeatherDoc["description"];
      // These lines work around an issue wirth strings and the JSON library
      WeatherDesc = (const char*)WeatherDoc["description"];
      WeatherDesc = WeatherDoc["description"].as<const char*>();
      WeatherDesc = WeatherDoc["description"].as<String>();
      WeatherID=WeatherDoc["id"];
      WindSpeed = Doc["wind"]["speed"];
      UnixTime = Doc["dt"];
      
      DateTime =gmtime(&UnixTime);
      // UK format time day of month then month then year, comment out and uncomment US version for different style
      strftime (NowTimeStr,80,"%d/%m/%Y %R",DateTime);
      //US version
      //strftime (NowTimeStr,80,"%m/%d/%Y %R",DateTime);
      if(MPH)
        WindSpeed*=2.237;           // convert to mph of required
      WindSpeed=round(WindSpeed);
      WindAngle = Doc["wind"]["deg"]; 
      JsonObject main = Doc["main"];
      main_temp = main["temp"];
      main_pressure = main["pressure"]; 
      main_humidity = main["humidity"]; 
      main_temp_min = main["temp_min"]; 
      main_temp_max = main["temp_max"]; 
      // convert from Kelvin to Deg C.
      main_temp-=273.15;
      main_temp=round(main_temp);
      main_temp_min-=273.15;
      main_temp_min=round(main_temp_min);
      main_temp_max-=273.15;
      main_temp_max=round(main_temp_max);
  }
  else 
    return ERROR_WEBSITE;

  http.end(); //Free the resources


  

  char *forecastURL_str;
  forecastURL_str=(char*)malloc(strlen(DayForcastURL)+strlen(weather_city)+1+6+strlen(weather_appid)+1);
  if (!forecastURL_str) return ERROR_NOTENOUGHRAM;
  sprintf(forecastURL_str,"%s%s\&APPID=%s",DayForcastURL,weather_city,weather_appid);

  // Get forcast for next four days and next few hours
  http.begin(forecastURL_str);
  free(forecastURL_str);
  httpCode = http.GET();                          
  if (httpCode > 0) 
  { 
    String payload = http.getString();      
    // the line below has been derived by the online tool for calculating the buffer size for the returned data, however as we don't control
    // this return data and it might increase in future I've added 1000 to the end of the line instead of the normal 410 it was suggesting 
    // This is just on case, we don't want any buffer over runs.
    
    DynamicJsonDocument  Doc(capacity);                           // create the JSON object passing in the memory required      
    DeserializationError error = deserializeJson(Doc, payload);   // Generate the data from the serialized text data returned from the website
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());                                // uncomment for debugging data issues
      return ERROR_BAD_DATA;
    }
      
    
    // Get all the different weather forecasts into an array
    JsonArray List = Doc["list"];
    JsonObject Data;
    int16_t WeatherIdx;
    char TimeBuffer[80];
    String ForcastTime;

    // get next 4 three hour forecasts, these are in order in the list return, so easy peasy
    for(uint8_t Idx=0;Idx<NUM_DAYS_FORCAST;Idx++)
    {
      Data=List[Idx];
      WeatherDoc = Data["weather"][0];
      ThreeHourlyForcastID[Idx]=WeatherDoc["id"];      
      // These lines work around an issue wirth strings and the JSON library
      ForcastTime = (const char*)Data["dt_txt"];
      ForcastTime = Data["dt_txt"].as<const char*>();
      ForcastTime = Data["dt_txt"].as<String>();
      ThreeHourlyForcastTime[Idx]=ForcastTime.substring(11,13);
    }

    // Next get next four days forecast,
    for(uint8_t Idx=0;Idx<NUM_DAYS_FORCAST;Idx++)
    {
      ForecastDate(DateTime,1);
      strftime (TimeBuffer,80,"%F 12:00:00",DateTime);
      WeatherIdx=GetWeatherData(List,TimeBuffer);
      if(WeatherIdx>=0)
      {
        Data=List[WeatherIdx];
        WeatherDoc = Data["weather"][0];
        ForecastID[Idx]=WeatherDoc["id"];
        const char* TheDate=Data["dt_txt"];
      }
      else
        return ERROR_WEBSITE;
    }
    DateTime =gmtime(&UnixTime);  // bAck to original
  }
  else 
    return ERROR_WEBSITE;

  http.end(); //Free the resources
  return 0;  // all good
}

void ForecastDate(tm *TheDate,uint8_t DaysInFuture)
{
  const time_t ONE_DAY = 24 * 60 * 60 ;
  
  // Seconds since start of epoch
  time_t date_seconds = mktime( TheDate ) + (DaysInFuture * ONE_DAY) ;
  
  // Update caller's date
  // Use localtime because mktime converts to UTC so may change date
  *TheDate = *localtime( &date_seconds ) ; ;

}

int16_t GetWeatherData(JsonArray List,const char* DateTime)
{
  // given the main data array of weather items returns the index of the data object
  // for the date time passed oin
  // returns -1 if not found
  
  
  JsonObject ThisEntry;
  uint16_t ListSize,ListIdx;
  bool Found=false;
  ThisEntry=List[0];
  const char* TheDate = ThisEntry["dt_txt"]; 
  
  ListIdx=0;
  ListSize=List.size();
 
  while((ListIdx<ListSize)&(Found==false))
  {    
    ThisEntry=List[ListIdx];
    if(strcmp(ThisEntry["dt_txt"],DateTime)==0)
      Found=true;
    else
      ListIdx++;
  }
  
  if(Found)
    return ListIdx;
  else
    return -1;  
}

void GetDirectionFromBearing(uint16_t Bearing,char* Direction)
{
  // returns a text version of the direction bearing in degrees. Note that it only returns
  // 8 basic directions, more finer detail than this can be confusing for average user
  // and really doesn't require that level of detail, feel free to expand to more
  // directions if desired

  // divide degrees by 45
  static char convstr[8];
  uint8_t SimpleDirection=round(Bearing/45);
  switch(SimpleDirection)
  {
    case 0:strcat(Direction,"North");break;
    case 1:strcat(Direction,"North E.");break;
    case 2:strcat(Direction,"East");break;
    case 3:strcat(Direction,"South E.");break;
    case 4:strcat(Direction,"South");break;
    case 5:strcat(Direction,"South W.");break;
    case 6:strcat(Direction,"West.");break;
    case 7:strcat(Direction,"North W.");break;
    case 8:strcat(Direction,"North");break;
    default: {
      strcat(Direction,"???");           // should never get here!
      dtostrf(Bearing,1, 0, convstr);
      strcat(Direction,convstr);
    }
  }
}