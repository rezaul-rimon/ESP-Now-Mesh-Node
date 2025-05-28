#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <assert.h>
#include <IRac.h>
#include <IRtext.h>

#define LED_PIN 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

const char* nodeID = "node-02";
bool isRepeater   = true;
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

struct Command {
  String powerOn;     // on/off_status
  String temperature; // e.g. "24"
  String mode;        // e.g. "cool"
  String fanSpeed;    // e.g. "auto"
  String protocol;    // e.g. "tcl112"
  String v_swing;     // optional
  String h_swing;     // optional
};

Command parseCommand(const String &cmdStr) {
  Command c;
  int start = 0, idx;
  int field = 0;
  while ((idx = cmdStr.indexOf('/', start)) != -1 && field < 6) {
    String part = cmdStr.substring(start, idx);
    switch (field) {
      case 0: c.powerOn     = part; break;
      case 1: c.temperature = part; break;
      case 2: c.mode        = part; break;
      case 3: c.fanSpeed    = part; break;
      case 4: c.protocol    = part; break;
      case 5: c.v_swing     = part; break;
    }
    start = idx + 1;
    field++;
  }
  // Last segment (rest of the string)
  String last = cmdStr.substring(start);
  if      (field == 0) c.powerOn     = last;
  else if (field == 1) c.temperature = last;
  else if (field == 2) c.mode        = last;
  else if (field == 3) c.fanSpeed    = last;
  else if (field == 4) c.protocol    = last;
  else if (field == 5) c.v_swing     = last;
  else                 c.h_swing     = last;
  return c;
}

// cache last handled CMD id to avoid dup exec
String lastCmdID;
// cache recent rebroadcasts to stop loops
std::deque<String> fwdCache;
const size_t MAX_FWDS=20;

bool alreadyForwarded(const String &key) {
  return std::find(fwdCache.begin(), fwdCache.end(), key) != fwdCache.end();
}
void recordForward(const String &key) {
  fwdCache.push_back(key);
  if (fwdCache.size()>MAX_FWDS) fwdCache.pop_front();
}

void rebroadcastIfNeeded(const String &msg_id, const String &type, const String &raw){
  if(!isRepeater) return;
  String key=type+":"+msg_id;
  if(alreadyForwarded(key)) return;
  // skip cmds for me
  if(type=="cmd" && raw.indexOf(nodeID)>0) return;
  // skip own ACKs
  if(type=="ack" && raw.startsWith(String(nodeID)+",")) return;

  delay(random(10,51));
  esp_now_send(broadcastAddress,(uint8_t*)raw.c_str(),raw.length());
  Serial.println("🔁 Re-broadcasted: "+raw);
  recordForward(key);
}

void onReceive(const uint8_t *mac, const uint8_t *data, int len){
  String msg((char*)data,len);
  Serial.println("\n📥 "+msg);
  int commas = std::count(msg.begin(), msg.end(), ',');
  // ACK (3 commas): node_id,cmd,ack,msg_id
  if(commas==3 && msg.indexOf(",ack,")>0){
    int i1=msg.indexOf(','), i2=msg.indexOf(',',i1+1), i3=msg.indexOf(',',i2+1);
    String node=msg.substring(0,i1),
      cmd =msg.substring(i1+1,i2),
      type=msg.substring(i2+1,i3),
      id  =msg.substring(i3+1);
    rebroadcastIfNeeded(id,type,msg);
    Serial.println("ℹ️ ACK ignored locally");
    return;
  }
  // CMD (4 commas): gw_id,node_id,cmd,cmd,msg_id
  int i1=msg.indexOf(','),
    i2=msg.indexOf(',',i1+1),
    i3=msg.indexOf(',',i2+1),
    i4=msg.indexOf(',',i3+1);

  if(i1<0||i2<0||i3<0||i4<0){
    Serial.println("❌ bad format"); return;
  }
  String gw=msg.substring(0,i1),
    node=msg.substring(i1+1,i2),
    cmd=msg.substring(i2+1,i3),
    type=msg.substring(i3+1,i4),
    id=msg.substring(i4+1);
  rebroadcastIfNeeded(id,type,msg);
  if(node!=nodeID || type!="cmd"){ 
    Serial.printf("⏭ not mine/type=%s\n",type.c_str()); 
    return;
  }
  if(id==lastCmdID){
    Serial.println("⚠️ dup cmd"); return;
  }
  lastCmdID=id;

  // execute
  Serial.println("✅ CMD: "+cmd);

  if(cmd=="red")   leds[0]=CRGB::Red;
  else if(cmd=="green") leds[0]=CRGB::Green;
  else if(cmd=="blue")  leds[0]=CRGB::Blue;
  else if(cmd=="orange") leds[0]=CRGB::Orange;
  else if(cmd=="purple") leds[0]=CRGB::Purple;
  else if(cmd=="yellow") leds[0]=CRGB::Yellow;
  else if(cmd=="white")  leds[0]=CRGB::White;
  else if(cmd=="off")    leds[0]=CRGB::Black;
  // else leds[0]=CRGB::Black;
  else{
    Serial.println("❌ unknown cmd: "+cmd);
    // leds[0]=CRGB::Black;
  }
  FastLED.show();

  
  // Parse the command
  // Example cmd: "on/24/cool/auto/tcl112/vswing/hSwing"
  Command ac = parseCommand(cmd);

  // Print out each field
  Serial.println("🔍 Parsed Command:");
  Serial.println("  Power On:    " + ac.powerOn);
  Serial.println("  Temperature: " + ac.temperature);
  Serial.println("  Mode:        " + ac.mode);
  Serial.println("  Fan Speed:   " + ac.fanSpeed);
  Serial.println("  Protocol:    " + ac.protocol);
  Serial.println("  V Swing:     " + ac.v_swing);
  Serial.println("  H Swing:     " + ac.h_swing);

  if(ac.powerOn == "on"){
    Serial.println("✅ AC turned ON");
  } else if(ac.powerOn == "off"){
    Serial.println("✅ AC turned OFF");
  } else {
    Serial.println("❌ unknown power cmd: "+ac.powerOn);
    return;
  }
  

  
  // ACK back
    // === Send ACK ===
  String ack = String(nodeID) + "," + cmd + ",ack," + id;
  Serial.println("📤 ACK: " + ack);
  esp_now_send(broadcastAddress, (uint8_t *)ack.c_str(), ack.length());

}

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); WiFi.disconnect();
  FastLED.addLeds<NEOPIXEL,LED_PIN>(leds,NUM_LEDS);
  leds[0]=CRGB::Black; FastLED.show();
  if(esp_now_init()!=ESP_OK){ Serial.println("Init FAIL"); return; }
  esp_now_peer_info_t pi={};
  memcpy(pi.peer_addr,broadcastAddress,6);
  pi.channel=0; pi.encrypt=false;
  esp_now_add_peer(&pi);
  esp_now_register_recv_cb(onReceive);
  Serial.printf("Node %s ready, repeater=%d\n", nodeID, isRepeater);
}

void loop(){
  delay(50);
}
