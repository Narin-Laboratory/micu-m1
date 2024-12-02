import 'dart:async';
import 'dart:convert';
import 'package:flutter/widgets.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:micu/features/main/presentation/bloc/websocket_bloc.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class WebSocketService extends ChangeNotifier {
  BuildContext? _context; // Store the BuildContext

  WebSocketService(BuildContext context) {
    _context = context;
  }

  WebSocketChannel? _channel;
  bool _isConnected = false;

  bool get isConnected => _isConnected;

  String? _latestMessage;

  String? get latestMessage => _latestMessage;

  // Connect to your WebSocket server
  Future<void> connect(String url) async {
    try {
      _channel = WebSocketChannel.connect(Uri.parse(url));

      _channel!.stream.listen((message) {
        // Handle incoming messages

        _latestMessage = message;
        try {
          final data = jsonDecode(_latestMessage ?? '');
          _context
              ?.read<WebSocketBloc>()
              .add(WebSocketOnMessage(message: data));

          // Handle based on message type
          if (data['status'] && data['status']['code']) {
            if (data['status']['code'] == 200) {
              _isConnected = true;
            } else {
              _isConnected = false;
            }
          }
          // ignore: empty_catches
        } catch (e) {}

        // ... your message processing logic
        notifyListeners();
      }, onDone: () {
        _isConnected = false;
        _context?.read<WebSocketBloc>().add(WebSocketOnDisconnect());
        // Handle connection closed
      }, onError: (error) {
        // Handle errors
        _isConnected = false;
      });
    } catch (e) {
      // Connection error handling
      _isConnected = false;
      _context?.read<WebSocketBloc>().add(WebSocketOnError(
          error:
              "Failed to connect to the WebSocket server, make sure you are on the same network with the device! Error details: ${e.toString()}"));
    }
  }

  void send(dynamic data) {
    if (_channel != null) {
      if (data == "") {
        return;
      }
      //print(data);
      _channel!.sink.add(jsonEncode(data));
    }
  }

  void close() {
    if (_channel != null) {
      _isConnected = false;
      _channel!.sink.close();
    }
  }
}
