/*
This code is written by Devbrat Raghuvanshi for TTGO T-Display module.
libraries used apart from ESP32 board is given below.
https://github.com/Bodmer/TFT_eSPI // to controle onboard LCD display 
https://github.com/LennartHennigs/Button2 // to controle onboard given button PIN 0 and PIN 35 as navigation

Note: I was faceing some issued with Button2 and wifi libraries so I used multithreading solve the problem. 
*/
 
// Define meter size
#define M_SIZE 1
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "WiFi.h"
#include "Button2.h";
Button2 buttonA = Button2(0);
Button2 buttonB = Button2(35);

TaskHandle_t TaskScan; //task1
TaskHandle_t TaskDisplay; // task2

typedef struct {
   byte  index;
   String ssid;
   int16_t  rssi;
} Network;
const byte networkCount = 10;
Network networks[networkCount];
bool isScanComplete = false;

String SSID_NAME= "";
int16_t SSID_Index= 0;
int16_t SSID_Count= 0;
int16_t Strenth = -100;

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define TFT_GREY 0x5AEB
#define TFT_ORANGE      0xFD20      /* 255, 165,   0 */
float ltx = 0;    // Saved x coord of bottom of needle
uint16_t osx = M_SIZE*120, osy = M_SIZE*120; // Saved x & y coords
uint32_t updateTime = 0;       // time for next update
int old_analog =  0; // Value last displayed



void setup(void) {
  
Serial.begin(115200); // For debug

/* Function to implement the task *//* Name of the task *//* Stack size in words *//* Task input parameter *//* Priority of the task *//* Task handle. *//* Core where the task should run */
xTaskCreatePinnedToCore(TaskScanNetwork,"Task1",10000,NULL, 0,&TaskScan,0); 
xTaskCreatePinnedToCore(TaskDisplayNetwork,"Task2",10000,NULL,1,&TaskDisplay,1);

}


void TaskScanNetwork( void * parameter) {
  Serial.print("TaskScanNetwork running on core ");
  Serial.println(xPortGetCoreID());
  int n = 0;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  for(;;) {
     SSID_Count = WiFi.scanNetworks();
     isScanComplete = true;
     //Serial.print("SSID_Count");Serial.println(SSID_Count);

      if (SSID_Count <= 0) {
         SSID_NAME = "No Network";
         plotNeedle("",0, 0);
    } else {
      //update network data
      for(n = 0; n < networkCount ; n++  ){
        if(n < SSID_Count){
           networks[n].ssid =  WiFi.SSID(n);
           networks[n].rssi =  WiFi.RSSI(n);
         }else{           
            networks[n].ssid =  "--";
            networks[n].rssi =  -100;
          }
        }
        WiFi.scanDelete();
        delay(50);
        updateNetworkInfo();
      
    }
  }
}

void TaskDisplayNetwork( void * parameter) {
  Serial.print("TaskDisplayNetwork running on core ");
  Serial.println(xPortGetCoreID());

  buttonA.setClickHandler(click);
  buttonB.setClickHandler(click);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  initializeNetworks();
  //getNetworkInfo();
  analogMeter(); // Draw analogue meter
  displaySearching();

  for(;;) {
      buttonA.loop();
      buttonB.loop();
      //getNetworkInfo();
      delay(10);
    }
}

void loop() { }

void click(Button2& btn) {
  
    if (btn == buttonA) {
      if(SSID_Index > 0){
        SSID_Index = SSID_Index -1;
        }
      Serial.print("A clicked SSID_Index:");Serial.println(SSID_Index);
      
    } else if (btn == buttonB) {
      if(SSID_Index < networkCount-1){
        SSID_Index = SSID_Index +1;
        }
      Serial.print("B clicked SSID_Index:");Serial.println(SSID_Index);
    }
}

