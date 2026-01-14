#include <config.h>

void setup(){
  Serial.begin(115200);

  preferences.begin("device_config", false);  // Open Preferences (NVS)
  static String node_id;
  static bool is_repeater;

  #if CHANGE_DEVICE_CONFIG
    node_id = String(NODE_ID);
    is_repeater = IS_REPEATER;

    preferences.putString("node_id", node_id);
    Serial.println("Device ID updated in Preferences: " + node_id);

    preferences.putBool("is_repeater", IS_REPEATER);
    Serial.println("Repeater status updated in Preferences: " + String(IS_REPEATER));
  #else
    node_id = preferences.getString("node_id", "UNKNOWN");
    is_repeater = preferences.getBool("is_repeater", false);

    Serial.println("Restored Device ID from Preferences: " + node_id);
    Serial.println("Repeater status restored from Preferences: " + String(is_repeater));
  #endif

  strncpy(nodeID, node_id.c_str(), sizeof(nodeID));  // Safely copy to global buffer
  nodeID[sizeof(nodeID) - 1] = '\0'; // Ensure null-termination
  isRepeater = is_repeater;
  preferences.end();

  // Serial.printf("Node ID: %s, repeater=%d\n", nodeID, isRepeater);
}

void loop() {
  
}