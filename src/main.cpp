/////// MP3: folders (01, 02, 03, ...) can have max 2999 files in mode 3

// Enter your WiFi setup here
// TODO To be moved to sdcard ini file
#include "SPIFFS.h"

#define BUFF_HTML_SIZE 1024
char small_buff[BUFF_HTML_SIZE];

#define _DICT_PACK_STRUCTURES
//#define _PP(a)
//#define _PL(a)
#include <Dictionary.h>


void config_savetosd(void);
void update_clock_color(void);
void task_fftLoop(void *parameter);


#define GIFS_ANIM_DIR_MAX 32 //MAX 32 directories

enum {
  ALARM_OFF=0,
  ALARM_SHOULD_RING,
  ALARM_RINGING,
  ALARM_REPEAT,
  ALARM_STOPPED
};

char clock_gif_animation_on_hours[24]={0/*0*/,0,0/*2*/,0,0/*4*/,0,0/*6*/,1,1/*8*/,1,1/*10*/,1,1/*12*/,1,1/*14*/,1,1/*16*/,1,1/*18*/,1,1/*20*/,1,0/*22*/,0};
char clock_mp3_authorized_hours[24]={0/*0*/,0,0/*2*/,0,0/*4*/,0,0/*6*/,1,1/*8*/,1,1/*10*/,1,1/*12*/,1,1/*14*/,1,1/*16*/,1,1/*18*/,1,1/*20*/,1,0/*22*/,0};
char clock_brightness_hours[24]={12/*0*/,12,12/*2*/,12,12/*4*/,12,12/*6*/,16,16/*8*/,24,24/*10*/,32,32/*12*/,32,32/*14*/,32,32/*16*/,32,32/*18*/,24,24/*20*/,16,16/*22*/,16};
char clock_gifs_anim_dir_activate[GIFS_ANIM_DIR_MAX];

char clock_alarm_weekday_on[7]={0/*sunday*/,1,1,1,1,1,0/*saturday*/};
char clock_alarm_status;
int clock_alarm_mp3_index;
char clock_alarm_weekday_hour[7]={7,20,7,7,7,7,7};
char clock_alarm_weekday_minute[7]={15,10,15,15,15,15,15};
//////////////////////////////////////////////////////////////
//index of alarm mp3 to play when alarm rings
//-1 is random, should be limited to 2 digits, i.e. 99 alarms files
int clock_alarm_weekday_mp3_index[7]={-1,-1,-1,-1,-1,-1,-1};

//#define SILENCE_MP3
#define CLOCK_DEFAULT_MP3_VOLUME 15
#define CLOCK_DEFAULT_ALARM_VOLUME 15
#define BTN5 5

#define PANEL_RBG

#ifdef PANEL_RBG
#define YRGB_COLOR(func,a,b,c) func(a,c,b)
#else
#define YRGB_COLOR(func,a,b,c) func(a,b,c)
#endif

#define WIFI_ON
//#define RTC_OFF

#define DRAW_MSG_LENGTH 64

#define DS3231_SCL 9
#define DS3231_SDA 10

#define RX1 25
#define TX1 26



#define DEFAULT_BRIGHTNESS 16

#define CLOCK_DEFAULT_HOSTNAME "YOYOFRCK"
char clk_global_hostname[32];

#define CLOCK_DELAY_BEFORE_ANIM 12
int clk_global_display_time_duration;

#define CLOCK_GLOBAL_CLOCK_HHMM_STD 0
#define CLOCK_GLOBAL_CLOCK_HHMMSS_STD 1
#define CLOCK_GLOBAL_CLOCK_HHMM_MORPH 2
#define CLOCK_GLOBAL_CLOCK_HHMMSS_MORPH 3
int clk_global_clock_mode=0;

#define CLOCK_MODE_TIME_FONT1 0
#define CLOCK_MODE_TIME_MORPHING 1

#define CLOCK_MODE_MP3_NO_AUDIO 0
#define CLOCK_MODE_MP3_PLAY_LIMITED_NB 1
#define CLOCK_MODE_MP3_TIMER 2
#define CLOCK_MODE_MP3_ALARM 3

#define CLOCK_MODE_MP3_TIMER_DELAY 60*30  //in seconds
int clk_global_mp3_timermode_duration;

#define MP3_MUSIC_FOLDER 1
#define MP3_ALARM_FOLDER 8
#define MP3_AUDIOBOOK_FOLDER 9

#define CLOCK_DISPLAY_BOOTANIM 0
#define CLOCK_DISPLAY_TIME_WITH_BG_ANIM 1
#define CLOCK_DISPLAY_TIME 2
#define CLOCK_DISPLAY_GIFANIM 3
#define CLOCK_DISPLAY_AUDIO_SPECTRUM 4
#define CLOCK_DISPLAY_APNG 5

#define CLOCK_DISPLAY_DEFAULT_STARTUP CLOCK_DISPLAY_BOOTANIM //AUDIO_SPECTRUM //

#define GIF_ANIM_NOCLOCK 1
#define GIF_ANIM_CLOCK 2

#define CLOCK_BUTTON_NB 5

#define MP3_MUSIC 0
#define MP3_AUDIOBOOK 1
#define MP3_ALARM 2

#define CLOCK_COLOR_MODE_NB 5

#define DEFAULT_CLOCK_MODE_TIME CLOCK_MODE_TIME_FONT1
#define DEFAULT_CLOCK_MODE_MP3 CLOCK_MODE_MP3_NO_AUDIO //CLOCK_MODE_MP3_NO_AUDIO //CLOCK_MODE_MP3_TIMER 

//#define GIF_BUFFERED_FILE
#define GIF_FILE_BUFFER_SIZE 128

#define BTN_LONG_PRESS_TIMING 1000 //1000

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32

#define CLOCK_TIME_FONT DJB_Get_Digital8pt7b
#define CLOCK_TIME_FONT_GLYPHS DJB_Get_Digital8pt7bGlyphs

#define CLOCK_DATE_FONT Picopixel
#define CLOCK_DATE_FONT_GLYPHS PicopixelGlyphs

#define CLOCK_MSG_FONT Picopixel
#define CLOCK_MSG_FONT_GLYPHS PicopixelGlyphs


////////////////////////////
//LIBRARIES SPECIFIC DEFINES
////////////////////////////
#include <time.h>

//////////////////////////////////////
////////// LED PANEL DRIVER //////////
//////////////////////////////////////
#define NEOPIXEL_MATRIX
#include "neomatrix_config.h"

char matrix_to_clear_flag;

#include "DJB_Get_Digital8pt7b.h"
#include "Org_01.h"
#include "Picopixel.h"
#include "Tiny3x3a2pt7b.h"
#include "TomThumb.h"
//////////////////////////////////////
////////// SD CARD ///////////////////
//////////////////////////////////////
#include "FS.h"



#include "SD_MMC.h"
#include "esp_vfs_fat.h"
/////////////////////////////////////
////////// MP3 DFPLAYER MINI /////////////
/////////////////////////////////////
#include <HardwareSerial.h>
#include <DFMiniMp3.h>

//////////////////////////////////////
////////// AUDIO ///////////////
/////////////////////////////////////

#include <Arduino.h>

#define FFTADC_PINL 39
#define FFTADC_ANALOGADC_DB ADC_0db  //ADC_6db
#define FFT_ADC_MULTIPLY_FACTOR 32.0f

int mp3_TotalAvailableMusicFiles, mp3_TotalAvailableABFiles,mp3_TotalAvailableAlarmFiles;
int mp3_currentIndex;

// implement a notification class,
// its member methods will get called
//


// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"




RTC_DS3231 rtc;
char rtc_time_init_ok;
char week_day[7][5]={"DIM.","LUN.","MAR.","MER.","JEU.","VEN.","SAM."};
//////////////////////////////////////
////////// WIFI & Webserver ///////////////
//////////////////////////////////////
#include <WiFi.h>
#include <HTTPServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <HTTPURLEncodedBodyParser.hpp>
// We need to specify some content-type mapping, so the resources get delivered with the
// right content type and are displayed correctly in the browser
char contentTypes[][2][32] = {
  {".html", "text/html"},
  {".css",  "text/css"},
  {".js",   "application/javascript"},
  {".json", "application/json"},
  {".png",  "image/png"},
  {".jpg",  "image/jpg"},
  {"", ""}
};
// The HTTPS Server comes in a separate namespace. For easier use, include it here.
using namespace httpsserver;

void webSendFileData(char *filename,HTTPResponse *response);
HTTPServer *webServer;

char wifi_ssid[64];
char wifi_password[64];
volatile int wifi_available;

////////////////////////////////////////////
////////// NTP & Time management ///////////////
//////////////////////////////////////
#include <ezTime.h>
#define CLOCK_NTP_UPDATE_DELAY_IN_SECONDS 5*60  //5minutes

/////////////////////////////////////
////////// GIF DECODER  /////////////
/////////////////////////////////////
#include "GifDecoder.h"
// If the matrix is a different size than the GIFs, allow panning through the GIF
// while displaying it, or bouncing it around if it's smaller than the display
int OFFSETX = 0;
int OFFSETY = 0;
int FACTX = 0;
int FACTY = 0;


File gif_file;
#ifdef GIF_BUFFERED_FILE
uint8_t gif_file_buffer[GIF_FILE_BUFFER_SIZE];
int gif_file_buffer_pos;
int gif_file_buffer_current_size;
#endif

char gif_in_progress = 0;
int gifs_TotalAvailableFiles;
int gifs_TotalBackgroundAvailableFiles;
int gifs_TotalBootLogoAvailableFiles;

char *gifs_anim_dir_list[GIFS_ANIM_DIR_MAX]; // = {"Arcade", "Bootlogos", "Computers", "Consoles", "Demoscene", "Halloween", "Other"};
int gifs_anim_dir_list_nbgifs[GIFS_ANIM_DIR_MAX];
char gifs_dir_total;

int gif_delay_before_restart;

char current_mp3_title[128];

volatile int clock_draw_audio_message;

char clock_brightness = DEFAULT_BRIGHTNESS;
int clock_r, clock_g, clock_b;
char clock_mode_time;
char clock_mode_mp3;
char clock_mode_mp3_type;
char clock_mode_mp3_pause;
volatile int mp3_isPlaying;
int clock_mode_mp3_files2play;
char mp3_playback_authorized;
unsigned long clock_mode_mp3_time2play;
char clock_current_display, clock_current_display_switch;
int clock_color_mode;
int clock_audio_volume,clock_alarm_volume;

//////////////////////////////////////////
// MP3 ///////////////
//////////////////////////////////////////
// instance a DFMiniMp3 object,
// defined with the above notification class and the hardware serial class

class Mp3Notify
{
  public:
    static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
    {
      if (source & DfMp3_PlaySources_Sd)
      {
        Serial.print("SD Card, ");
      }
      if (source & DfMp3_PlaySources_Usb)
      {
        Serial.print("USB Disk, ");
      }
      if (source & DfMp3_PlaySources_Flash)
      {
        Serial.print("Flash, ");
      }
      Serial.println(action);
    }
    static void OnError(uint16_t errorCode)
    {
      // see DfMp3_Error for code meaning
      Serial.println();
      Serial.print("Com Error ");
      Serial.println(errorCode);

    }
    static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
    {
      Serial.printf("Play finished for %d: %s\n", track, current_mp3_title);
      mp3_isPlaying = 0;
    }
    static void OnPlaySourceOnline(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "online");
    }
    static void OnPlaySourceInserted(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "inserted");
      if (source & DfMp3_PlaySources_Sd) {
        mp3_isPlaying = -1;
      }

    }
    static void OnPlaySourceRemoved(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "removed");
      if (source & DfMp3_PlaySources_Sd) {
        mp3_isPlaying = -2;
      }

    }
};


DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial1);

//////////////////////////////////////////////
/// AUDIO FFT ///////////////////////////////
////////////////////////////////////////////
#define XMAX 64
#define YMAX 20

#include "arduinoFFT.h" // Standard Arduino FFT library https://github.com/kosme/arduinoFFT

#define FFT_VREAL_THRESHOLD 500 //1000 //1000

const uint16_t samples = 512; //This value MUST ALWAYS be a power of 2
const float samplingFrequency = 10000; //Hz, must be less than 10000 due to ADC
unsigned int sampling_period_us;
unsigned long microseconds;

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
float vReal[samples];
float vImag[samples];

/*
Allocate space for FFT window weighing factors, so they are calculated only the first time windowing() is called.
If you don't do this, a lot of calculations are necessary, depending on the window function.
*/
float weighingFactors[samples];

/* Create FFT object with weighing factor storage */
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, samples, samplingFrequency, weighingFactors);

TaskHandle_t task_fft;

//unsigned int sampling_period_us;
//unsigned long microseconds;

float bin_size;
float BIN_MAX = 240000;

#define BAND_MAX 31   //0..BAND_MAX - 1 are the values
int band_min, band_max;
#define BAND_LENGTH 1

int amin = 4096; int amax = 0;

const float LEDlimit[YMAX] = {
  0.041, 0.049, 0.059, 0.070, 0.084, 0.100, 0.119, 0.143, 0.170, 0.203, 0.242, 0.289, 0.346, 0.412, 0.492, 0.588, 0.702, 0.838, 1.000, 1.413
};

// 1/3 ocatave frequency scale   (lower 8 values are missing)
/*const int limit[] = {50,100,125,160,200,250,315,400,500,630,800,
              1000,1250,1600,2000,2500,3150,4000,5000,6300,8000,
              10000,12500,16000,20000};
*/
/*const int limit[] = {50, 120, 200, 300, 400, 600, 800,
                     1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000,
                     10000, 12500, 16000, 20000
                    };
*/
const int limit[] = {20, 25, 31, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800,
                     1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000,
                     10000, 12500, 16000, 20000, 40000
                    };


int bin_band[samples];
volatile float fft_bands[BAND_MAX];
volatile float dc_level;
byte dc_level_band;
byte peak[BAND_MAX];
byte peak_staytopcnt[BAND_MAX];
byte peak2[BAND_MAX];
//float vReal[SAMPLES];
//float vImag[SAMPLES];
unsigned long newTime, oldTime;
int dominant_value;

char sdcard_avail;

void sdcard_init() {
  sdcard_avail = 0;
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Card Mount Failed");
    return;
  }
  Serial.printf("SD Card init ok\n");
  sdcard_avail = 1;
}

//////////////////////////////////////////////
/////////////////////////////////////////////
////////////////////////////////////////////
GifDecoder<kMatrixWidth, kMatrixHeight, 12> decoder;

