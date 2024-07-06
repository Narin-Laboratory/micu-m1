part of 'auth_bloc.dart';

@immutable
sealed class AuthEvent {}

final class AuthDeviceScannerOnRequested extends AuthEvent {}

final class AuthLocalOnRequested extends AuthEvent {
  final String ip;
  final String webApiKey;

  AuthLocalOnRequested({
    required this.ip,
    required this.webApiKey,
  });
}

final class AuthLocalOnError extends AuthEvent {
  final dynamic error;

  AuthLocalOnError({this.error});
}

final class AuthLocalOnWebSocketLocalAuthReceived extends AuthEvent {
  final dynamic model;

  AuthLocalOnWebSocketLocalAuthReceived({
    required this.model,
  });
}

final class AuthLocalOnSignOut extends AuthEvent {}
