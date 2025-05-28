#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>
#include <deque>
#include <algorithm>

#define LED_PIN 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

const char* nodeID = "node-02";
bool isRepeater   = true;
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

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
  else if(cmd=="blue")  leds[0]=CRGB::Blue;
  else if(cmd=="orange") leds[0]=CRGB::Orange;
  else if(cmd=="purple") leds[0]=CRGB::Purple;
  else if(cmd=="pink")   leds[0]=CRGB::Pink;
  else if(cmd=="gray")   leds[0]=CRGB::Gray;
  else if(cmd=="brown")  leds[0]=CRGB::Brown;
  else if(cmd=="lime")   leds[0]=CRGB::Lime;
  else if(cmd=="teal")   leds[0]=CRGB::Teal;
  else if(cmd=="aqua")   leds[0]=CRGB::Aqua;
  else if(cmd=="navy")   leds[0]=CRGB::Navy;
  else if(cmd=="fuchsia") leds[0]=CRGB::Fuchsia;
  else if(cmd=="silver") leds[0]=CRGB::Silver;
  else if(cmd=="white") leds[0]=CRGB::White;
  else if(cmd=="yellow") leds[0]=CRGB::Yellow;
  else if(cmd=="cyan")   leds[0]=CRGB::Cyan;
  else if(cmd=="magenta") leds[0]=CRGB::Magenta;
  else if(cmd=="off")    leds[0]=CRGB::Black;
  else if(cmd=="blink") {
    leds[0]=CRGB::Red; FastLED.show();
    delay(500);
    leds[0]=CRGB::Black;
  }
  else if(cmd=="flash") {
    leds[0]=CRGB::Red; FastLED.show();
    delay(100);
    leds[0]=CRGB::Black;
  }
  else if(cmd=="toggle") {
    if(leds[0].r>0 || leds[0].g>0 || leds[0].b>0) 
      leds[0]=CRGB::Black;
    else 
      leds[0]=CRGB::Red;
  }
  else leds[0]=CRGB::Black;
  FastLED.show();
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