bool fileSeekCallback(unsigned long position) {
  //Serial.printf("call to seek %d\n",position);
#ifdef GIF_BUFFERED_FILE
  gif_file_buffer_pos = GIF_FILE_BUFFER_SIZE; //BUFFER WILL BE RESETED AT NEXT READ
#endif
  return gif_file.seek(position);
}
unsigned long filePositionCallback(void) {
  //Serial.printf("call to position\n");
#ifdef GIF_BUFFERED_FILE
  if (gif_file_buffer_pos < GIF_FILE_BUFFER_SIZE) return gif_file.position() + gif_file_buffer_pos - gif_file_buffer_current_size;
#endif
  return gif_file.position();
}
int fileReadCallback(void) {
  //Serial.printf("call to read 1\n");
#ifdef GIF_BUFFERED_FILE
  if (gif_file_buffer_pos == GIF_FILE_BUFFER_SIZE) {  //BUFFER RESET

    //NEW BUFFER
    gif_file_buffer_current_size = gif_file.read(gif_file_buffer, GIF_FILE_BUFFER_SIZE); //READ NEW BUFFER
    if (gif_file_buffer_current_size < 0) {
      sdcard_avail = 0;
      ESP.restart();
      return gif_file_buffer_current_size;
    }
    //if (gif_file_buffer_current_size<GIF_FILE_BUFFER_SIZE) Serial.printf("Could only read1: %d/%d, EOF probably reached\n",gif_file_buffer_current_size,GIF_FILE_BUFFER_SIZE);
    gif_file_buffer_pos = 0;
  }
  return (int)gif_file_buffer[gif_file_buffer_pos++];
#endif
  int ret = gif_file.read();
  if (ret < 0) {
    sdcard_avail = 0;
    ESP.restart();
  }
  return ret;
}
int fileReadBlockCallback(void * buffer, int numberOfBytes) {
  //Serial.printf("call to read %d\n",numberOfBytes);
#ifdef GIF_BUFFERED_FILE
  if (gif_file_buffer_pos == GIF_FILE_BUFFER_SIZE) { // BUFFER TO BE RESETED
    if (numberOfBytes < GIF_FILE_BUFFER_SIZE) { // INIT NEW BUFFER

      //NEW BUFFER
      gif_file_buffer_current_size = gif_file.read(gif_file_buffer, GIF_FILE_BUFFER_SIZE); //READ NEW BUFFER
      if (gif_file_buffer_current_size < 0) {
        sdcard_avail = 0;
        ESP.restart();
        return gif_file_buffer_current_size;
      }
      //if (gif_file_buffer_current_size<GIF_FILE_BUFFER_SIZE) Serial.printf("Could only read2: %d/%d, EOF probably reached\n",gif_file_buffer_current_size,GIF_FILE_BUFFER_SIZE);

      if (gif_file_buffer_current_size < GIF_FILE_BUFFER_SIZE) { //EOF REACHED
        if (numberOfBytes < gif_file_buffer_current_size) { //eof reached & less bytes than available are asked
          memcpy((uint8_t*)buffer, gif_file_buffer, numberOfBytes);
          gif_file_buffer_pos = numberOfBytes;
          return numberOfBytes;
        } else {  //eof reached & more bytes than available are asked
          memcpy((uint8_t*)buffer, gif_file_buffer, gif_file_buffer_current_size);
          gif_file_buffer_pos = GIF_FILE_BUFFER_SIZE; //mark buffer as invalid (EOF reached)
          return gif_file_buffer_current_size;
        }
      } else {  //EOF NOT REACHED
        memcpy((uint8_t*)buffer, gif_file_buffer, numberOfBytes);
        gif_file_buffer_pos = numberOfBytes;     //Mark buffer as to be reseted, end was reached
        return numberOfBytes;
      }
    } else {  //Read more than buffer size, do not reset buffer yet
      int ret = gif_file.read((uint8_t*)buffer, numberOfBytes);
      if (ret < 0) {
        sdcard_avail = 0;
        ESP.restart();
      }
      return ret;
    }
  } else { //BUFFER AVAILABLE
    int remaining_to_read = numberOfBytes - (gif_file_buffer_current_size - gif_file_buffer_pos);

    if (remaining_to_read <= 0) { //Enough already in the buffer
      memcpy((uint8_t*)buffer, gif_file_buffer + gif_file_buffer_pos, numberOfBytes);
      gif_file_buffer_pos += numberOfBytes;
      return numberOfBytes;
    } else { //not enough in the buffer
      memcpy((uint8_t*)buffer, gif_file_buffer + gif_file_buffer_pos, gif_file_buffer_current_size - gif_file_buffer_pos);     //copy buffer part
      int has_read = gif_file.read((uint8_t*)buffer + gif_file_buffer_current_size - gif_file_buffer_pos, remaining_to_read); //complete with SD read
      if (has_read < 0) {
        sdcard_avail = 0;
        ESP.restart();
        return has_read;
      }
      //if (has_read<remaining_to_read) Serial.printf("Could only read3: %d/%d, EOF probably reached\n",has_read,remaining_to_read);

      has_read += gif_file_buffer_current_size - gif_file_buffer_pos;
      gif_file_buffer_pos = GIF_FILE_BUFFER_SIZE; //BUFFER to be reseted
      return has_read;
    }
  }
#endif
  int ret = gif_file.read((uint8_t*)buffer, numberOfBytes);
  if (ret < 0) {
    sdcard_avail = 0;
    ESP.restart();
  }
  return ret;
}

void clock_draw_msg() {
  char str_msg[10];
  int x, y, xs, ys;

  //compute width
  int width, height;
  int first_char;

  if (!clock_brightness) return;


  if (clock_draw_audio_message) {
    matrix->setFont(&CLOCK_MSG_FONT);
    matrix->setTextSize(1);
    switch (clock_mode_mp3) {
      case CLOCK_MODE_MP3_PLAY_LIMITED_NB:
        if (clock_mode_mp3_pause) sprintf(str_msg, "\|\|%d", clock_mode_mp3_files2play);
        else sprintf(str_msg, ">%d", clock_mode_mp3_files2play);
        break;
      case CLOCK_MODE_MP3_TIMER:
        if (clock_mode_mp3_pause) sprintf(str_msg, "\|\|%dmin", clock_mode_mp3_time2play / 60);
        else sprintf(str_msg, ">%dmin", clock_mode_mp3_time2play / 60);
        break;
      case CLOCK_MODE_MP3_NO_AUDIO:
        sprintf(str_msg, "STOP", clock_mode_mp3_time2play / 60);
        break;
      default:
        str_msg[0] = 0;
    }
    CRGB color, color0;
    if (clock_draw_audio_message > DRAW_MSG_LENGTH / 2) matrix->setTextColor(YRGB_COLOR(matrix->Color, (clock_r ) , (clock_g ) , (clock_b )));
    else matrix->setTextColor(YRGB_COLOR(matrix->Color, (clock_r * clock_draw_audio_message * 2 / DRAW_MSG_LENGTH) , (clock_g * clock_draw_audio_message * 2 / DRAW_MSG_LENGTH) , (clock_b * clock_draw_audio_message * 2 / DRAW_MSG_LENGTH) ));

    matrix->setCursor(0, 4);
    matrix->print(str_msg);

    int i;
    int ofsx, ofsy;
    ofsx = 32; ofsy = 0;
    color0.red = color0.green = color0.blue = 0;
    if (clock_draw_audio_message > DRAW_MSG_LENGTH / 2) {
      color.red = matrix->gamma[clock_r]; color.blue = matrix->gamma[clock_g]; color.green = matrix->gamma[clock_b];
    } else {
      color.red = matrix->gamma[clock_r] * clock_draw_audio_message * 2 / DRAW_MSG_LENGTH; color.blue = matrix->gamma[clock_g] * clock_draw_audio_message * 2 / DRAW_MSG_LENGTH; color.green = matrix->gamma[clock_b] * clock_draw_audio_message * 2 / DRAW_MSG_LENGTH;
    }
    for (i = 0; i < clock_audio_volume / 2; i++) {
      matrix->drawPixel(ofsx + i * 2, ofsy, color);
      matrix->drawPixel(ofsx + i * 2, ofsy + 1, color);
      matrix->drawPixel(ofsx + i * 2, ofsy + 2, color);
      matrix->drawPixel(ofsx + i * 2, ofsy + 3, color);
      matrix->drawPixel(ofsx + i * 2, ofsy + 4, color);

      matrix->drawPixel(ofsx + i * 2 + 1, ofsy, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 1, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 2, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 3, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 4, color0);
    }
    if (clock_audio_volume & 1) {
      matrix->drawPixel(ofsx + i * 2, ofsy + 1, color);
      matrix->drawPixel(ofsx + i * 2, ofsy + 2, color);
      matrix->drawPixel(ofsx + i * 2, ofsy + 3, color);

      matrix->drawPixel(ofsx + i * 2, ofsy, color0);
      matrix->drawPixel(ofsx + i * 2, ofsy + 4, color0);

      matrix->drawPixel(ofsx + i * 2 + 1, ofsy, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 1, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 2, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 3, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 4, color0);
      i++;
    }
    for (; i < 30 / 2; i++) {
      matrix->drawPixel(ofsx + i * 2, ofsy + 2, color);

      matrix->drawPixel(ofsx + i * 2, ofsy, color0);
      matrix->drawPixel(ofsx + i * 2, ofsy + 1, color0);
      matrix->drawPixel(ofsx + i * 2, ofsy + 3, color0);
      matrix->drawPixel(ofsx + i * 2, ofsy + 4, color0);

      matrix->drawPixel(ofsx + i * 2 + 1, ofsy, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 1, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 2, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 3, color0);
      matrix->drawPixel(ofsx + i * 2 + 1, ofsy + 4, color0);
    }
  } else if ((clock_current_display != CLOCK_DISPLAY_GIFANIM) && (clock_current_display != CLOCK_DISPLAY_BOOTANIM)) {
    char date_str[6];
    //if (rtc_time_init_ok) sprintf(date_str, "%2.1fC", rtc.getTemperature());
    
    if (rtc_time_init_ok) sprintf(date_str, "%s", week_day[rtc.now().dayOfTheWeek()]);
    

    matrix->setFont(&CLOCK_MSG_FONT);
    matrix->setTextSize(1);
    first_char = CLOCK_DATE_FONT.first;
    width = 0;
    height = 0;
    for (int i = 0; i < strlen(date_str); i++) {
      width += CLOCK_DATE_FONT_GLYPHS[date_str[i] - first_char].xAdvance;
      if (height < CLOCK_DATE_FONT_GLYPHS[date_str[i] - first_char].height) height = CLOCK_DATE_FONT_GLYPHS[date_str[i] - first_char].height;
    }

    x = 0;
    y = height - 1;
    matrix->setTextColor(YRGB_COLOR(matrix->Color, clock_r * 4 / 5 , clock_g * 4 / 5 , clock_b * 4 / 5 ));
    matrix->setCursor(x, y);
    matrix->print(date_str);

    matrix->setFont(&CLOCK_DATE_FONT);
    matrix->setTextSize(1);

    if (rtc_time_init_ok) {
      DateTime now = rtc.now();
      sprintf(date_str, "%02d/%02d", now.day(), now.month());
    }
    

    first_char = CLOCK_DATE_FONT.first;
    width = 0;
    height = 0;
    for (int i = 0; i < strlen(date_str); i++) {
      width += CLOCK_DATE_FONT_GLYPHS[date_str[i] - first_char].xAdvance;
      if (height < CLOCK_DATE_FONT_GLYPHS[date_str[i] - first_char].height) height = CLOCK_DATE_FONT_GLYPHS[date_str[i] - first_char].height;
    }

    x = PANEL_WIDTH - width;
    y = height - 1;
    matrix->setTextColor(YRGB_COLOR(matrix->Color, clock_r * 4 / 5, clock_g * 4 / 5, clock_b * 4 / 5));
    matrix->setCursor(x, y);
    matrix->print(date_str);

  }

}

