// ignore_for_file: depend_on_referenced_packages
import 'package:flutter/widgets.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:micu/features/main/data/models/mdns_device_model.dart';
import 'package:micu/features/main/data/datasources/websocket_data_provider.dart';
import 'package:multicast_dns/multicast_dns.dart';
import 'package:crypto/crypto.dart';
import 'dart:convert';
import 'dart:math';

part 'auth_event.dart';
part 'auth_state.dart';

class AuthBloc extends Bloc<AuthEvent, AuthState> {
  BuildContext? _context; // Store the BuildContext

  AuthBloc(BuildContext context) : super(AuthInitial()) {
    _context = context;
    on<AuthDeviceScannerOnRequested>(_onAuthDeviceScannerRequested);
    on<AuthLocalOnRequested>(_onAuthLocalRequested);
    on<AuthLocalOnWebSocketLocalAuthReceived>(
        _onAuthLocalWebSocketLocalAuthReceived);
    on<AuthLocalOnError>(_onAuthLocalOnError);
    on<AuthLocalOnSignOut>(_onAuthLocalSignOut);
  }

  void _onAuthLocalSignOut(AuthLocalOnSignOut event, Emitter<AuthState> emit) {
    final webSocketService = _context!.read<WebSocketService>();
    webSocketService.close();
    emit(AuthLocalSignOut());
  }

  void _onAuthLocalOnError(AuthLocalOnError event, Emitter<AuthState> emit) {
    emit(AuthLocalError(error: event.error));
  }

  void _onAuthLocalWebSocketLocalAuthReceived(
      AuthLocalOnWebSocketLocalAuthReceived event, Emitter<AuthState> emit) {
    emit(AuthLocalSuccess(model: event.model));
  }

  void _onAuthDeviceScannerRequested(
    AuthDeviceScannerOnRequested event,
    Emitter<AuthState> emit,
  ) async {
    List<MdnsDevice> deviceList = [];

    emit(AuthDeviceScannerStarted());

    const serviceType = '_http._tcp';
    final MDnsClient client = MDnsClient();
    await client.start();

    try {
      await for (final ptr in client.lookup<PtrResourceRecord>(
          ResourceRecordQuery.serverPointer(serviceType))) {
        await for (final srv in client.lookup<SrvResourceRecord>(
            ResourceRecordQuery.service(ptr.domainName))) {
          await for (final ip in client.lookup<IPAddressResourceRecord>(
              ResourceRecordQuery.addressIPv4(srv.target))) {
            MdnsDevice device =
                MdnsDevice(name: srv.name, address: ip.address, port: srv.port);
            if (!deviceList.contains(device)) {
              deviceList.add(device);
              emit(AuthDeviceScannerOnProcess(mdnsDevice: device));
            }
          }
        }
      }
    } catch (error) {
      emit(AuthDeviceScannerError(error: error));
    } finally {
      client.stop();
    }

    emit(AuthDeviceScannerFinished(mdnsDeviceList: deviceList));
  }

  void _onAuthLocalRequested(
    AuthLocalOnRequested event,
    Emitter<AuthState> emit,
  ) async {
    if (event.ip == "") {
      emit(AuthLocalError(error: "Device IP Address can not be empty!"));
      return;
    }

    if (event.webApiKey == "") {
      emit(AuthLocalError(error: "Web API Key can not be empty!"));
      return;
    }

    emit(AuthLocalStarted());
    try {
      final webSocketService = _context!.read<WebSocketService>();
      await webSocketService.connect('ws://${event.ip}/ws');

      String salt = _generateSalt();
      String auth = _generateAuth(salt, event.webApiKey);
      Map<String, dynamic> authObject = {
        'salt': salt,
        'auth': auth,
      };

      emit(AuthLocalOnProcess(message: "Connecting to device..."));
      webSocketService.send(authObject);
    } catch (error) {
      emit(AuthLocalError(error: error.toString()));
    }
  }

  String _generateSalt() {
    final Random random = Random.secure();
    var values = List<int>.generate(32, (i) => random.nextInt(256));
    return base64Url.encode(values);
  }

  String _generateAuth(String salt, String apiKey) {
    final key = utf8.encode(apiKey);
    final bytes = utf8.encode(salt);
    final hmacSha256 = Hmac(sha256, key);
    final digest = hmacSha256.convert(bytes);

    return digest.toString();
  }
}
