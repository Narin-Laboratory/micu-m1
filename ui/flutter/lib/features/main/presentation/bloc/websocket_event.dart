part of 'websocket_bloc.dart';

@immutable
sealed class WebSocketEvent {}

final class WebSocketOnMessage extends WebSocketEvent {
  final dynamic message;
  WebSocketOnMessage({required this.message});
}

final class WebSocketOnError extends WebSocketEvent {
  final dynamic error;

  WebSocketOnError({required this.error});
}

final class WebSocketOnDisconnect extends WebSocketEvent {}
