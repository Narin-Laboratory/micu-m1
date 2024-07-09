// ignore: depend_on_referenced_packages

import 'package:bloc/bloc.dart';
// ignore: depend_on_referenced_packages
import 'package:meta/meta.dart';
import 'package:micu/features/main/data/models/device_config_model.dart';
import 'package:micu/features/main/data/models/device_telemetry_model.dart';
import 'package:micu/features/main/data/models/device_attributes_model.dart';
import 'package:micu/features/main/data/models/micum1_state_model.dart';

part 'websocket_event.dart';
part 'websocket_state.dart';

class WebSocketBloc extends Bloc<WebSocketEvent, WebSocketState> {
  WebSocketBloc() : super(WebSocketInitial()) {
    on<WebSocketOnMessage>(_onWebSocketOnMessage);
    on<WebSocketOnError>(_onWebSocketOnError);
    on<WebSocketOnDisconnect>(_onWebSocketOnDisconnect);
  }

  void _onWebSocketOnDisconnect(
      WebSocketOnDisconnect event, Emitter<WebSocketState> emit) {
    emit(WebSocketDisconnect());
  }

  void _onWebSocketOnError(
      WebSocketOnError event, Emitter<WebSocketState> emit) {
    emit(WebSocketError(error: event.error));
  }

  void _onWebSocketOnMessage(
      WebSocketOnMessage event, Emitter<WebSocketState> emit) {
    try {
      if (event.message?['devTel'] == null) {
        print(event.message);
      }
      //print(event.message);
      // Handle message if not null
      if (event.message != null) {
        if (event.message?['status'] != null) {
          final code = event.message?['status']?['code'] ?? -1;
          if (code == 200) {
            emit(WebSocketLocalAuthReceived(model: event.message));
          } else if (code != 200) {
            emit(WebSocketLocalAuthError(message: event.message));
          }
        } else if (event.message?['attr'] != null) {
          final DeviceAttributes attributes =
              DeviceAttributes.fromJson(event.message['attr']);
          emit(WebSocketMessageReadyDeviceAttributes(attributes: attributes));
        } else if (event.message?['devTel'] != null) {
          final DeviceTelemetry telemetry =
              DeviceTelemetry.fromJson(event.message['devTel']);
          emit(WebSocketMessageReadyDeviceTelemetry(telemetry: telemetry));
        } else if (event.message?['cfg'] != null) {
          final DeviceConfig config =
              DeviceConfig.fromJson(event.message['cfg']);
          emit(WebSocketMessageReadyDeviceConfig(config: config));
        } else if (event.message?['micuM1State'] != null) {
          final MicuM1State micuM1State =
              MicuM1State.fromJson(event.message['micuM1State']);
          emit(WebSocketMessageReadyMicuM1State(micuM1State: micuM1State));
        }
      }

      //emit(WebSocketMessageReady(message: event.message));
    } catch (e) {
      //print("__func__ ${e}");
    }
  }
}
