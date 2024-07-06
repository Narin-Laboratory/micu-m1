part of 'websocket_bloc.dart';

@immutable
sealed class WebSocketState {}

final class WebSocketInitial extends WebSocketState {}

final class WebSocketLocalAuthReceived extends WebSocketState {
  final dynamic model;

  WebSocketLocalAuthReceived({required this.model});
}

final class WebSocketLocalAuthError extends WebSocketState {
  final dynamic message;

  WebSocketLocalAuthError({required this.message});
}

final class WebSocketMessageReady extends WebSocketState {
  final dynamic message;

  WebSocketMessageReady({required this.message});
}

final class WebSocketError extends WebSocketState {
  final dynamic error;

  WebSocketError({required this.error});
}

final class WebSocketMessageReadyDeviceAttributes extends WebSocketState {
  final DeviceAttributes attributes;

  WebSocketMessageReadyDeviceAttributes({required this.attributes});
}

final class WebSocketMessageReadyDeviceTelemetry extends WebSocketState {
  final DeviceTelemetry telemetry;

  WebSocketMessageReadyDeviceTelemetry({required this.telemetry});
}

final class WebSocketMessageReadyDeviceConfig extends WebSocketState {
  final DeviceConfig config;

  WebSocketMessageReadyDeviceConfig({required this.config});
}

final class WebSocketDisconnect extends WebSocketState {}
