// A class to represent a single mDNS device
class DeviceTelemetry {
  final int heap;
  final int rssi;
  final int uptime;
  final int dt;
  final String dts;

  DeviceTelemetry({
    this.heap = 0,
    this.rssi = 0,
    this.uptime = 0,
    this.dt = 0,
    this.dts = "",
  });

  // Factory constructor to create from a JSON map
  factory DeviceTelemetry.fromJson(Map<String, dynamic> data) {
    try {
      return DeviceTelemetry(
        heap: data["heap"] is int ? data["heap"] : (data["heap"] as int),
        rssi: data["rssi"] is int ? data["rssi"] : (data["rssi"] as int),
        uptime:
            data["uptime"] is int ? data["uptime"] : (data["uptime"] as int),
        dt: data["dt"] is int ? data["dt"] : (data["dt"] as int),
        dts: data["dts"] is String ? data["dts"] : (data["dts"] as String),
      );
    } catch (error) {
      //print("DeviceTelemetry error: ${error.toString()}");
      return DeviceTelemetry(); // Return default on error
    }
  }
}
