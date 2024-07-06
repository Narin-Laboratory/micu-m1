// A class to represent a single mDNS device
class DeviceAttributes {
  final String ipad;
  final String compdate;
  final String fmTitle;
  final String fmVersion;
  final String stamac;
  final String apmac;
  final int flFree;
  final int fwSize;
  final int flSize;
  final int dSize;
  final int dUsed;
  final String sdkVer;

  DeviceAttributes({
    this.ipad = "",
    this.compdate = "",
    this.fmTitle = "",
    this.fmVersion = "",
    this.stamac = "",
    this.apmac = "",
    this.flFree = 0,
    this.fwSize = 0,
    this.flSize = 0,
    this.dSize = 0,
    this.dUsed = 0,
    this.sdkVer = "",
  });

  // Factory constructor to create from a JSON map
  factory DeviceAttributes.fromJson(Map<String, dynamic> data) {
    return DeviceAttributes(
      ipad: data["ipad"],
      compdate: data["compdate"],
      fmTitle: data["fmTitle"],
      fmVersion: data["fmVersion"],
      stamac: data["stamac"],
      apmac: data["apmac"],
      flFree: data["flFree"],
      fwSize: data["fwSize"],
      flSize: data["flSize"],
      dSize: data["dSize"],
      dUsed: data["dUsed"],
      sdkVer: data["sdkVer"],
    );
  }
}