void clock_draw_time(char reduced) {
  if (!clock_brightness) return;

  if (rtc_time_init_ok) {
    DateTime now = rtc.now();



    int hours = now.hour(); //timeinfo.tm_hour;
    int minutes = now.minute(); //timeinfo.tm_min;
    int seconds = now.second() % 60; //timeinfo.tm_sec;    
    int x, y, xs, ys;

    //compute width
    int width, height;
    int first_char;
    const GFXglyph *time_fontglyphs;
    if (reduced) {
      matrix->setFont(&CLOCK_DATE_FONT);
      first_char = CLOCK_DATE_FONT.first;
      time_fontglyphs = CLOCK_DATE_FONT_GLYPHS;
    }
    else {
      matrix->setFont(&CLOCK_TIME_FONT);
      first_char = CLOCK_TIME_FONT.first;
      time_fontglyphs = CLOCK_TIME_FONT_GLYPHS;
    }

    matrix->setTextSize(1);



    width = 0;
    height = 0;
    width += time_fontglyphs[((hours / 10) % 10) + '0' - first_char].xAdvance;
    if (height < time_fontglyphs[((hours / 10) % 10) + '0' - first_char].height) height = time_fontglyphs[((hours / 10) % 10) + '0' - first_char].height;

    width += time_fontglyphs[(hours % 10) + '0' - first_char].xAdvance;
    if (height < time_fontglyphs[(hours % 10) + '0' - first_char].height) height = time_fontglyphs[(hours % 10) + '0' - first_char].height;

    width += time_fontglyphs[':' - first_char].xAdvance;
    if (height < time_fontglyphs[':' - first_char].height) height = time_fontglyphs[':' - first_char].height;

    width += time_fontglyphs[((minutes / 10) % 10) + '0' - first_char].xAdvance;
    if (height < time_fontglyphs[((minutes / 10) % 10) + '0' - first_char].height) height = time_fontglyphs[((minutes / 10) % 10) + '0' - first_char].height;

    width += time_fontglyphs[(minutes % 10) + '0' - first_char].xAdvance;
    if (height < time_fontglyphs[(minutes % 10) + '0' - first_char].height) height = time_fontglyphs[(minutes % 10) + '0' - first_char].height;

    /*width += time_fontglyphs[':' - first_char].xAdvance;
      if (height < time_fontglyphs[':' - first_char].height) height = time_fontglyphs[':' - first_char].height;

      width += time_fontglyphs[((seconds / 10) % 10) + '0' - first_char].xAdvance;
      if (height < time_fontglyphs[((seconds / 10) % 10) + '0' - first_char].height) height = time_fontglyphs[((seconds / 10) % 10) + '0' - first_char].height;

      width += time_fontglyphs[(seconds % 10) + '0' - first_char].xAdvance;
      if (height < time_fontglyphs[(seconds % 10) + '0' - first_char].height) height = time_fontglyphs[(seconds % 10) + '0' - first_char].height;
    */
    x = (PANEL_WIDTH - width) / 2;
    if (x < 0) x = 0;

    if (reduced) y = (PANEL_HEIGHT / 4) + (height / 2);
    else y = (PANEL_HEIGHT / 2) + (height / 2) - 2;
    if (y < 0) y = 0;


    //Shadow
    xs = x + 1;
    ys = y + 1;
    //if (xs+width>PANEL_WIDTH) xs=x;
    //if (ys+height>PANEL_HEIGHT) ys=y;

    if (reduced == 0) {
      matrix->setCursor(xs, ys);
      matrix->setTextColor(YRGB_COLOR(matrix->Color, clock_r * 3 / 4, clock_g * 3 / 4, clock_b * 3 / 4));
      matrix->print(((hours / 10) % 10));
      matrix->print((hours % 10));
      //matrix->setTextColor(YRGB_COLOR(matrix->Color,clock_r * 3 / 4 *3/4, clock_g * 3 / 4 *3/4, clock_b * 3 / 4 *3/4));
      /*if (seconds & 1)*/ matrix->print(':');
      //else matrix->print(' ');
      //matrix->setTextColor(YRGB_COLOR(matrix->Color,clock_r *3/4, clock_g *3/4, clock_b *3/4));
      matrix->print(((minutes / 10) % 10));
      matrix->print((minutes % 10));

      CRGB color;
      color.red = matrix->gamma[clock_r]; color.blue = matrix->gamma[clock_g]; color.green = matrix->gamma[clock_b];
      
      if (seconds<20) {
        matrix->drawPixel(12 + seconds*2, 8, color);  
      } else if (seconds<30) {
        matrix->drawPixel(12 + 40, 8+(seconds-20)*14/10, color);  
      } else if (seconds<50) {
        matrix->drawPixel(12 + 40 - (seconds-30)*2, 8+14, color);  
      } else {
        matrix->drawPixel(12, 8+14-(seconds-50)*14/10, color);  
      }
      /*matrix->setTextColor(YRGB_COLOR(matrix->Color,clock_r * 3 / 4 *3/4, clock_g * 3 / 4 *3/4, clock_b * 3 / 4 *3/4));
        matrix->print(':');
        matrix->setTextColor(YRGB_COLOR(matrix->Color,clock_r *3/4, clock_g *3/4, clock_b *3/4));
        matrix->print(((seconds / 10) % 10));
        matrix->print((seconds % 10));*/
    }
    //Time
    matrix->setCursor(x, y);
    matrix->setTextColor(YRGB_COLOR(matrix->Color, clock_r, clock_g, clock_b));
    matrix->print(((hours / 10) % 10));
    matrix->print((hours % 10));
    //matrix->setTextColor(YRGB_COLOR(matrix->Color,clock_r * 3 / 4, clock_g * 3 / 4, clock_b * 3 / 4));
    /*if (seconds & 1)*/ matrix->print(':');
    //else matrix->print(' ');
    //matrix->setTextColor(YRGB_COLOR(matrix->Color,clock_r, clock_g, clock_b));
    matrix->print(((minutes / 10) % 10));
    matrix->print((minutes % 10));
    /*matrix->setTextColor(YRGB_COLOR(matrix->Color,clock_r * 3 / 4, clock_g * 3 / 4, clock_b * 3 / 4));
      matrix->print(':');
      matrix->setTextColor(YRGB_COLOR(matrix->Color,clock_r, clock_g, clock_b));
      matrix->print(((seconds / 10) % 10));
      matrix->print((seconds % 10));
    */




  } else {
    int x, y, xs, ys;

    //compute width
    int width, height;
    int first_char;

    matrix->setFont(&CLOCK_MSG_FONT);
    matrix->setTextSize(1);


    const char date_str[] = "RESET TIME / WIFI";

    first_char = CLOCK_MSG_FONT.first;
    width = 0;
    height = 0;
    for (int i = 0; i < strlen(date_str); i++) {
      width += CLOCK_MSG_FONT_GLYPHS[date_str[i] - first_char].xAdvance;
      if (height < CLOCK_MSG_FONT_GLYPHS[date_str[i] - first_char].height) height = CLOCK_MSG_FONT_GLYPHS[date_str[i] - first_char].height;
    }

    x = (PANEL_WIDTH - width) / 2;
    if (x < 0) x = 0;
    y = (PANEL_HEIGHT + height) / 2 - 1;
    matrix->setCursor(x + 1, y + 1);
    matrix->setTextColor(YRGB_COLOR(matrix->Color, clock_r / 2, clock_g / 2, clock_b / 2));
    matrix->print(date_str);
    matrix->setTextColor(YRGB_COLOR(matrix->Color, clock_r, clock_g, clock_b));
    matrix->setCursor(x, y);
    matrix->print(date_str);
  }
}

void screenClearCallback(void) {
  if (clock_brightness) matrix->clear();
}
void updateScreenCallback(void) {
  clock_draw_msg();
  if (gif_in_progress == GIF_ANIM_CLOCK) {
    //display hour on top of gif
    clock_draw_time(0);
  }
  if (clock_brightness) {
    if (clock_brightness) matrix->show();
  }
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
  CRGB color = YRGB_COLOR(CRGB, matrix->gamma[red], matrix->gamma[green], matrix->gamma[blue]);
  // This works but does not handle out of bounds pixels well (it writes to the last pixel)
  // matrixleds[XY(x+OFFSETX,y+OFFSETY)] = color;
  // drawPixel ensures we don't write out of bounds
  matrix->drawPixel(x + OFFSETX, y + OFFSETY, color);
}


/////////////////////////////////
///// push button ///////////////
/////////////////////////////////
const char PushButton1 = 34;
const char PushButton2 = 36;
const char PushButton3 = 35;
const char PushButton4 = 3;
const char PushButton5 = 5;

unsigned long btn_pressed_time[CLOCK_BUTTON_NB];
char btn_push_status[CLOCK_BUTTON_NB];
char comboBtn[2];

volatile char btn_Pushed = 0;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// SD CARD //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

hw_timer_t * timer = NULL;
hw_timer_t * timerFPS = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

void IRAM_ATTR onTimer() {
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output

}

void IRAM_ATTR onTimerFPS() {
  if (clock_draw_audio_message) {
    clock_draw_audio_message--;
    if (clock_draw_audio_message == 0) matrix_to_clear_flag = 1;
  }
}


void gif_checkGifsFilesCount() {
  gifs_TotalAvailableFiles = 0;
  char dirname[64];
  gifs_dir_total = 0;

  if (!sdcard_avail) return;
  //1st check existing dirs
  File gifs_dir = SD_MMC.open("/gifs");
  File file = gifs_dir.openNextFile();
  while (file) {
    if ((gifs_dir_total < GIFS_ANIM_DIR_MAX) && (file.isDirectory())) {
      char *dir_shortname = strrchr(file.name(), '/');
      gifs_anim_dir_list[gifs_dir_total] = (char*)malloc(strlen(dir_shortname) + 1 - 1);
      strcpy(gifs_anim_dir_list[gifs_dir_total], dir_shortname + 1);
      Serial.printf("Gifs | found dir: %s\n", gifs_anim_dir_list[gifs_dir_total]);
      clock_gifs_anim_dir_activate[gifs_dir_total]=1; //by default activate dir
      gifs_dir_total++;
    }
    //file.close();
    file = gifs_dir.openNextFile();
  }
  gifs_dir.close();

  for (int i = 0; i < gifs_dir_total; i++) {
    gifs_anim_dir_list_nbgifs[i] = 0;
    sprintf(dirname, "/gifs/%s", gifs_anim_dir_list[i]);
    File gifs_dir = SD_MMC.open(dirname);
    if (!gifs_dir) {
      Serial.printf("open %s failed\n", dirname);

    } else {
      char *filename;
  
      while (filename = gifs_dir.getNextFileName()) {
        //yield();
        //vTaskDelay(1);

        char filename_len = strlen(filename);
        if (filename_len > 4) {
          if ((filename[filename_len - 4] == '.') && ((filename[filename_len - 3] == 'g') || (filename[filename_len - 3] == 'G')) && ((filename[filename_len - 2] == 'i') || (filename[filename_len - 2] == 'I')) && ((filename[filename_len - 1] == 'f') || (filename[filename_len - 1] == 'F'))) {
            gifs_TotalAvailableFiles++;
            gifs_anim_dir_list_nbgifs[i]++;
          }
        }
        free(filename);
      }
      //Serial.printf("Gifs files sub total: %d files in %s\n",gifs_TotalAvailableFiles,dirname);

      gifs_dir.close();
    }
  }
  Serial.printf("Gifs files total: %d files\n", gifs_TotalAvailableFiles);
}

void gif_checkBackgroundGifsFilesCount() {
  gifs_TotalBackgroundAvailableFiles = 0;
  if (!sdcard_avail) return;

  File gifs_dir = SD_MMC.open("/gifs/Backgrounds");
  if (!gifs_dir) {
    Serial.printf("open /Backgrounds failed\n");

  } else {
    char *filename;

    while (filename = gifs_dir.getNextFileName()) {
      char filename_len = strlen(filename);
      if (filename_len > 4) {
        if ((filename[filename_len - 4] == '.') && ((filename[filename_len - 3] == 'g') || (filename[filename_len - 3] == 'G')) && ((filename[filename_len - 2] == 'i') || (filename[filename_len - 2] == 'I')) && ((filename[filename_len - 1] == 'f') || (filename[filename_len - 1] == 'F'))) {
          gifs_TotalBackgroundAvailableFiles++;
        }
      }
      free(filename);
    }
    gifs_dir.close();
  }
  Serial.printf("Background gifs files total: %d files\n", gifs_TotalBackgroundAvailableFiles);
}

void gif_checkBootLogoGifsFilesCount() {
  gifs_TotalBootLogoAvailableFiles = 0;
  if (!sdcard_avail) return;
  File gifs_dir = SD_MMC.open("/Bootlogos");
  if (!gifs_dir) {
    Serial.printf("open /Bootlogos failed\n");

  } else {
    char *filename;

    while (filename = gifs_dir.getNextFileName()) {
      char filename_len = strlen(filename);
      if (filename_len > 4) {
        if ((filename[filename_len - 4] == '.') && ((filename[filename_len - 3] == 'g') || (filename[filename_len - 3] == 'G')) && ((filename[filename_len - 2] == 'i') || (filename[filename_len - 2] == 'I')) && ((filename[filename_len - 1] == 'f') || (filename[filename_len - 1] == 'F'))) {
          gifs_TotalBootLogoAvailableFiles++;
        }
      }
      free(filename);
    }
    gifs_dir.close();
  }
  Serial.printf("Bootlogos gifs files total: %d files\n", gifs_TotalBootLogoAvailableFiles);
}


bool gif_startNewAnimByName(char *filename) {
  if (!sdcard_avail) return false;
  gif_file = SD_MMC.open(filename);

  if (gif_file) {
    //read buffer
#ifdef GIF_BUFFERED_FILE
    gif_file_buffer_pos = GIF_FILE_BUFFER_SIZE;
#endif
    return true;
  }
  else return false;
}

bool gif_startNewBootLogo(int index) {
  int gifsFileCount = 0;

  //Serial.printf("Total RAM: %d / available: %d / max allocatable: %d\n", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());

  if (!sdcard_avail) return false;

  if (gifs_TotalBootLogoAvailableFiles == 0) return false;
  if (index < 0) index = random(gifs_TotalBootLogoAvailableFiles);
  if (index >= gifs_TotalBootLogoAvailableFiles) index = index % gifs_TotalBootLogoAvailableFiles;
  File gifs_dir = SD_MMC.open("/Bootlogos");
  if (!gifs_dir) {
    Serial.println("open /Bootlogos failed");
    sdcard_avail = 0;
    return false;
  }
  char *filename;

  while (filename = gifs_dir.getNextFileName()) {
    yield();
    vTaskDelay(1);

    //file_browser.getSFN(fname);
    char filename_len = strlen(filename);
    if (filename_len > 4) {
      if ((filename[filename_len - 4] == '.') && ((filename[filename_len - 3] == 'g') || (filename[filename_len - 3] == 'G')) && ((filename[filename_len - 2] == 'i') || (filename[filename_len - 2] == 'I')) && ((filename[filename_len - 1] == 'f') || (filename[filename_len - 1] == 'F'))) {
        if (gifsFileCount == index) {
          //Serial.printf("Start GIF #%d - %s\n", index, filename);
          gif_file = SD_MMC.open(filename);
          free(filename);
          break;
        }
        else gifsFileCount++;
        free(filename);
      }
    }
  }

  gifs_dir.close();

  if (gif_file) {
    //read buffer
#ifdef GIF_BUFFERED_FILE
    gif_file_buffer_pos = GIF_FILE_BUFFER_SIZE;
#endif
    return true;
  } else return false;
}


bool gif_startNewBGTimeAnim(int index) {
  int gifsFileCount = 0;

  //Serial.printf("Total RAM: %d / available: %d / max allocatable: %d\n", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());

  if (gifs_TotalBackgroundAvailableFiles == 0) return false;
  if (index < 0) index = random(gifs_TotalBackgroundAvailableFiles);
  if (index >= gifs_TotalBackgroundAvailableFiles) index = index % gifs_TotalBackgroundAvailableFiles;
  File gifs_dir = SD_MMC.open("/gifs/Backgrounds");
  if (!gifs_dir) {
    Serial.println("open /Backgrounds failed");
    sdcard_avail = 0;
    return false;
  }
  char *filename;

  while (filename = gifs_dir.getNextFileName()) {
    yield();
    vTaskDelay(1);

    //file_browser.getSFN(fname);
    char filename_len = strlen(filename);
    if (filename_len > 4) {
      if ((filename[filename_len - 4] == '.') && ((filename[filename_len - 3] == 'g') || (filename[filename_len - 3] == 'G')) && ((filename[filename_len - 2] == 'i') || (filename[filename_len - 2] == 'I')) && ((filename[filename_len - 1] == 'f') || (filename[filename_len - 1] == 'F'))) {
        if (gifsFileCount == index) {
          //Serial.printf("Start GIF #%d - %s\n", index, filename);
          gif_file = SD_MMC.open(filename);
          free(filename);
          break;
        }
        else gifsFileCount++;
        free((void*)filename);
      }
    }
  }

  gifs_dir.close();

  if (gif_file) {
    //read buffer
#ifdef GIF_BUFFERED_FILE
    gif_file_buffer_pos = GIF_FILE_BUFFER_SIZE;
#endif
    return true;
  } else return false;
}

