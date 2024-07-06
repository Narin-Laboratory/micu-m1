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
    return DeviceTelemetry(
      heap: data["heap"],
      rssi: data["rssi"],
      uptime: data["uptime"],
      dt: data["dt"],
      dts: data["dts"],
    );
  }
}
