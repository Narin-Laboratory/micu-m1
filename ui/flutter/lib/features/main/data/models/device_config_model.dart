// A class to represent a single mDNS device
class DeviceConfig {
  final String name;
  final String model;
  final String group;
  final String ap;
  final int gmtOff;
  final String hname;

  DeviceConfig({
    this.name = "",
    this.model = "",
    this.group = "",
    this.ap = "",
    this.gmtOff = 0,
    this.hname = "",
  });

  // Factory constructor to create from a JSON map
  factory DeviceConfig.fromJson(Map<String, dynamic> data) {
    try {
      return DeviceConfig(
        name: data["name"] is String ? data["name"] : (data["name"] as String),
        model:
            data["model"] is String ? data["model"] : (data["model"] as String),
        group:
            data["group"] is String ? data["group"] : (data["group"] as String),
        ap: data["ap"] is String ? data["ap"] : (data["ap"] as String),
        gmtOff:
            data["gmtOff"] is int ? data["gmtOff"] : (data["gmtOff"] as int),
        hname:
            data["hname"] is String ? data["hname"] : (data["hname"] as String),
      );
    } catch (error) {
      //print("DeviceConfig error: ${error.toString()}");
      return DeviceConfig(); // Return default on error
    }
  }
}
