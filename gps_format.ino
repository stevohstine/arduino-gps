#include <SoftwareSerial.h>
SoftwareSerial GPSModule(7,6); // RX, TX
#define DEBUG true
int updates;
int failedUpdates;
int pos;
int stringplace = 0;

String timeUp;
String nmea[15];
String labels[12] = {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};

void setup() {
  Serial.begin(9600);
  GPSModule.begin(9600);

  sendData("AT+CGPSPWR=1",1000,DEBUG);
  delay(50);
  sendData("AT+CGPSOUT=32",1000,DEBUG);
  delay(100);
}

void loop() {
  Serial.flush();
  GPSModule.flush();
  while (GPSModule.available() > 0)
  {
    GPSModule.read();

  }
  if (GPSModule.find("$GPRMC,")) {
    Serial.println("found");
    String tempMsg = GPSModule.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }
    updates++;
    nmea[0] = convertTime();
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    nmea[8] = convertDate();
    for (int i = 0; i < 9; i++) {
      Serial.print(labels[i]);
      Serial.print(nmea[i]);
      Serial.println("");
    }

  }
  else {

    failedUpdates++;

  }
  stringplace = 0;
  pos = 0;
}

String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }

  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);

    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}

String sendData(String command, const int timeout, boolean debug){
  
    String response = "";    
    GPSModule.println(command); 
    long int time = millis();
    int i = 0;  
     
    while( (time+timeout) > millis()){
      while(GPSModule.available()){       
        char c = GPSModule.read();
        response+=c;
      }  
    }    
    if(debug){
      Serial.print(response);
    }    
    return response;
}

String convertTime(){
  String hr,mn,sec,cHr;
  String data = nmea[0];  

  hr = data.substring(0,2);
  int temp = hr.toInt();
  temp += 3;
  cHr = String(temp);
  mn = data.substring(2,4);
  sec =data.substring(4,6);
  return cHr+":"+mn+":"+sec;
}

String convertDate(){
  String yr;
  String mon;
  String date;
  String data = nmea[8];  

  yr = data.substring(0,2);
  mon = data.substring(2,4);
  date =data.substring(4,6);
  return date+"/"+mon+"/"+yr;
}