void updateNetworkInfo(){
        //update meter reading
      SSID_Index = SSID_Index >= SSID_Count ? SSID_Count-1:SSID_Index; // reset current index if out of range
      SSID_NAME = networks[SSID_Index].ssid.substring(0, 5);
      SSID_NAME = String(SSID_Index+1)+SSID_NAME;
      SSID_NAME = String(SSID_Index > 0 ? "<":"")+SSID_NAME;
      SSID_NAME = SSID_NAME + String(SSID_Index < SSID_Count-1 ? ">":"");
      Strenth = map(networks[SSID_Index].rssi,-100,0,0,100);
      plotNeedle(SSID_NAME,Strenth, 0);
}
void displaySearching(){
    //fancy code for 3 sec delay  :P
    int d = 0;
    int value[6] = {0, 0, 0, 0, 0, 0};
    SSID_NAME = "Scaning";
    while(!isScanComplete){
        d += 4; if (d >= 360) d = 0;
        value[0] = 50 + 50 * sin((d + 0) * 0.0174532925);
        plotNeedle(SSID_NAME,value[0], 0); 
        delay(50);
      }
}
void initializeNetworks(){
  for(int nt = 0; nt < networkCount ; nt++  ){
    networks[nt].index = nt;
    networks[nt].ssid = "network"+String(nt);
    networks[nt].rssi = 0;
    }
  }

  void getNetworkInfo(){

    Serial.println("Networks:");
    Serial.println("=========================================");
    
    for(int nt = 0; nt < networkCount ; nt++  ){
        Serial.print("Index:");
        Serial.print(networks[nt].index);
        
        Serial.print("       SSID:");
        Serial.print(networks[nt].ssid);
  
        Serial.print("       RSSI:");
        Serial.print(networks[nt].rssi);
        Serial.println();
        delay(500);
      }
    Serial.println("=========================================");
  }

  
// #########################################################################
//  Draw the analogue meter on the screen
// #########################################################################
void analogMeter()
{

  // Meter outline
  tft.fillRect(0, 0, M_SIZE*239, M_SIZE*131, TFT_GREY);
  tft.fillRect(1, M_SIZE*3, M_SIZE*234, M_SIZE*125, TFT_WHITE);

  tft.setTextColor(TFT_BLACK);  // Text colour

  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
  for (int i = -50; i < 51; i += 5) {
    // Long scale tick length
    int tl = 15;

    // Coodinates of tick to draw
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    uint16_t y0 = sy * (M_SIZE*100 + tl) + M_SIZE*150;
    uint16_t x1 = sx * M_SIZE*100 + M_SIZE*120;
    uint16_t y1 = sy * M_SIZE*100 + M_SIZE*150;

    // Coordinates of next tick for zone fill
    float sx2 = cos((i + 5 - 90) * 0.0174532925);
    float sy2 = sin((i + 5 - 90) * 0.0174532925);
    int x2 = sx2 * (M_SIZE*100 + tl) + M_SIZE*120;
    int y2 = sy2 * (M_SIZE*100 + tl) + M_SIZE*150;
    int x3 = sx2 * M_SIZE*100 + M_SIZE*120;
    int y3 = sy2 * M_SIZE*100 + M_SIZE*150;

     //Red zone limits
    if (i >= -50 && i < -30) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_RED);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_RED);
    }
     //ORANGE zone limits
    if (i >= -30 && i < -10) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_ORANGE);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_ORANGE);
    }

    // YELLOW zone limits
    if (i >=-10 && i < 10) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
    }

    // GREEN zone limits
    if (i >= 10 && i < 30) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
    }
    
    // BLUE zone limits
    if (i >= 30 && i < 50) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_BLUE);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_BLUE);
    }
    // Short scale tick length
    if (i % 25 != 0) tl = 8;

    // Recalculate coords incase tick lenght changed
    x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    y0 = sy * (M_SIZE*100 + tl) + M_SIZE*150;
    x1 = sx * M_SIZE*100 + M_SIZE*120;
    y1 = sy * M_SIZE*100 + M_SIZE*150;

    // Draw tick
    tft.drawLine(x0, y0, x1, y1, TFT_BLACK);

    // Check if labels should be drawn, with position tweaks
    if (i % 25 == 0) {
      // Calculate label positions
      x0 = sx * (M_SIZE*100 + tl + 10) + M_SIZE*120;
      y0 = sy * (M_SIZE*100 + tl + 10) + M_SIZE*150;
      switch (i / 25) {
        case -2: tft.drawCentreString("0", x0+4, y0-4, 1); break;
        case -1: tft.drawCentreString("25", x0+2, y0, 1); break;
        case 0: tft.drawCentreString("50", x0, y0, 1); break;
        case 1: tft.drawCentreString("75", x0, y0, 1); break;
        case 2: tft.drawCentreString("100", x0-2, y0-4, 1); break;
      }
    }

    // Now draw the arc of the scale
    sx = cos((i + 5 - 90) * 0.0174532925);
    sy = sin((i + 5 - 90) * 0.0174532925);
    x0 = sx * M_SIZE*100 + M_SIZE*120;
    y0 = sy * M_SIZE*100 + M_SIZE*150;
    // Draw scale arc, don't draw the last part
    if (i < 50) tft.drawLine(x0, y0, x1, y1, TFT_BLACK);
  }

  tft.drawString("dBm", M_SIZE*(3 + 230 - 40), M_SIZE*(119 - 20), 2); // Units at bottom right
  tft.drawCentreString("Scaning", M_SIZE*120, M_SIZE*75, 4); // Comment out to avoid font 4
  tft.drawRect(1, M_SIZE*3, M_SIZE*236, M_SIZE*126, TFT_BLACK); // Draw bezel line
  plotNeedle("",0, 0); // Put meter needle at 0
}


// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void plotNeedle(String ssid, int value, byte ms_delay)
{
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  char buf[8]; dtostrf(value, 4, 0, buf);
  tft.drawRightString(buf, 33, M_SIZE*(119 - 20), 2);

  if (value < -10) value = -10; // Limit value to emulate needle end stops
  if (value > 110) value = 110;

  // Move the needle until new value reached
  while (!(value == old_analog)) {
    if (old_analog < value) old_analog++;
    else old_analog--;

    if (ms_delay == 0) old_analog = value; // Update immediately if delay is 0

    float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
    // Calculate tip of needle coords
    float sx = cos(sdeg * 0.0174532925);
    float sy = sin(sdeg * 0.0174532925);

    // Calculate x delta of needle start (does not start at pivot point)
    float tx = tan((sdeg + 90) * 0.0174532925);

    // Erase old needle image
    tft.drawLine(M_SIZE*(120 + 24 * ltx) - 1, M_SIZE*(150 - 24), osx - 1, osy, TFT_WHITE);
    tft.drawLine(M_SIZE*(120 + 24 * ltx), M_SIZE*(150 - 24), osx, osy, TFT_WHITE);
    tft.drawLine(M_SIZE*(120 + 24 * ltx) + 1, M_SIZE*(150 - 24), osx + 1, osy, TFT_WHITE);

    // Re-plot text under needle
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.drawCentreString("                            ", M_SIZE*120, M_SIZE*75, 4);
    tft.drawCentreString(ssid, M_SIZE*120, M_SIZE*75, 4); // // Comment out to avoid font 4

    // Store new needle end coords for next erase
    ltx = tx;
    osx = M_SIZE*(sx * 98 + 120);
    osy = M_SIZE*(sy * 98 + 150);

    // Draw the needle in the new postion, magenta makes needle a bit bolder
    // draws 3 lines to thicken needle
    tft.drawLine(M_SIZE*(120 + 24 * ltx) - 1, M_SIZE*(150 - 24), osx - 1, osy, TFT_RED);
    tft.drawLine(M_SIZE*(120 + 24 * ltx), M_SIZE*(150 - 24), osx, osy, TFT_MAGENTA);
    tft.drawLine(M_SIZE*(120 + 24 * ltx) + 1, M_SIZE*(150 - 24), osx + 1, osy, TFT_RED);

    // Slow needle down slightly as it approaches new postion
    if (abs(old_analog - value) < 10) ms_delay += ms_delay / 5;

    // Wait before next update
    delay(ms_delay);
  }
}