bool gif_startNewAnim(int index) {
  int gifsFileCount = 0;
  int gifs_dir_index = 0;
  int totalActiveDirs;
  if (gifs_TotalAvailableFiles == 0) return false;

  //check if sd card is available
  if (!sdcard_avail) return false;

  //check if at least 1 dir is active
  totalActiveDirs=0;
  for (int i=0;i<gifs_dir_total;i++) {
    if (clock_gifs_anim_dir_activate[i]) totalActiveDirs++;
  }
  if (!totalActiveDirs) return false;

  if (index < 0) {    
    gifs_dir_index=0;
    //move to 1st dir available
    while (clock_gifs_anim_dir_activate[gifs_dir_index]==0) {
      gifs_dir_index++;
      if (gifs_dir_index>=gifs_dir_total) {          
          Serial.println("Issue in random");
          return false;
        }
    }
    //randomize / available dir
    for (int i=0;i<random(totalActiveDirs);i++) {
      gifs_dir_index++;
      while (clock_gifs_anim_dir_activate[gifs_dir_index]==0) {
        gifs_dir_index++;
        if (gifs_dir_index>=gifs_dir_total) {          
          Serial.println("Issue in random");
          return false;
        }
      }
    }
    index=random(gifs_anim_dir_list_nbgifs[gifs_dir_index]);
    gifsFileCount=0;
  } else {
    if (index >= gifs_TotalAvailableFiles) index = index % gifs_TotalAvailableFiles;

    for (int i = 0; i < gifs_dir_total; i++) {
      if (index < gifsFileCount + gifs_anim_dir_list_nbgifs[i]) {
        gifs_dir_index = i;
        break;
      } else gifsFileCount += gifs_anim_dir_list_nbgifs[i];
    }
  }

  char dirname[64];
  sprintf(dirname, "/gifs/%s", gifs_anim_dir_list[gifs_dir_index]);
  File gifs_dir = SD_MMC.open(dirname);
  if (!gifs_dir) {
    Serial.printf("open %s failed", dirname);
    sdcard_avail = 0;
    return false;
  }
  char *filename;

  while (filename = gifs_dir.getNextFileName()) {
    yield();
    vTaskDelay(1);

    //file_browser.getSFN(fname);
    char filename_len = strlen(filename);
    if (filename_len > 4) {
      if ((filename[filename_len - 4] == '.') && ((filename[filename_len - 3] == 'g') || (filename[filename_len - 3] == 'G')) && ((filename[filename_len - 2] == 'i') || (filename[filename_len - 2] == 'I')) && ((filename[filename_len - 1] == 'f') || (filename[filename_len - 1] == 'F'))) {
        //strcpy(gifs_filenames[gifs_TotalAvailableFiles],filename);
        if (gifsFileCount == index) {
          //Serial.printf("Start GIF #%d - %s\n", index, filename);
          gif_file = SD_MMC.open(filename);
          free(filename);
          break;
        }
        else gifsFileCount++;
        free(filename);
      }
    }
  }

  gifs_dir.close();

  if (gif_file) {
    //read buffer
#ifdef GIF_BUFFERED_FILE
    gif_file_buffer_pos = GIF_FILE_BUFFER_SIZE;
#endif
    return true;
  } else return false;
}


void mp3_getMusicTitle(char type, int index, char *strbuffer, char max_length) {
  File f;
  if (!strbuffer) return;
  if (!sdcard_avail) return;
  if (type == MP3_MUSIC) {
    f = SD_MMC.open("/indexMusic.lst");
  } else if (type == MP3_AUDIOBOOK) {
    f = SD_MMC.open("/indexAB.lst");
  } else  if (type == MP3_ALARM) {
    f = SD_MMC.open("/indexAlarm.lst");
  } else return;
  if (f) {
    int i = 0;
    String buffer;

    while (f.available()) {
      buffer = f.readStringUntil('\n');
      if (i == index) {
        int char_nb = min(buffer.length() - 4, max_length);
        strncpy(strbuffer, buffer.c_str(), char_nb);
        strbuffer[char_nb] = 0;
        break;
      }
      i++;
    }
    f.close();
  }
}


void mp3_startNewMP3(int index, char type) {

  if (type == MP3_MUSIC) {
#ifdef SILENCE_MP3
    index = 967 - 1; //silence
#endif

    if (mp3_TotalAvailableMusicFiles == 0) return;
    if (index < 0) index = random(mp3_TotalAvailableMusicFiles);

    mp3.setVolume(clock_audio_volume);
    mp3.playFolderTrack16(MP3_MUSIC_FOLDER, index + 1);
    mp3_currentIndex = index;
    mp3_isPlaying = 1;

    current_mp3_title[0] = 0;
    mp3_getMusicTitle(MP3_MUSIC, index, current_mp3_title, 64);

    if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
    //Serial.printf("Start MP3 MUSIC %d: %s\n", index, current_mp3_title);
  } else if (type == MP3_AUDIOBOOK) {
    if (mp3_TotalAvailableABFiles == 0) return;
    if (index < 0) index = random(mp3_TotalAvailableABFiles);

    mp3.setVolume(clock_audio_volume);
    mp3.playFolderTrack16(MP3_AUDIOBOOK_FOLDER, index + 1);
    mp3_currentIndex = index;
    mp3_isPlaying = 1;

    current_mp3_title[0] = 0;
    mp3_getMusicTitle(MP3_AUDIOBOOK, index, current_mp3_title, 64);

    if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
    //Serial.printf("Start MP3 AB %d: %s\n", index, current_mp3_title);
  }  else if (type == MP3_ALARM) {
    if (mp3_TotalAvailableAlarmFiles == 0) return;
    if (index < 0) index = random(mp3_TotalAvailableAlarmFiles);

    mp3.setVolume(clock_alarm_volume);
    mp3.playFolderTrack16(MP3_ALARM_FOLDER, index + 1);
    mp3_currentIndex = index;
    mp3_isPlaying = 1;

    current_mp3_title[0] = 0;
    mp3_getMusicTitle(MP3_ALARM, index, current_mp3_title, 64);

    if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
    //Serial.printf("Start MP3 Alarm %d: %s\n", index, current_mp3_title);
  }
}

void checkBrightness() {
  if (rtc_time_init_ok) {
      DateTime now = rtc.now();
      int hour=now.hour();      
      clock_brightness=clock_brightness_hours[hour];
  } else clock_brightness=16;
}

void applyConfChanges() {
  //check brightness
  checkBrightness();
  //update color
  update_clock_color();
  gif_delay_before_restart = clk_global_display_time_duration;
  matrixLayer.setBrightness(clock_brightness);
  config_savetosd();
}

Dictionary *parsePOSTdata(HTTPRequest * req) {
  HTTPBodyParser *parser;
  std::string contentType = req->getHeader("Content-Type");
  size_t semicolonPos = contentType.find(";");
  if (semicolonPos != std::string::npos) {
    contentType = contentType.substr(0, semicolonPos);
  }

  if (contentType == "application/x-www-form-urlencoded") {
    parser = new HTTPURLEncodedBodyParser(req);
    if (parser == NULL) {
      Serial.println("Could not allocate parser");
    }
  } else {
    Serial.printf("Unknown POST Content-Type: %s\n", contentType.c_str());
    return NULL;
  }

  Dictionary *d = new Dictionary();
  while(parser->nextField()) {
    byte buf[512+1];
    std::string name = parser->getFieldName();
    //std::string filename = parser->getFieldFilename();
    //std::string mimeType = parser->getFieldMimeType();
    buf[0]=0;
    while (!parser->endOfField()) {      
      size_t readLength = parser->read(buf, 512);
      buf[readLength]=0;
    }
    
    d->insert(name.c_str(),(char*)buf);    
  }
  delete parser;
  return d;
}

void handleUpdateConfig(HTTPRequest * req, HTTPResponse * res) {
  char str_tmp[64];
  int int_val,int_val2;
  String stringTmp;
  
  Dictionary *d=parsePOSTdata(req);
  res->setHeader("Content-Type","text/html");
  if (d) {    
    for (int i=0;i<24;i++) {
      //Brightness
      sprintf(str_tmp,"BrightT-%d",i+1);
      int_val=d->search(str_tmp).toInt();
      if (int_val<0) int_val=0;
      if (int_val>255) int_val=255;
      clock_brightness_hours[i]=int_val;

      //GIF schedule
      sprintf(str_tmp,"cbgif-%d",i+1);
      if (d->search(str_tmp).compareTo("on")==0) int_val=1;
      else int_val=0;      
      clock_gif_animation_on_hours[i]=int_val;

      //MP3 schedule
      sprintf(str_tmp,"cbMP3-%d",i+1);
      if (d->search(str_tmp).compareTo("on")==0) int_val=1;
      else int_val=0;
      clock_mp3_authorized_hours[i]=int_val;
    }
    for (int i=0;i<7;i++) {
      //Alarm activated
      sprintf(str_tmp,"cbal-%d",i+1);
      if (d->search(str_tmp).compareTo("on")==0) int_val=1;
      else int_val=0;
      clock_alarm_weekday_on[i]=int_val;

      //Alarm mp3 file
      sprintf(str_tmp,"mp3al-%d",i+1);
      int_val=d->search(str_tmp).toInt();
      if (int_val<0) int_val=0;
      if (int_val>mp3_TotalAvailableAlarmFiles) int_val=mp3_TotalAvailableAlarmFiles;
      clock_alarm_weekday_mp3_index[i]=int_val-1;

      //Alarm time
      sprintf(str_tmp,"al-%d",i+1);
      String stringTmp=d->search(str_tmp);
      sscanf(stringTmp.c_str(),"%d:%d",&int_val,&int_val2);
      if (int_val<0) int_val=0;
      if (int_val>23) int_val=23;
      if (int_val2<0) int_val2=0;
      if (int_val2>59) int_val2=59;
      clock_alarm_weekday_hour[i]=int_val;
      clock_alarm_weekday_minute[i]=int_val2;      
    }

    //Alarm volume
    int_val=d->search("alarmvol").toInt();
    if (int_val<0) int_val=0;
    if (int_val>31) int_val=31;
    clock_alarm_volume=int_val;

    for (int i=0;i<gifs_dir_total;i++) {
      //Active GIFs directories
      sprintf(str_tmp,"cbgd-%d",i+1);
      if (d->search(str_tmp).compareTo("on")==0) int_val=1;
      else int_val=0;
      clock_gifs_anim_dir_activate[i]=int_val;       
    }

    //clock_color_mode
    int_val=d->search("clk_color").toInt();
    if (int_val<0) int_val=0;
    if (int_val>5) int_val=5;
    clock_color_mode=int_val;

    //clk_global_display_time_duration
    int_val=d->search("clk_display_duration").toInt();
    if (int_val<0) int_val=0;    
    clk_global_display_time_duration=int_val;

    //clk_global_mp3_timermode_duration
    int_val=d->search("clk_mp3_timer").toInt();
    if (int_val<0) int_val=0;    
    clk_global_mp3_timermode_duration=int_val;

    //clk_global_clock_mode
    int_val=d->search("clockmode").toInt();
    if (int_val<0) int_val=0;
    if (int_val>3) int_val=3;
    clk_global_clock_mode=int_val;
        
    res->setHeader("Content-Type", "text/html");
    //take into account changes
    applyConfChanges();
      
    res->println("OK");
  } else {    
    res->println("Error");
  }
  delete d;  
  //Serial.printf("Ended\nTotal RAM: %d / available: %d / max allocatable: %d\n", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}

void handleTestAlarm(HTTPRequest * req, HTTPResponse * res) {
  Serial.printf("URL: handleTest\nTotal RAM: %d / available: %d / max allocatable: %d\n", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());
  Dictionary *d=parsePOSTdata(req);
  Serial.printf("Dictionnary initialized\nTotal RAM: %d / available: %d / max allocatable: %d\n", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());
  res->setHeader("Content-Type","text/html");
  if (d) {
    int int_val=d->search("alarmIndex").toInt();
    if ((int_val>=0)&&(int_val<mp3_TotalAvailableAlarmFiles)){
      clock_alarm_mp3_index=int_val-1;
      clock_alarm_status=ALARM_SHOULD_RING;
    }       
    res->println("OK");
  } else {    
    res->println("Error");
  }
  delete d;  
  Serial.printf("Dictionnary deleted\nTotal RAM: %d / available: %d / max allocatable: %d\n", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}

void handle404(HTTPRequest * req, HTTPResponse * res);

void handleSPIFFS(HTTPRequest * req, HTTPResponse * res) {
  //Serial.println("SPIFFS access required");
  // We only handle GET here
  if (req->getMethod() == "GET") {
    // Redirect / to /index.html
    std::string reqFile = req->getRequestString()=="/" ? "/menu.html" : req->getRequestString();
    std::string filename;
    // Try to open the file
    if (reqFile.at(0)!='/') filename = std::string("/") + reqFile;
    else filename = reqFile;
    //Serial.printf("accessing: %s\n",filename.c_str());

    int file_exist=0; //1: exist, 2:exist & compressed with gzip
    if (SPIFFS.exists(filename.c_str())) file_exist=1;
    if (SPIFFS.exists((filename+".gz").c_str())) file_exist=2;
    // Check if the file exists
    if (!file_exist) {
      // Send "404 Not Found" as response, as the file doesn't seem to exist
      handle404(req,res);      
      return;
    }

    File file;
    if (file_exist==1) file = SPIFFS.open(filename.c_str());
    else {
      file = SPIFFS.open((filename+".gz").c_str());      
      res->setHeader("Content-Encoding","gzip");
    }

    // Set length
    res->setHeader("Content-Length", httpsserver::intToString(file.size()));

    // Content-Type is guessed using the definition of the contentTypes-table defined above
    int cTypeIdx = 0;
    do {
      if(reqFile.rfind(contentTypes[cTypeIdx][0])!=std::string::npos) {
        res->setHeader("Content-Type", contentTypes[cTypeIdx][1]);
        break;
      }
      cTypeIdx+=1;
    } while(strlen(contentTypes[cTypeIdx][0])>0);

    // Read the file and write it to the response
    uint8_t buffer[256];
    size_t length = 0;
    do {
      length = file.read(buffer, 256);
      res->write(buffer, length);
    } while (length > 0);

    file.close();
  } else {
    // If there's any body, discard it
    req->discardRequestBody();
    // Send "405 Method not allowed" as response
    res->setStatusCode(405);
    res->setStatusText("Method not allowed");
    res->println("405 Method not allowed");
  }
}


void handle404(HTTPRequest * req, HTTPResponse * res) {
  // Discard request body, if we received any
  // We do this, as this is the default node and may also server POST/PUT requests
  req->discardRequestBody();
  // Set the response status
  res->setStatusCode(404);
  res->setHeader("Content-Type", "text/html");
  webSendFileData("/error404.html",res);

/*
  res->setStatusText("Not Found");

  // Set content type of the response
  res->setHeader("Content-Type", "text/html");

  // Write a tiny HTML page
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Not Found</title></head>");
  res->println("<body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body>");
  res->println("</html>");
  */
}

void handleStartMP3(HTTPRequest * req, HTTPResponse * res) {
  
}

void handleStartAB(HTTPRequest * req, HTTPResponse * res) {
  
}

void handleStartGIF(HTTPRequest * req, HTTPResponse * res) {
  
}

void webSendFileData(char *filename,HTTPResponse *response) {
  File file = SPIFFS.open(filename); 
  if(!file){ 
    Serial.printf("Failed to open %s for reading\n",filename); 
    return; 
  } 
  int fsize=file.size();
  if (fsize<=0) {
    file.close();
    Serial.printf("Wrong file size for %s\n",filename);
    return;
  } 
  int str_len=fsize;
  while (str_len) {
    if (str_len>BUFF_HTML_SIZE) {      
      file.readBytes(small_buff,BUFF_HTML_SIZE);
      response->write((byte*)small_buff,BUFF_HTML_SIZE);   
      str_len-=BUFF_HTML_SIZE;
    } else {
      file.readBytes(small_buff,str_len);      
      response->write((byte*)small_buff,str_len);
      str_len=0;
    }
  }
  file.close();
}
void handleConfig(HTTPRequest * req, HTTPResponse * res) {  
  // Status code is 200 OK by default.
  // We want to deliver a simple HTML page, so we send a corresponding content type:
  res->setHeader("Content-Type", "text/html");

  webSendFileData("/config.html",res);
  /////////////////////
  // GIF Directories
  /////////////////////
  for (int i=0;i<gifs_dir_total;i++) {
    res->printf("$('#gifdirs_schedule_list').append('<div><input type=\"checkbox\" name=\"cbgd-%d\" id=\"cbgd-%d\"><label for=\"cbgd-%d\" ></label><text class=\"txtGifDirLbl\">%s</text></div>');\n",i+1,i+1,i+1,gifs_anim_dir_list[i]);    
  }

  /////////////////////
  //ALARM FILES
  /////////////////////
  for (int j=0;j<mp3_TotalAvailableAlarmFiles;j++) {
    mp3_getMusicTitle(MP3_ALARM, j, small_buff, 64);
    for (int i=0;i<7;i++) {      
      res->printf("$('#mp3al-%d').append('<option value=\"%d\">%s</option>');\n",i+1,j+1,small_buff);          
    }
  }

  /////////////////////////////
  //DAILY GIF & MP3 Schedules
  ////////////////////////////
  for (int i=0;i<24;i++) {
    res->printf("document.getElementById('cbgif-%d').checked=%d;\n",i+1,clock_gif_animation_on_hours[i]);
    res->printf("document.getElementById('cbMP3-%d').checked=%d;\n",i+1,clock_mp3_authorized_hours[i]);
    res->printf("document.getElementById('BrightT-%d').value=%d;\n",i+1,clock_brightness_hours[i]);
  }

  /////////////////////////////
  //ALARMS details
  ////////////////////////////  
  for (int i=0;i<7;i++) {
    res->printf("document.getElementById('mp3al-%d').selectedIndex=%d;\n",i+1,clock_alarm_weekday_mp3_index[i]+1);
    res->printf("document.getElementById('al-%d').value='%02d:%02d';\n",i+1,clock_alarm_weekday_hour[i],clock_alarm_weekday_minute[i]);
    res->printf("document.getElementById('cbal-%d').checked=%d;\n",i+1,clock_alarm_weekday_on[i]);
  }

  for (int i=0;i<gifs_dir_total;i++) {    
    res->printf("document.getElementById('cbgd-%d').checked=%d;\n",i+1,clock_gifs_anim_dir_activate[i]);
  }

  res->printf("document.getElementById('alarmvol').value=%d\n",clock_alarm_volume);
  res->printf("document.getElementById('clk_color').selectedIndex=%d\n",clock_color_mode);
  res->printf("document.getElementById('clk_display_duration').value=%d\n",clk_global_display_time_duration);
  res->printf("document.getElementById('clk_mp3_timer').value=%d\n",clk_global_mp3_timermode_duration);
  res->printf("document.getElementById('clockmode').selectedIndex=%d\n",clk_global_clock_mode);
   
  res->printf("</script></body></html>");
}

void handleRoot(HTTPRequest * req, HTTPResponse * res) {  
  // Status code is 200 OK by default.
  // We want to deliver a simple HTML page, so we send a corresponding content type:
  res->setHeader("Content-Type", "text/html");
  webSendFileData("/menu.html",res);
}


static void configureWiFi()
{
    WiFi.persistent(false);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(clk_global_hostname);
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.print("WiFi connected. IP: ");
        Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
        wifi_available=1;
    },
        WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.print("WiFi lost connection. Reason: ");
        Serial.println(info.disconnected.reason);
        WiFi.persistent(false);
        WiFi.disconnect(true);
        //wifi_available=0;
        ESP.restart();
    },
        WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
}

