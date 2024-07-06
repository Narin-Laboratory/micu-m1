import 'dart:io';

// A class to represent a single mDNS device
class MdnsDevice {
  final String name;
  final InternetAddress address;
  final int port;
  final Map<String, String> attributes;

  MdnsDevice({
    required this.name,
    required this.address,
    required this.port,
    this.attributes = const {},
  });

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is MdnsDevice &&
          runtimeType == other.runtimeType &&
          name == other.name &&
          address == other.address;

  @override
  int get hashCode => name.hashCode ^ address.hashCode;
}
