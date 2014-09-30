#include <Time.h>
#include <SD.h>

const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
byte colors[3] = {0, 0, 0};
double colorInterval[3];
int intervals[10][2][3];// ={{{255, 255, 255}, {0, 0, 0}}, {{255, 255, 255}, {0, 0, 0}}};
int intervalTimes[10][2][2];//= {{{18, 20}, {18, 21}}, {16, 0}, {17, 0}}; //{{start hour, start min}, {end hour, end min}}
int startTime;
int endTime = -1;
int currentInterval[2][3];
boolean musicMode = false;
boolean intervalMode = false;

void setup() {               
  Serial.begin(9600);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT); 
  pinMode(bluePin, OUTPUT);
  
  setTime(18, 20, 0, 1, 1, 2014);
}

void loop() {
  if (Serial.available() > 0) {
      int serialCmd = (int) Serial.read();
      
      Serial.println(serialCmd);
      
      if(serialCmd == 0) {
        setColor();
      }
      else if (serialCmd == 1) {
        intervalOn();
      }
      else if (serialCmd == 2) {
        intervalOff();
      }
      else if (serialCmd == 3) {
        musicOn();
      }
      else if (serialCmd == 4) {
        musicOff();
      }
      else if (serialCmd == 5) {
        setIntervals();
      }
      else if (serialCmd == 6) {
        setIntervalTimes();
      }
      else if (serialCmd == 7) {
        printIntervals();
      }
      else if (serialCmd == 8) {
        printIntervalTimes();
      }
      else if (serialCmd == 9) {
        setTime();
      }
      else if (serialCmd == 10) {
        printTime();
      }
      else if (serialCmd == 11) {
        clearIntervals();
      }
      else if(serialCmd == 255) {
        handshake();
      }
  }
  
  if(intervalMode) {
    runInterval();
  }
  
  delay(1);
}

/* COMMANDS */
void handshake() {
  Serial.print(255);
}
void printTime() {
  Serial.println(String(hour()) + " " + String(minute()));
}
void setTime() {
  long timeOut = millis() + 100;
  while(Serial.available() < 2 && timeOut > millis()); 
  if(Serial.available() <= 2){
    setTime((int)Serial.read(), (int)Serial.read(), 0, 1, 1, 2014);
  }
}
void printIntervals(){
  String s;
  for(int i = 0; i < 10; i++){
    for(int j = 0; j < 2; j++){ 
      for(int k = 0; k < 3; k++){
        s += String(intervals[i][j][k]);
      } 
    }
  }
  Serial.println(s);
}

void printIntervalTimes(){
  String s;
  for(int i = 0; i < 10; i++){
    for(int j = 0; j < 2; j++){ 
      for(int k = 0; k < 2; k++){
        s += String(intervalTimes[i][j][k]);
      } 
    }
  }
  Serial.println(s);
}

void setColor() {
  long timeOut = millis() + 1000;
  while(Serial.available() < 3 && timeOut > millis()); 
  if(Serial.available() <= 3){
    colors[0] = (int) Serial.read();
    colors[1] = (int) Serial.read();
    colors[2] = (int) Serial.read();
    writeColor();
  }
}

void intervalOn() {
  intervalMode = true;
  endTime = -1;
}

void intervalOff() {
  intervalMode = false;
  endTime = -1;
}

void musicOn() {
  musicMode = true;
}

void musicOff() {
  musicMode = false;
}

void setIntervals() {
  long timeOut = millis() + 1000;
  while(Serial.available() < 60 && timeOut > millis()); 
  if(Serial.available() <= 60){
    //Read intervals
    for(int i = 0; i < 10; i++){
      for(int j = 0; j < 2; j++){ 
        for(int k = 0; k < 3; k++){
          intervals[i][j][k] = (int) Serial.read();
        } 
      }
    }
  }
}

void clearIntervals() {
  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 2; j++) {
      for(int k = 0; k < 3; k++) {
        intervals[i][j][k] = 0;
      }
    }
  }
  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 2; j++) {
      for(int k = 0; k < 2; k++) {
        intervalTimes[i][j][k] = 0;
      }
    }
  }
}

void setIntervalTimes() {
  long timeOut = millis() + 1000;
  while(Serial.available() < 40 && timeOut > millis()); 
  if(Serial.available() <= 40){
    //Read interval times
    for(int i = 0; i < 10; i++){
      for(int j = 0; j < 2; j++){ 
        for(int k = 0; k < 2; k++){
          intervalTimes[i][j][k] = (int) Serial.read();
        } 
      }
    }
    endTime = -1;
  }
}

/* INTERVAL MODE */

void runInterval() {
  int minNow = (hour() * 60) + minute();
  if(endTime == -1 || (endTime < minNow || startTime > minNow)){
    Serial.print("Searching");
    for(int i = 0; i < 10; i++){
      int minStart = (intervalTimes[i][0][0] * 60) + intervalTimes[i][0][1];
      int minEnd = (intervalTimes[i][1][0] * 60) + intervalTimes[i][1][1];
      
      if(minNow >= minStart && minNow <= minEnd){
        Serial.print("Found"); 
        startTime = minStart;
        endTime = minEnd;
        memcpy(currentInterval, intervals[i], sizeof(currentInterval));
        getColorInterval();
        break;
      }
    }
  } else {
    getColor();
  
    writeColor();
  } 
}
double getColorInterval() {
  double timespan = (endTime - startTime) * 60;
  //Serial.println("Timespan: " + String((int)timespan));
  colorInterval[0] = (currentInterval[0][0] - currentInterval[1][0]) / timespan;
  colorInterval[1] = (currentInterval[0][1] - currentInterval[1][1]) / timespan;
  colorInterval[2] = (currentInterval[0][2] - currentInterval[1][2]) / timespan;
  
  //Serial.println("Calculated color interval: " + String(int(colorInterval[0]*100)) + " " + String(int(colorInterval[1]*100)) + " " + String(int(colorInterval[2])*100));
}

void getColor(){
  int timespan = ((hour() * 3600) + (minute()*60) + second()) - (startTime * 60);
  
  if(((startTime * 60) + timespan) <= (endTime * 60)){
    colors[0] = (int)abs(currentInterval[0][0] - (timespan * colorInterval[0]));
    colors[1] = (int)abs(currentInterval[0][1] - (timespan * colorInterval[1]));
    colors[2] = (int)abs(currentInterval[0][2] - (timespan * colorInterval[2]));
    //Serial.println("Color:" + String(colors[0]) + " " + String(colors[1]) + " " + String(colors[2]));
  } 
}

void writeColor() {
  analogWrite(redPin, colors[0]);
  analogWrite(greenPin, colors[1]);
  analogWrite(bluePin, colors[2]);
}