void config_loadfromsd() {
  char str_tmp[512+1];
  int read_byte,index,int_val;
  File configFile = SD_MMC.open("/config.ini");  
  if (!configFile) {
    Serial.println("Error: cannot read ini file");
    return;
  }
  index=0;
  while (1) {
    read_byte=configFile.read();
    if (read_byte<0) break;
    if (read_byte!='\n') {
      str_tmp[index]=read_byte;
      index++;
    }
    else {
      str_tmp[index]=0;
      if (strncmp(str_tmp,"GIF schedule:",strlen("GIF schedule:"))==0) {
        Serial.println("Got GIF schedule");
        index=strlen("GIF schedule:");
        for (int i=0;i<24;i++) {
          int_val=str_tmp[index]-'0';
          index+=2;
          if (int_val) clock_gif_animation_on_hours[i]=1;
          else clock_gif_animation_on_hours[i]=0;
        }
        index=0;
      } else if (strncmp(str_tmp,"MP3 schedule:",strlen("MP3 schedule:"))==0) {
        Serial.println("Got MP3 schedule");
        index=strlen("MP3 schedule:");
        for (int i=0;i<24;i++) {
          int_val=str_tmp[index]-'0';
          index+=2;
          if (int_val) clock_mp3_authorized_hours[i]=1;
          else clock_mp3_authorized_hours[i]=0;
        }
        index=0;
      } else if (strncmp(str_tmp,"Brightness schedule:",strlen("Brightness schedule:"))==0) {
        Serial.println("Got Brightness schedule");
        index=strlen("Brightness schedule:");
        for (int i=0;i<24;i++) {
          int_val=(str_tmp[index]-'0')*100+(str_tmp[index+1]-'0')*10+str_tmp[index+2]-'0';
          index+=4;
          if (int_val>255) int_val=255;
          clock_brightness_hours[i]=int_val;          
        }
        index=0;
      } else if (strncmp(str_tmp,"Alarms:",strlen("Alarms:"))==0) {
        Serial.println("Got Alarms");
        index=strlen("Alarms:");
        for (int i=0;i<7;i++) {
          int_val=str_tmp[index]-'0';
          index+=2;
          if (int_val) clock_alarm_weekday_on[i]=1;
          else clock_alarm_weekday_on[i]=0;

          int_val=(str_tmp[index]-'0')*10+str_tmp[index+1]-'0';
          index+=3;          
          clock_alarm_weekday_mp3_index[i]=int_val-1;

          int_val=(str_tmp[index]-'0')*10+str_tmp[index+1]-'0';
          index+=3;
          if (int_val>23) int_val=23;
          clock_alarm_weekday_hour[i]=int_val;

          int_val=(str_tmp[index]-'0')*10+str_tmp[index+1]-'0';
          index+=3;
          if (int_val>59) int_val=59;
          clock_alarm_weekday_minute[i]=int_val;
        }
        index=0;
      } else if (strncmp(str_tmp,"GIF directories:",strlen("GIF directories:"))==0) {
        Serial.println("Got GIF directories");
        index=strlen("GIF directories:");
        for (int i=0;i<GIFS_ANIM_DIR_MAX;i++) {
          int_val=str_tmp[index]-'0';
          index+=2;
          if (int_val) clock_gifs_anim_dir_activate[i]=1;
          else clock_gifs_anim_dir_activate[i]=0;
        }
        index=0;
      } else if (strncmp(str_tmp,"WIFI SSID:",strlen("WIFI SSID:"))==0) {
        Serial.println("Got WIFI SSID");
        index=strlen("WIFI SSID:");
        strcpy(wifi_ssid,&str_tmp[index]);        
        index=0;
      } else if (strncmp(str_tmp,"WIFI PSK:",strlen("WIFI PSK:"))==0) {
        Serial.println("Got WIFI PSK");
        index=strlen("WIFI PSK:");
        strcpy(wifi_password,&str_tmp[index]);        
        index=0;
      } else if (strncmp(str_tmp,"Hostname:",strlen("Hostname:"))==0) {
        Serial.println("Got Hostname");
        index=strlen("Hostname:");
        strcpy(clk_global_hostname,&str_tmp[index]);        
        index=0;
      } else if (strncmp(str_tmp,"Display time:",strlen("Display time:"))==0) {
        Serial.println("Got Display time");
        index=strlen("Display time:");
        sscanf(&str_tmp[index],"%d",&clk_global_display_time_duration);        
        index=0;
      } else if (strncmp(str_tmp,"MP3 timer duration:",strlen("MP3 timer duration:"))==0) {
        Serial.println("Got MP3 timer duration");
        index=strlen("MP3 timer duration:");
        sscanf(&str_tmp[index],"%d",&clk_global_mp3_timermode_duration);        
        index=0;
      } else if (strncmp(str_tmp,"Clock color:",strlen("Clock color:"))==0) {
        Serial.println("Got Clock color");
        index=strlen("Clock color:");
        sscanf(&str_tmp[index],"%d",&clock_color_mode);        
        index=0;
      } else if (strncmp(str_tmp,"MP3 volume:",strlen("MP3 volume:"))==0) {
        Serial.println("Got MP3 volume");
        index=strlen("MP3 volume:");
        sscanf(&str_tmp[index],"%d",&clock_audio_volume);        
        index=0;
      } else if (strncmp(str_tmp,"Alarm volume:",strlen("Alarm volume:"))==0) {
        Serial.println("Got Alarm volume");
        index=strlen("Alarm volume:");
        sscanf(&str_tmp[index],"%d",&clock_alarm_volume);        
        index=0;
      } else if (strncmp(str_tmp,"Clock mode:",strlen("Clock mode:"))==0) {
        Serial.println("Got Clock mode");
        index=strlen("Clock mode:");
        sscanf(&str_tmp[index],"%d",&clk_global_clock_mode);        
        index=0;
      } else {       
        Serial.println("Unknown entry");
      }
    }    
  }
  configFile.close();
  //gif_file_buffer_current_size = gif_file.read(gif_file_buffer, GIF_FILE_BUFFER_SIZE); //READ NEW BUFFER
}

void config_savetosd() {
  char str_tmp[512],*str_ptr;
  File configFile = SD_MMC.open("/config.ini",FILE_WRITE);  
  if (!configFile) {
    Serial.println("Error: cannot write ini file");
    return;
  }

  sprintf(str_tmp,"WIFI SSID:%s\n",wifi_ssid);
  configFile.write((byte*)str_tmp, strlen(str_tmp));
  
  sprintf(str_tmp,"WIFI PSK:%s\n",wifi_password);
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"Hostname:%s\n",clk_global_hostname);
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"Display time:%d\n",clk_global_display_time_duration);
  configFile.write((byte*)str_tmp, strlen(str_tmp));
  
  sprintf(str_tmp,"MP3 timer duration:%d\n",clk_global_mp3_timermode_duration);
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"Clock color:%d\n",clock_color_mode);
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"MP3 volume:%d\n",clock_audio_volume);
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"Alarm volume:%d\n",clock_alarm_volume);
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"Clock mode:%d\n",clk_global_clock_mode);
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"GIF schedule:");
  str_ptr=str_tmp+strlen(str_tmp);
  for (int i=0;i<24;i++) {
    if (clock_gif_animation_on_hours[i]) *str_ptr++='1';
    else *str_ptr++='0';
    *str_ptr++=',';
  }
  str_ptr--;
  *str_ptr++='\n';
  *str_ptr=0;  
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"MP3 schedule:");
  str_ptr=str_tmp+strlen(str_tmp);
  for (int i=0;i<24;i++) {
    if (clock_mp3_authorized_hours[i]) *str_ptr++='1';
    else *str_ptr++='0';
    *str_ptr++=',';
  }
  str_ptr--;
  *str_ptr++='\n';
  *str_ptr=0;  
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"Brightness schedule:");
  str_ptr=str_tmp+strlen(str_tmp);
  for (int i=0;i<24;i++) {
    *str_ptr++='0'+(clock_brightness_hours[i]/100)%10;
    *str_ptr++='0'+(clock_brightness_hours[i]/10)%10;
    *str_ptr++='0'+(clock_brightness_hours[i]/1)%10;    
    *str_ptr++=',';
  }
  str_ptr--;
  *str_ptr++='\n';
  *str_ptr=0;  
  configFile.write((byte*)str_tmp, strlen(str_tmp));

  sprintf(str_tmp,"Alarms:");
  str_ptr=str_tmp+strlen(str_tmp);
  for (int i=0;i<7;i++) {
    if (clock_alarm_weekday_on[i]) *str_ptr++='1';
    else *str_ptr++='0';
    *str_ptr++='|';
    *str_ptr++='0'+((clock_alarm_weekday_mp3_index[i]+1)/10)%10;
    *str_ptr++='0'+((clock_alarm_weekday_mp3_index[i]+1)/1)%10;    
    *str_ptr++=',';
    *str_ptr++='0'+((clock_alarm_weekday_hour[i])/10)%10;
    *str_ptr++='0'+((clock_alarm_weekday_hour[i])/1)%10;    
    *str_ptr++=':';
    *str_ptr++='0'+((clock_alarm_weekday_minute[i])/10)%10;
    *str_ptr++='0'+((clock_alarm_weekday_minute[i])/1)%10;
    *str_ptr++=',';
  }  
  str_ptr--;
  *str_ptr++='\n';
  *str_ptr=0;  
  configFile.write((byte*)str_tmp, strlen(str_tmp));


  sprintf(str_tmp,"GIF directories:");
  str_ptr=str_tmp+strlen(str_tmp);
  for (int i=0;i<GIFS_ANIM_DIR_MAX;i++) {
    if (clock_gifs_anim_dir_activate[i]) *str_ptr++='1';
    else *str_ptr++='0';
    *str_ptr++=',';    
  }
  str_ptr--;
  *str_ptr++='\n';
  *str_ptr=0;  
  configFile.write((byte*)str_tmp, strlen(str_tmp));
  
  configFile.close();
}

