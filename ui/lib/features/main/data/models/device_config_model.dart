// A class to represent a single mDNS device
class DeviceConfig {
  final String name;
  final String model;
  final String group;
  final String ap;
  final int gmtOff;
  final int fP;
  final int fIoT;
  final String hname;

  DeviceConfig({
    this.name = "",
    this.model = "",
    this.group = "",
    this.ap = "",
    this.gmtOff = 0,
    this.fP = 0,
    this.fIoT = 1,
    this.hname = "",
  });

  // Factory constructor to create from a JSON map
  factory DeviceConfig.fromJson(Map<String, dynamic> data) {
    return DeviceConfig(
      name: data["name"],
      model: data["model"],
      group: data["group"],
      ap: data["ap"],
      gmtOff: data["gmtOff"],
      fP: data["fP"],
      fIoT: data["fIoT"],
      hname: data["hname"],
    );
  }
}
