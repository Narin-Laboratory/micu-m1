// A class to represent a single mDNS device
class MicuM1State {
  bool growLightState;
  bool pumpState;
  int mode;
  String sowingTS;
  int selectedProfile;

  MicuM1State({
    this.growLightState = false,
    this.pumpState = false,
    this.mode = 0,
    this.sowingTS = "",
    this.selectedProfile = 1,
  });

  // Factory constructor to create from a JSON map
  factory MicuM1State.fromJson(Map<String, dynamic> data) {
    try {
      return MicuM1State(
        growLightState: data["growLightState"] is bool
            ? data["growLightState"]
            : (data["growLightState"] as bool),
        pumpState: data["pumpState"] is bool
            ? data["pumpState"]
            : (data["pumpState"] as bool),
        mode: data["mode"] is int ? data["mode"] : (data["mode"] as int),
        sowingTS: data["sowingTS"] is String
            ? data["sowingTS"]
            : (data["sowingTS"] as String),
        selectedProfile: data["selectedProfile"] is int
            ? data["selectedProfile"]
            : (data["selectedProfile"] as int),
      );
    } catch (error) {
      print("MicuM1State error: ${error.toString()}");
      return MicuM1State(); // Return default on error
    }
  }
}