void config_setdefault() {
  strcpy(clk_global_hostname, CLOCK_DEFAULT_HOSTNAME);

  //strcpy(wifi_ssid,STASSID);
  //strcpy(wifi_password,STAPSK);

  for (int i=0;i<GIFS_ANIM_DIR_MAX;i++) clock_gifs_anim_dir_activate[i]=1;

  clk_global_clock_mode=CLOCK_GLOBAL_CLOCK_HHMM_STD;

  clock_brightness = DEFAULT_BRIGHTNESS;
  clock_color_mode = 0; //GREEN
  clock_mode_time = DEFAULT_CLOCK_MODE_TIME;
  clock_mode_mp3 = DEFAULT_CLOCK_MODE_MP3;
  clock_alarm_status=ALARM_OFF;
  
  clk_global_display_time_duration = CLOCK_DELAY_BEFORE_ANIM;

  clk_global_mp3_timermode_duration=CLOCK_MODE_MP3_TIMER_DELAY;

  clock_audio_volume = CLOCK_DEFAULT_MP3_VOLUME;

  clock_alarm_volume = CLOCK_DEFAULT_ALARM_VOLUME;
}

char clock_init_time_internet() {
    /*struct tm timeinfo;
    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return 0;
    } else {
      DateTime new_dt=DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      rtc.adjust(new_dt);
      return 1;
    }*/

  waitForSync();
  /*  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime()); */
    Timezone myTZ;

  // Provide official timezone names
  // https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
  myTZ.setLocation(F("fr"));
  //Serial.print(F("Paris:     "));
  //Serial.println(myTZ.dateTime());
  //Serial.printf("%d/%d/%d %d:%d:%d\n",myTZ.year(), myTZ.month(), myTZ.day(), myTZ.hour(), myTZ.minute(), myTZ.second());
  DateTime new_dt=DateTime(myTZ.year(), myTZ.month(), myTZ.day(), myTZ.hour(), myTZ.minute(), myTZ.second());
  rtc.adjust(new_dt);

  return 1;
}

void setup() {
  Serial.begin(115200);
  Serial.println("initializing...");

  if(!SPIFFS.begin()){ 
    Serial.println("An Error has occurred while mounting SPIFFS");  
  } else Serial.println("SPIFFS init ok");

  //TODO: load config
  config_setdefault();
  //

  //////////////////////////////////////////////////
  /////////// SD CARD  //////////////////////
  //////////////////////////////////////////////////
  pinMode(2, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);

  sdcard_init();
  delay(100);

  config_loadfromsd();
  //////////////////////////////////////////////////
  /////////// WIFI INIT ////////////////////////////
  //////////////////////////////////////////////////
#ifdef WIFI_ON
  wifi_available = 0;
  Serial.printf("Connecting to WiFi %s %s\n",wifi_ssid,wifi_password);
  configureWiFi();
  WiFi.begin(wifi_ssid, wifi_password);

  // Try forever
  int wifi_init_cnt = 0;
  //wifi_available = 1;
  while (!wifi_available) {
    Serial.println("...Connecting to WiFi");
    wifi_init_cnt++;
    if (wifi_init_cnt > 40) {
      //wifi_available = 0;
      //restart
      ESP.restart();
      break;
    }
    delay(250);
  }
  if (wifi_available) {

    Serial.println("Connected");
    Serial.println(WiFi.localIP());

    webServer = new HTTPServer();

    // For every resource available on the server, we need to create a ResourceNode
    // The ResourceNode links URL and HTTP method to a handler function
    ResourceNode * spiffsNode = new ResourceNode("/*", "GET" , &handleSPIFFS);
    //ResourceNode * nodeRoot    = new ResourceNode("/", "GET", &handleRoot);
    ResourceNode * nodeConfig    = new ResourceNode("/config.html", "GET", &handleConfig);
    ResourceNode * nodeUpdateConfig = new ResourceNode("/update_config", "POST", &handleUpdateConfig);
    ResourceNode * nodeTestAlarm = new ResourceNode("/testAlarm", "POST", &handleTestAlarm);
    ResourceNode * node404     = new ResourceNode("", "GET", &handle404);

    // Add the root node to the server
    //webServer->registerNode(nodeRoot);
    webServer->registerNode(nodeConfig);
    webServer->registerNode(nodeTestAlarm);    
    webServer->registerNode(nodeUpdateConfig);
    webServer->registerNode(spiffsNode);
    

    // Add the 404 not found node to the server.
    // The path is ignored for the default node.
    webServer->setDefaultNode(node404);

    Serial.println("Starting server...");
    webServer->start();
    if (webServer->isRunning()) {
      Serial.println("Server ready.");
    }
  }
#endif
  esp_random();

  //RESET MP3 TITLE
  current_mp3_title[0] = 0;

  //////////////////////////////////////////////////
  /////////// LED MATRIX INIT //////////////////////
  //////////////////////////////////////////////////
  matrix_setup();
  matrix_to_clear_flag = 0;
  matrixLayer.setBrightness(clock_brightness);
  update_clock_color();

  delay(100);
  Serial.println("smartmatrix setup done");




  matrix->clear();
  matrix->setFont(&CLOCK_MSG_FONT);
  matrix->setTextSize(1);
  matrix->setCursor(0, 4);
  if (wifi_available) {
    matrix->setTextColor(YRGB_COLOR(matrix->Color, 255, 0, 0));
    matrix->print("IP : ");
    matrix->print(WiFi.localIP());
  }
  else {
    matrix->setTextColor(YRGB_COLOR(matrix->Color, 255, 0, 0));
    matrix->print("No WIFI connection");
  }

  matrix->setFont(&CLOCK_TIME_FONT);
  matrix->setTextSize(1);


  matrix->setTextColor(YRGB_COLOR(matrix->Color, 0, 255, 0));
  matrix->setCursor(4, MATRIX_HEIGHT / 2 + 3);
  matrix->print("Loading");
  matrix->show();
  matrix->setCursor(0, MATRIX_HEIGHT);

  mp3.begin();
  //mp3.reset();
  mp3.stop();

  mp3.setVolume(clock_audio_volume);
  //  delay(200);

  mp3_isPlaying = 0;
  mp3.setPlaybackSource(DfMp3_PlaySource_Sd);
  mp3_TotalAvailableMusicFiles = mp3.getFolderTrackCount(MP3_MUSIC_FOLDER);
  mp3_TotalAvailableABFiles = mp3.getFolderTrackCount(MP3_AUDIOBOOK_FOLDER);
  mp3_TotalAvailableAlarmFiles = mp3.getFolderTrackCount(MP3_ALARM_FOLDER);
  clock_mode_mp3_type = MP3_MUSIC;
  mp3_currentIndex = 0;
  mp3_playback_authorized=0;
  Serial.printf("mp3 music/Audiobook/alarm files: %d/%d/%d\n", mp3_TotalAvailableMusicFiles, mp3_TotalAvailableABFiles, mp3_TotalAvailableAlarmFiles);

  matrix->print(".");
  matrix->show();

  clock_draw_audio_message = 0;


  clock_current_display = CLOCK_DISPLAY_DEFAULT_STARTUP; //AUDIO_SPECTRUM;//CLOCK_DISPLAY_BOOTANIM;
  clock_current_display_switch = 0;



  //////////////////////////////////////////////////
  /////////// RTC CLOCK ////////////////////////////
  //////////////////////////////////////////////////
#ifdef RTC_OFF
  rtc_time_init_ok = 0;
#else
  if (! rtc.begin(DS3231_SDA, DS3231_SCL)) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  if (wifi_available) {
    if (rtc.lostPower()) {
      Serial.println("RTC lost power! => change battery warning");  //TODO print message
      matrix->clear();
      matrix->setFont(&CLOCK_DATE_FONT);
      matrix->setTextSize(1);

      
      matrix->setTextColor(YRGB_COLOR(matrix->Color, 255, 0, 0));
      matrix->setCursor(4, MATRIX_HEIGHT / 2);
      matrix->print("RTC Battery KO");
      matrix->show();
      delay(2000);
      matrix->clear();
      matrix->setFont(&CLOCK_TIME_FONT);
      matrix->setTextSize(1);

      matrix->setTextColor(YRGB_COLOR(matrix->Color, 0, 255, 0));
      matrix->setCursor(4, MATRIX_HEIGHT / 2 + 3);
      matrix->print("Loading");
      matrix->show();
      matrix->setCursor(0, MATRIX_HEIGHT);

      //matrix->print("RTC Battery KO");
      //matrix->show();
    }
    
#ifdef WIFI_ON
    Serial.print("Init time from NTP\n");
    rtc_time_init_ok = clock_init_time_internet();
#endif
  } else rtc_time_init_ok = 1;

#endif
  matrix->print(".");
  matrix->show();

  ///////////////////////////////////////////////
  //  GIF INIT
  ///////////////////////////////////////////////
  decoder.setScreenClearCallback(screenClearCallback);
  decoder.setUpdateScreenCallback(updateScreenCallback);
  decoder.setDrawPixelCallback(drawPixelCallback);


  decoder.setFileSeekCallback(fileSeekCallback);
  decoder.setFilePositionCallback(filePositionCallback);
  decoder.setFileReadCallback(fileReadCallback);
  decoder.setFileReadBlockCallback(fileReadBlockCallback);
  Serial.println("gif decoder done");

  matrix->print(".");
  matrix->show();
  //////////////////////////////////////////////////
  /////////// PUSH BUTTONS /////////////////////////
  //////////////////////////////////////////////////

  pinMode(PushButton1, INPUT);
  pinMode(PushButton2, INPUT);
  pinMode(PushButton3, INPUT);
  pinMode(PushButton4, INPUT_PULLUP);
  pinMode(PushButton5, INPUT_PULLUP);


  for (int i = 0; i < CLOCK_BUTTON_NB; i++) {
    btn_pressed_time[i] = 0;
    btn_push_status[i] = 0;
  }
  comboBtn[0] = comboBtn[1] = 0;

  matrix->print(".");
  matrix->show();

  
  //////////////////////
  /// TIMER ///////////
  ////////////////////

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 1000000, true);

  // Start an alarm
  timerAlarmEnable(timer);

  timerFPS = timerBegin(1, 80, true);
  timerAttachInterrupt(timerFPS, &onTimerFPS, true);
  timerAlarmWrite(timerFPS, 20000, true);
  timerAlarmEnable(timerFPS);

  gif_in_progress = 0;
  gif_delay_before_restart = clk_global_display_time_duration;
  unsigned long time_beg, time_end;
  time_beg = millis();

  gif_checkGifsFilesCount();
  gif_checkBootLogoGifsFilesCount();
  gif_checkBackgroundGifsFilesCount();

  matrix->print(".");
  matrix->show();


  time_end = millis();
  Serial.printf("Gif browsing took: %lu ms\n", time_end - time_beg);

  clock_mode_mp3_pause = 0;

  if (mp3_playback_authorized)
    if (clock_mode_mp3 == CLOCK_MODE_MP3_TIMER) {
      mp3_startNewMP3(-1, clock_mode_mp3_type);
      clock_mode_mp3_time2play = clk_global_mp3_timermode_duration;
    }

  //Serial.printf("Total RAM: %d / available: %d / max allocatable: %d\n", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());

  //FFT
  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
  pinMode(FFTADC_PINL, INPUT);
 
  
  analogReadResolution(12);
  analogSetWidth(12);
  analogSetClockDiv(1);
  analogSetAttenuation(FFTADC_ANALOGADC_DB);
  
  analogSetPinAttenuation(FFTADC_PINL, FFTADC_ANALOGADC_DB);
  adcAttachPin(FFTADC_PINL);
  //analogSetClockDiv(1);
  
  
  //analogSetClockDiv(255); // 1338mS


  bin_size = (1.0 * samplingFrequency) / samples;

  dc_level_band = 0;
  for (int i = 0; i < BAND_MAX; i++) {
    peak[i] = 0;  //clear peak levels
    peak2[i] = 0;  //clear peak levels
    peak_staytopcnt[i] = 0;
  }

  int actband = 0;  //Calculate bands
  band_min = BAND_MAX;
  band_max = 0;
  for (int i = 1; i < samples / 2; i++) {
    if (i * bin_size > (limit[actband] + (limit[actband + 1] - limit[actband]) / 2)) actband ++;
    if (actband > BAND_MAX - 1) actband = BAND_MAX - 1;
    if (actband < band_min) band_min = actband;
    if (actband > band_max) band_max = actband;
    bin_band[i] = actband;
    //Serial.print(i); Serial.print(" "); Serial.print(i*bin_size); Serial.print(" -> "); Serial.println(actband);
  }
  Serial.printf("min band: %d max_band: %d\n", band_min, band_max);

  matrix->print(".");
  matrix->show();


  xTaskCreatePinnedToCore(
    task_fftLoop, /* Function to implement the task */
    "Task FFT", /* Name of the task */
    2048,  /* Stack size in words */
    NULL,  /* Task input parameter */
    1,  /* Priority of the task */
    &task_fft,  /* Task handle. */
    0); /* Core where the task should run */
}

int counter = 0; // how many times we have seen new value
long btn_check_time = 0;  // the last time the output pin was sampled
int ButtonVal, btn_min, btn_max;


void update_clock_color() {
  switch (clock_color_mode) {
    case 0: //green
      clock_r = 0; clock_g = 255; clock_b = 0;
      break;
    case 1: //blue
      clock_r = 0; clock_g = 0; clock_b = 255;
      break;
    case 2: //red
      clock_r = 255; clock_g = 0; clock_b = 0;
      break;
    case 3: //pink
      clock_r = 255; clock_g = 180; clock_b = 220;
      break;
    case 4: //yellow
      clock_r = 255; clock_g = 255; clock_b = 0;
      break;
    case 5: //white
      clock_r = 255; clock_g = 255; clock_b = 255;
      break;
  }

  scrollingLayer1.setColor(YRGB_COLOR(rgb24, clock_r , clock_g , clock_b));
}

enum {
  ACTION_ADD_1FILE_QUEUE = 1,
  ACTION_PLAY_TIMER_MODE,
  ACTION_PAUSE_AUDIO,
  ACTION_STOP_AUDIO,
  ACTION_SHUFFLE_CURRENT,
  ACTION_SWITCH_MUSIC_AB,
  ACTION_VOL_DOWN,
  ACTION_VOL_UP,
  ACTION_CHANGE_COLOR,
  ACTION_CHANGE_BRIGHTNESS,
  ACTION_TURN_CLOCK_DISPLAY_ONOFF,
  ACTION_CLOCK_STYLE,
  ACTION_RESET
};
void stopAlarm() {
  mp3.stop();
  clock_mode_mp3_type=MP3_MUSIC;
  clock_mode_mp3_files2play=0;
  clock_mode_mp3_time2play=0;
  mp3_isPlaying = 0;
  scrollingLayer1.update("");
}

