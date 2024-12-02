// A class to represent a single mDNS device
class MicuM1State {
  bool growLightState;
  bool pumpState;
  int mode;
  String sowingDatetime;
  int selectedProfile;
  String label;

  MicuM1State({
    this.growLightState = false,
    this.pumpState = false,
    this.mode = 0,
    this.sowingDatetime = "",
    this.selectedProfile = 1,
    this.label = "",
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
        sowingDatetime: data["sowingDatetime"] is String
            ? data["sowingDatetime"]
            : (data["sowingDatetime"] as String),
        selectedProfile: data["selectedProfile"] is int
            ? data["selectedProfile"]
            : (data["selectedProfile"] as int),
        label:
            data["label"] is String ? data["label"] : (data["label"] as String),
      );
    } catch (error) {
      //print("MicuM1State error: ${error.toString()}");
      return MicuM1State(); // Return default on error
    }
  }
}
