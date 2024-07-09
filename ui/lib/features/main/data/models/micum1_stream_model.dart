// A class to represent a single mDNS device
class MicuM1Stream {
  int remainingTimeToHarvest;

  MicuM1Stream({
    this.remainingTimeToHarvest = 0,
  });

  // Factory constructor to create from a JSON map
  factory MicuM1Stream.fromJson(Map<String, dynamic> data) {
    try {
      return MicuM1Stream(
        remainingTimeToHarvest: data["remainingTimeToHarvest"] is int
            ? data["remainingTimeToHarvest"]
            : (data["remainingTimeToHarvest"] as int),
      );
    } catch (error) {
      //print("MicuM1Stream error: ${error.toString()}");
      return MicuM1Stream(); // Return default on error
    }
  }
}