void ButtonPerformAction() {

  char actionToExecute;

  actionToExecute = 0;

  for (int i = 0; i < CLOCK_BUTTON_NB; i++) {
    if (btn_Pushed & (1 << i)) { //pressed
      if (btn_pressed_time[i] == 0) {
        btn_pressed_time[i] = millis(); //initial push
        btn_push_status[i] = 1; //pushed but not event triggered yet
      } else if (btn_push_status[i] == 1) {
        if (millis() - btn_pressed_time[i] > BTN_LONG_PRESS_TIMING) { //long press
          btn_push_status[i] = 3; //long press
        }
      }
    } else { //released
      btn_push_status[i] = 0;
      if (btn_pressed_time[i] > 0) {
        if (millis() - btn_pressed_time[i] < BTN_LONG_PRESS_TIMING) { //short press
          btn_push_status[i] = 2; //short press
        }
      }
      btn_pressed_time[i] = 0;
    }
  }

  //update btn combo flags
  if (btn_push_status[3]) { //BTN 4 pressed
    comboBtn[0] = 1;
  } else comboBtn[0] = 0;
  if (btn_push_status[4]) { //BTN 5 pressed
    comboBtn[1] = 1;
  } else comboBtn[1] = 0;

  //check button
  if (btn_push_status[0] == 2) { //BTN 1 short press
    if (comboBtn[0] || comboBtn[1]) actionToExecute = ACTION_CHANGE_COLOR;
    else actionToExecute = ACTION_ADD_1FILE_QUEUE;
    btn_push_status[0] = 4;
  }
  if (btn_push_status[0] == 3) { //BTN 1 long press
    if (comboBtn[0] && comboBtn[1]) actionToExecute = ACTION_RESET;
    else if (comboBtn[1]) actionToExecute = ACTION_CLOCK_STYLE;
    else actionToExecute = ACTION_PLAY_TIMER_MODE;
    btn_push_status[0] = 4;
  }
  if (btn_push_status[1] == 2) { //BTN 2 short press
    if (comboBtn[0] || comboBtn[1]) actionToExecute = ACTION_CHANGE_BRIGHTNESS;
    else actionToExecute = ACTION_PAUSE_AUDIO;
    btn_push_status[1] = 4;
  }
  if (btn_push_status[1] == 3) { //BTN 2 long press
    actionToExecute = ACTION_STOP_AUDIO;
    btn_push_status[1] = 4;
  }
  if (btn_push_status[2] == 2) { //BTN 3 short press
    if (comboBtn[0] || comboBtn[1]) actionToExecute = ACTION_TURN_CLOCK_DISPLAY_ONOFF;
    else actionToExecute = ACTION_SHUFFLE_CURRENT;
    btn_push_status[2] = 4;
  }
  if (btn_push_status[2] == 3) { //BTN 3 long press
    actionToExecute = ACTION_SWITCH_MUSIC_AB;
    btn_push_status[2] = 4;
  }
  if (btn_push_status[3] == 2) { //BTN 4 short press
    actionToExecute = ACTION_VOL_DOWN;
    btn_push_status[3] = 4;
  }

  if (btn_push_status[4] == 2) { //BTN 5 short press
    actionToExecute = ACTION_VOL_UP;
    btn_push_status[4] = 4;
  }

  //if (actionToExecute) Serial.printf("Execute action %d\n",actionToExecute);


  switch (actionToExecute) {
    case ACTION_RESET:
      abort();
      break;
    case ACTION_ADD_1FILE_QUEUE: //short press button 1
      //ADD 1 FILE IN QUEUE
      //If in ALARM mode, stop & return to MP3_Music
      if (clock_mode_mp3_type == MP3_ALARM) {
        stopAlarm();
        break;
      }
      if (!mp3_playback_authorized) break;
      
      if (clock_mode_mp3_pause) {
        clock_mode_mp3_pause = 0;
        mp3.start();
      } else {
        if (clock_mode_mp3 != CLOCK_MODE_MP3_PLAY_LIMITED_NB) { //if not same mode, stop current mp3
          clock_mode_mp3_files2play = 0;
        }
        clock_mode_mp3 = CLOCK_MODE_MP3_PLAY_LIMITED_NB;
        clock_mode_mp3_files2play++;
      }
      clock_draw_audio_message = DRAW_MSG_LENGTH;
      
      if (clock_mode_mp3_type == MP3_MUSIC) {
        if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
      } else if (clock_mode_mp3_type == MP3_AUDIOBOOK) {
        if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
      }
      break;
    case ACTION_PLAY_TIMER_MODE:
      //If in ALARM mode, stop & return to MP3_Music
      if (clock_mode_mp3_type == MP3_ALARM) {
        stopAlarm();
        break;
      }
      if (!mp3_playback_authorized) break;
      //SWITCH TO TIMER MODE / 30min
      if (clock_mode_mp3_pause) {
        clock_mode_mp3_pause = 0;
        mp3.start();
      } else {
        clock_mode_mp3 = CLOCK_MODE_MP3_TIMER;
        clock_mode_mp3_time2play = clk_global_mp3_timermode_duration;
        clock_draw_audio_message = DRAW_MSG_LENGTH;
      }
      if (clock_mode_mp3_type == MP3_MUSIC) {
        if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
      } else if (clock_mode_mp3_type == MP3_AUDIOBOOK) {
        if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
      }

      break;
    case ACTION_PAUSE_AUDIO:
      //If in ALARM mode, stop & return to MP3_Music
      if (clock_mode_mp3_type == MP3_ALARM) {
        stopAlarm();
        break;
      }
      if (!mp3_playback_authorized) break;
      // PLAY/PAUSE MP3 AUDIO
      if (mp3_isPlaying) {
        clock_draw_audio_message = DRAW_MSG_LENGTH;
        if (clock_mode_mp3_pause) {
          clock_mode_mp3_pause = 0;

          mp3.start();
        } else {
          clock_mode_mp3_pause = 1;
          mp3.pause();


        }
        if (clock_mode_mp3_type == MP3_MUSIC) {
          if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
        } else if (clock_mode_mp3_type == MP3_AUDIOBOOK) {
          if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
        }
      }
      break;
    case ACTION_STOP_AUDIO:
      // STOP AUDIO & CLEAR QUEUE
      //If in ALARM mode, stop & return to MP3_Music
      if (clock_mode_mp3_type == MP3_ALARM) {
        stopAlarm();
        break;
      }
      if (!mp3_playback_authorized) break;
      mp3.stop();

      

      mp3_isPlaying = 0;
      scrollingLayer1.update("");
      clock_mode_mp3_files2play = 0;
      clock_mode_mp3 = CLOCK_MODE_MP3_NO_AUDIO;
      clock_draw_audio_message = DRAW_MSG_LENGTH;
      break;
    case ACTION_SHUFFLE_CURRENT:
      // START A NEW MP3/AB

      //If in ALARM mode, stop & return to MP3_Music
      if (clock_mode_mp3_type == MP3_ALARM) {
        stopAlarm();
        break;
      }
      if (!mp3_playback_authorized) break;
      
      if (clock_mode_mp3_pause) {
        clock_mode_mp3_pause = 0;
      }
      mp3_startNewMP3(-1, clock_mode_mp3_type);
      break;
    case ACTION_SWITCH_MUSIC_AB:
      // SWITCH MUSIC/AB
      //If in ALARM mode, stop & return to MP3_Music
      if (clock_mode_mp3_type == MP3_ALARM) {
        stopAlarm();
        break;
      }
      if (!mp3_playback_authorized) break;
      
      clock_draw_audio_message = DRAW_MSG_LENGTH;
      if (clock_mode_mp3_type == MP3_MUSIC) {
        clock_mode_mp3_type = MP3_AUDIOBOOK;
        mp3.stop();

        mp3_isPlaying = 0;
        clock_mode_mp3 = CLOCK_MODE_MP3_PLAY_LIMITED_NB;
        clock_mode_mp3_files2play = 1;
      } else {
        clock_mode_mp3_type = MP3_MUSIC;
        mp3.stop();

        mp3_isPlaying = 0;
        clock_mode_mp3 = CLOCK_MODE_MP3_PLAY_LIMITED_NB;
        clock_mode_mp3_files2play = 1;
      }
      break;
    case ACTION_VOL_DOWN:
      if (clock_audio_volume > 0) clock_audio_volume--;
      mp3.setVolume(clock_audio_volume);
      clock_draw_audio_message = DRAW_MSG_LENGTH;
      break;
    case ACTION_VOL_UP:
      if (clock_audio_volume < 30) clock_audio_volume++;
      mp3.setVolume(clock_audio_volume);
      clock_draw_audio_message = DRAW_MSG_LENGTH;
      break;
    case ACTION_CHANGE_COLOR:
      if (clock_color_mode < CLOCK_COLOR_MODE_NB) clock_color_mode++;
      else clock_color_mode = 0;
      update_clock_color();
      break;
    case ACTION_CLOCK_STYLE:
      //switch Font based / morphing time display
      break;
    case ACTION_CHANGE_BRIGHTNESS:
      if (clock_brightness == 0) {
        clock_brightness = 16;
      } else if (clock_brightness == 16) {
        clock_brightness = 32;
      } else if (clock_brightness == 32) {
        clock_brightness = 64;
      } else if (clock_brightness == 64) {
        clock_brightness = 128;
      } else if (clock_brightness == 128) {
        clock_brightness = 192;
      } else if (clock_brightness == 192) {
        clock_brightness = 16;
      }
      matrixLayer.setBrightness(clock_brightness);
      break;
    case ACTION_TURN_CLOCK_DISPLAY_ONOFF:
      if (clock_brightness) clock_brightness = 0;
      else clock_brightness=16;
      matrixLayer.setBrightness(clock_brightness);
      break;
  }
}

void displayBand(int band, float level) {
  int dsize = 0;
  CRGB color1;
  int r, g, b;
  int xofs;

  CRGBPalette16 cur_palette;
  cur_palette = RainbowColors_p;//ForestColors_p ;//OceanColors_p; //PartyColors_p ; //RainbowColors_p;

  for (int k = 0; k < YMAX; k++) if (level >= LEDlimit[k]) dsize = k; //find the upper LED position
  //dsize=level*4*8*YMAX;

  if (dsize < 0) {
    return; //level is below the last limit
  }

  if (dsize >= YMAX) dsize = YMAX;  //safety only

  if (dsize > peak2[band]) {
    peak2[band] = dsize; //peak level monitoring
  }
  if (dsize > peak[band]) {
    peak[band] = dsize; //peak level monitoring
    peak_staytopcnt[band] = 5;
  }

  xofs = (PANEL_WIDTH - (band_max - band_min + 1) * 2) / 2;

  for (int s = 0; s <= peak2[band]/*dsize*/; s++) {


    r = cur_palette[(YMAX - s) * 15 / YMAX].red;
    g = cur_palette[(YMAX - s) * 15 / YMAX].green;
    b = cur_palette[(YMAX - s) * 15 / YMAX].blue;

#ifdef PANEL_RBG
    color1.red = matrix->gamma[r]; color1.blue = matrix->gamma[g]; color1.green = matrix->gamma[b];
#else
    color1.red = matrix->gamma[r]; color1.green = matrix->gamma[g]; color1.blue = matrix->gamma[b];
#endif


    matrix->drawPixel(xofs + (band - band_min) * 2, PANEL_HEIGHT - s - 1 - 4, color1);

  }


}

void  showDCLevel() {
  int dsize = 0;
  CRGB color1;
  int r, g, b;
  int xofs;

  CRGBPalette16 cur_palette;
  cur_palette = PartyColors_p;//ForestColors_p ;//OceanColors_p; //PartyColors_p ; //RainbowColors_p;

  for (int k = 0; k < YMAX; k++) if (dc_level >= LEDlimit[k]) dsize = k; //find the upper LED position
  //dsize=dc_level*4*YMAX;

  if (dsize >= YMAX) dsize = YMAX;  //safety only
  if (dc_level_band < dsize) dc_level_band = dsize;

  for (int s = 0; s < dc_level_band; s++) {


    r = cur_palette[(YMAX - s) * 15 / YMAX].red;
    g = cur_palette[(YMAX - s) * 15 / YMAX].green;
    b = cur_palette[(YMAX - s) * 15 / YMAX].blue;

#ifdef PANEL_RBG
    color1.red = matrix->gamma[r]; color1.blue = matrix->gamma[g]; color1.green = matrix->gamma[b];
#else
    color1.red = matrix->gamma[r]; color1.green = matrix->gamma[g]; color1.blue = matrix->gamma[b];
#endif


    for (int y = 0; y < 5; y++) {
      matrix->drawPixel(PANEL_WIDTH / 2 - s - 1, PANEL_HEIGHT - y, color1);
      matrix->drawPixel(PANEL_WIDTH / 2 + s, PANEL_HEIGHT - y, color1);
    }
  }
}

void  showOldPeaks() {
  CRGB color1;
  int s;
  int r, g, b;
  int xofs;
  CRGBPalette16 cur_palette;
  static long lastDecay = 0;
  static long lastDecay2 = 0;
  cur_palette = RainbowColors_p;//ForestColors_p ; //OceanColors_p ; //PartyColors_p ; //RainbowColors_p;

  xofs = (PANEL_WIDTH - (band_max - band_min + 1) * 2) / 2;
  for (byte band = band_min+1; band <= band_max; band++) {
    s = peak[band];

    r = cur_palette[(YMAX - s) * 15 / YMAX].red;
    g = cur_palette[(YMAX - s) * 15 / YMAX].green;
    b = cur_palette[(YMAX - s) * 15 / YMAX].blue;

    color1.red = r; color1.green = g; color1.blue = b;

    if (s == 0) {
      r = r / 2; g = g / 2; b = b / 2;
    } else {
      r = r + 128; g = g + 128; b = b + 128;
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
    }

#ifdef PANEL_RBG
    color1.red = matrix->gamma[r]; color1.blue = matrix->gamma[g]; color1.green = matrix->gamma[b];
#else
    color1.red = matrix->gamma[r]; color1.green = matrix->gamma[g]; color1.blue = matrix->gamma[b];
#endif


    matrix->drawPixel(xofs + (band - band_min) * 2, PANEL_HEIGHT - peak[band] - 1 - 4, color1);



  }
  if (millis() - lastDecay >= 20 * 5 - 1) {
    amin = 4096; amax = 0;

    lastDecay = millis();
    for (byte band = band_min+1; band <= band_max; band++) {
      if (peak_staytopcnt[band] > 0) peak_staytopcnt[band]--;
      else if (peak[band] > 0) peak[band]--;
    }
  } // Decay the peak

  if (millis() - lastDecay2 >= 20 - 1) {
    amin = 4096; amax = 0;

    lastDecay2 = millis();
    for (byte band = band_min+1; band <= band_max; band++) {
      if (peak2[band] > 0) peak2[band]--;
    }
    if (dc_level_band > 0) dc_level_band--;
  } // Decay the peak
}


void task_fftLoop(void *parameter) {

  for (;;) {
    yield();

    ////////////////////////////
    // BUTTONS CHECK //////////
    //////////////////////////


    // If we have gone on to the next millisecond
    if (millis() != btn_check_time) {
      btn_Pushed = 0;
      if (digitalRead(PushButton1) == 0) btn_Pushed |= 1 << 0;
      if (digitalRead(PushButton2) == 0) btn_Pushed |= 1 << 1;
      if (digitalRead(PushButton3) == 0) btn_Pushed |= 1 << 2;
      if (digitalRead(PushButton4) == 0) btn_Pushed |= 1 << 3;
      if (digitalRead(PushButton5) == 0) btn_Pushed |= 1 << 4;

      //Serial.printf("btn: %08X\n",btn_Pushed);
      // check analog pin for the button value and save it to ButtonVal

      btn_check_time = millis();
    }

    ///////////////////
    /// FFT //////////
    ////////////////
    vTaskDelay(1);
    int vmin, vmax;
    vmin = 4096;
    vmax = 0;
    newTime = micros();
    for (int i = 0; i < samples; i++) {
      int val=0;
      
      //for (int j=0;j<8;j++) {        
      val += analogRead(FFTADC_PINL);      
      //}
      
      if (val < vmin) vmin = val;
      if (val > vmax) vmax = val;      
      vReal[i] = val * FFT_ADC_MULTIPLY_FACTOR; // Using Arduino ADC nomenclature. A conversion takes about 1uS on an ESP32

      vImag[i] = 0;      

      while ((micros() - newTime) < sampling_period_us) {
      }
      newTime+=sampling_period_us;
    }
    //if (vmax) Serial.printf("min/max: %d/%d\n",vmin,vmax);
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);	/* Weigh data */
    FFT.compute(FFTDirection::Forward); /* Compute FFT */
    FFT.complexToMagnitude(); /* Compute magnitudes */

    float sum = 0;
    dc_level = vReal[0] * 0.1 / BIN_MAX;

    FFT.dcRemoval();
    for (int i = 1; i < (samples / 2); i++) { // 0 = DC level!
      if (vReal[i] > FFT_VREAL_THRESHOLD) {
        if (vReal[i] > sum) sum = vReal[i];       //maximum among bins
        //sum += vReal[i];   //sum of bins
      }
      if (sum > amax) amax = sum;
      if (sum < amin) amin = sum;
      if (bin_band[i] != bin_band[i + 1]) {
        //displayBand(bin_band[i],sum / BIN_MAX * 1);
        fft_bands[bin_band[i]] = sum * 0.3 / BIN_MAX ;
        sum = 0;
      }
    }

  }
}

void update_audioFFT() {
  for (int i = band_min+1; i <= band_max; i++) { //
    displayBand(i, fft_bands[i]);
  }
  showOldPeaks();
  showDCLevel();
}

void loop() {
  int result;
  static long long loop_cnt=0;

  if (matrix_to_clear_flag) {
    matrix_to_clear_flag = 0;
    matrix->clear();
  }

  switch (clock_current_display) {
    case CLOCK_DISPLAY_BOOTANIM:
      //logo intro
      if (!gif_in_progress) { //Launch BOOT ANIM
        if (gif_startNewBootLogo(-1)) {
          screenClearCallback();
          gif_delay_before_restart = 0;
          decoder.startDecoding();
          gif_in_progress = GIF_ANIM_NOCLOCK;
        } else {  //Cannot launch anim, switch to Clock Time mode
          Serial.println("Cannot launch Boot anim");
          if (mp3_isPlaying) {
            clock_current_display_switch = 0;
            clock_current_display = CLOCK_DISPLAY_AUDIO_SPECTRUM;
          }
          else clock_current_display = CLOCK_DISPLAY_TIME_WITH_BG_ANIM;
          gif_delay_before_restart = clk_global_display_time_duration;
        }
      } else { //BOOT ANIM in progress
        result = decoder.decodeFrame();
        if ((result != ERROR_WAITING) && (result != ERROR_NONE)) { //Done with anim
          gif_file.close();
          gif_in_progress = 0;
          gif_delay_before_restart = clk_global_display_time_duration;
          if (mp3_isPlaying) {
            clock_current_display_switch = 0;
            if (clock_current_display_switch & 1) clock_current_display = CLOCK_DISPLAY_TIME_WITH_BG_ANIM;
            else clock_current_display = CLOCK_DISPLAY_AUDIO_SPECTRUM;
          } else clock_current_display = CLOCK_DISPLAY_TIME_WITH_BG_ANIM;
          screenClearCallback();
          clock_draw_msg();
          clock_draw_time(0);
          //if MP3 in progress, show name
          if (mp3_isPlaying) {
            if (clock_mode_mp3_type == MP3_MUSIC) {
              if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
            } else if (clock_mode_mp3_type == MP3_AUDIOBOOK) {
              if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
            }
          }

        }
      }
      break;
    case CLOCK_DISPLAY_TIME_WITH_BG_ANIM:
      if (gif_in_progress) {  //PLAY CURRENT CLOCK GIF
        result = decoder.decodeFrame();
        if ((result != ERROR_WAITING) && (result != ERROR_NONE)) { //CURRENT Clock GIF ended
          gif_file.close();
          clock_current_display = CLOCK_DISPLAY_TIME; //Display time
          gif_in_progress = 0;
          screenClearCallback();
          clock_draw_msg();
          clock_draw_time(0);
          if (clock_brightness) matrix->show();
        }
      } else {    //STAR NEW BG TIME GIF
        if (gif_startNewBGTimeAnim(-1)) {
          screenClearCallback();
          gif_in_progress = GIF_ANIM_CLOCK;
          decoder.startDecoding();
        } else {  //Cannot launch anim, switch to Clock Time mode
          Serial.printf("Could not launch new gif\n");
        }
      }
      if (!gif_in_progress) {  //If countdown is over & clock gif ended, start new gif anim
        clock_current_display = CLOCK_DISPLAY_TIME;
      }
      break;
    case CLOCK_DISPLAY_TIME:
      screenClearCallback(); //DISPLAY TIME
      clock_draw_msg();
      if (clock_brightness) clock_draw_time(0);
      if (clock_brightness) matrix->show();
      if (!gif_delay_before_restart) {  //If countdown is over & clock gif ended, start new gif anim
        clock_current_display = CLOCK_DISPLAY_GIFANIM;
      }
      break;
    case CLOCK_DISPLAY_GIFANIM:
      if (gif_in_progress) {  //PLAY CURRENT GIF
        result = decoder.decodeFrame();
        if ((result != ERROR_WAITING) && (result != ERROR_NONE)) { //CURRENT GIF ended
          gif_file.close();
          if (mp3_isPlaying) {
            clock_current_display_switch++;
            if (clock_current_display_switch & 1) clock_current_display = CLOCK_DISPLAY_TIME_WITH_BG_ANIM;
            else clock_current_display = CLOCK_DISPLAY_AUDIO_SPECTRUM;
          } else clock_current_display = CLOCK_DISPLAY_TIME_WITH_BG_ANIM;
          gif_in_progress = 0;
          gif_delay_before_restart = clk_global_display_time_duration;
          screenClearCallback();
          clock_draw_msg();
          clock_draw_time(0);
          if (clock_brightness) matrix->show();

          if (mp3_isPlaying) {
            if (clock_mode_mp3_type == MP3_MUSIC) {
              if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
            } else if (clock_mode_mp3_type == MP3_AUDIOBOOK) {
              if (current_mp3_title[0]) scrollingLayer1.start(current_mp3_title, 1);
            }
          }

        }
      } else {    //STAR NEW GIF
        if (gif_startNewAnim(-1)) {
          screenClearCallback();
          gif_in_progress = GIF_ANIM_NOCLOCK;
          scrollingLayer1.update("");  //Remove any scrolling info
          decoder.startDecoding();
        } else {  //Cannot launch anim, switch to Clock Time mode
          if (mp3_isPlaying) {
            clock_current_display_switch++;
            if (clock_current_display_switch & 1) clock_current_display = CLOCK_DISPLAY_TIME_WITH_BG_ANIM;
            else clock_current_display = CLOCK_DISPLAY_AUDIO_SPECTRUM;
          } else clock_current_display = CLOCK_DISPLAY_TIME_WITH_BG_ANIM;
          gif_delay_before_restart = clk_global_display_time_duration;
          Serial.printf("Could not launch new gif\n");
        }
      }
      break;
    case CLOCK_DISPLAY_AUDIO_SPECTRUM:
      screenClearCallback();
      clock_draw_msg();
      if (clock_brightness) {
        update_audioFFT();
        clock_draw_time(1);
        matrix->show();
      }
      if (!gif_delay_before_restart) {  //If countdown is over & clock gif ended, start new gif anim
        clock_current_display = CLOCK_DISPLAY_GIFANIM;
      }
      if (!mp3_isPlaying) clock_current_display=CLOCK_DISPLAY_TIME;
      break;
    case CLOCK_DISPLAY_APNG:

      break;
    default:
      break;

  }



  // Timer fire every second
  // Update Gif trigger counter
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    uint32_t isrCount = 0, isrTime = 0;
    static int mycounter=0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);

    mycounter++;
    if (mycounter>=10) {
      mycounter=0;
      int wifistatus=WiFi.status();
      switch (wifistatus) {
        case 3:Serial.println("Wifi connected");break;
        default:Serial.printf("Wifi not connected, code: %d\n",wifistatus);break;
      }            
      Serial.printf("Total RAM: %d / available: %d / max allocatable: %d\n", ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());
    }
    
    if (rtc_time_init_ok) {
      DateTime now = rtc.now();        
      //check if gif authorized / time
      if (clock_gif_animation_on_hours[now.hour()]) {
        if (gif_delay_before_restart) gif_delay_before_restart--;
      }

      //check if mp3 authorized / time
      if (clock_mp3_authorized_hours[now.hour()]) {
        mp3_playback_authorized=1;
      } else {
        mp3_playback_authorized=0;
        //If an MP3 is playing, stop it, except if it is the alarm
        if ((clock_mode_mp3_type!=MP3_ALARM)&&(mp3_isPlaying>0)) {
          mp3.stop();
          mp3_isPlaying=0;
          clock_mode_mp3_files2play=0;
          clock_mode_mp3_time2play=0;
        }
      }

      //check if alarm is to be triggered
      if (clock_alarm_status==ALARM_OFF) {
        int day=now.dayOfTheWeek();
        if (clock_alarm_weekday_on[day]) {
          if ((clock_alarm_weekday_hour[day]==now.hour()) && (clock_alarm_weekday_minute[day]==now.minute())) {
            clock_alarm_status=ALARM_SHOULD_RING;
            clock_alarm_mp3_index=clock_alarm_weekday_mp3_index[day];
          }
        }
      }
    } else gif_delay_before_restart--; //no clock, let gif play
  
    if (clock_mode_mp3_time2play) {
      clock_mode_mp3_time2play--;
    }

    loop_cnt++;

  }

  if ((clock_alarm_status==ALARM_RINGING)&&(mp3_isPlaying<=0)) {
    //ALARM stopped, return to MP3 Music mode
    clock_alarm_status=ALARM_STOPPED;  
    clock_mode_mp3_type=MP3_MUSIC;
  }
  if (clock_alarm_status==ALARM_SHOULD_RING) {
    clock_alarm_status=ALARM_RINGING;
    mp3.stop();   
    clock_mode_mp3_type=MP3_ALARM;
    mp3_startNewMP3(clock_alarm_mp3_index, clock_mode_mp3_type);
  }
  
  // calling mp3.loop() periodically allows for notifications
  // to be handled without interrupts
  mp3.loop();
  if (mp3_isPlaying == -1) {
    //1st call generate error
    mp3.stop();

    mp3.setPlaybackSource(DfMp3_PlaySource_Sd);

    Serial.printf("mp3 folders: %d\n", mp3.getTotalFolderCount());
    mp3_TotalAvailableMusicFiles = mp3.getFolderTrackCount(MP3_MUSIC_FOLDER);
    mp3_TotalAvailableABFiles = mp3.getFolderTrackCount(MP3_AUDIOBOOK_FOLDER);
    mp3_TotalAvailableAlarmFiles = mp3.getFolderTrackCount(MP3_ALARM_FOLDER);
    Serial.printf("mp3 music/audiobook/alarm files: %d/%d/%d\n", mp3_TotalAvailableMusicFiles, mp3_TotalAvailableABFiles, mp3_TotalAvailableAlarmFiles);

    clock_mode_mp3_type = MP3_MUSIC;
    mp3_currentIndex = 0;
    mp3_isPlaying = 0;
  } else if (mp3_isPlaying == -2) {
    mp3.stop();

    mp3_TotalAvailableMusicFiles = 0;
    mp3_TotalAvailableABFiles = 0;
    mp3_TotalAvailableAlarmFiles = 0;
    clock_mode_mp3_type = MP3_MUSIC;
    mp3_currentIndex = 0;
    mp3_isPlaying = 0;
  }

  if (mp3_isPlaying) {
    //Serial.println("playing");
  } else if (mp3_playback_authorized) { //MP3 done
    //Serial.println("not playing");
    if (clock_mode_mp3 == CLOCK_MODE_MP3_TIMER) { //TIMER MODE
      mp3_startNewMP3(-1, clock_mode_mp3_type);
      if (mp3_isPlaying) clock_draw_audio_message = DRAW_MSG_LENGTH;
    } else if (clock_mode_mp3 == CLOCK_MODE_MP3_PLAY_LIMITED_NB) { //n songs play mode
      if (clock_mode_mp3_files2play > 0) {
        clock_mode_mp3_files2play--;
        mp3_startNewMP3(-1, clock_mode_mp3_type);
        if (mp3_startNewMP3) clock_draw_audio_message = DRAW_MSG_LENGTH;
      }
    }
  }

  if (clock_mode_mp3 == CLOCK_MODE_MP3_TIMER) { //CHECK IF TIMER IS REACHED, THEN STOP
    if (clock_mode_mp3_time2play <= 0) {
      mp3.stop();

      mp3_isPlaying = 0;
      scrollingLayer1.update("");
      clock_mode_mp3 = CLOCK_MODE_MP3_NO_AUDIO;
      clock_mode_mp3_files2play = 0;
    }
  }


  ButtonPerformAction();

  
  if (wifi_available) {
    webServer->loop();

    if (loop_cnt==CLOCK_NTP_UPDATE_DELAY_IN_SECONDS) {
      loop_cnt=0;
      Serial.print("reinit time from NTP\n");
      if (clock_init_time_internet()) {
        rtc_time_init_ok = 1;
      }
    }
  }

}