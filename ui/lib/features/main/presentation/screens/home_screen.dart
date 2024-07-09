import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:micu/features/main/data/models/mdns_device_model.dart';
import 'package:micu/features/main/presentation/bloc/auth_bloc.dart';
import 'package:micu/features/main/presentation/bloc/websocket_bloc.dart';
import 'package:micu/features/main/presentation/screens/m1_screen.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  String _textLog = '';
  String _selectedDevice = '';

  void _doConnect() {
    context.read<AuthBloc>().add(AuthLocalOnSignOut());
    context
        .read<AuthBloc>()
        .add(AuthLocalOnRequested(ip: _selectedDevice, webApiKey: "smartgrow"));
  }

  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    /*setState(() {
      webApiKeyController.text = "default";
      deviceIpAddressController.text = "192.168.99.180";
    });*/

    return BlocConsumer<WebSocketBloc, WebSocketState>(
      listener: (context, state) {
        if (state is WebSocketLocalAuthReceived) {
          context
              .read<AuthBloc>()
              .add(AuthLocalOnWebSocketLocalAuthReceived(model: state.model));
        }
        if (state is WebSocketError) {
          setState(() {
            _textLog = state.error;
          });
          context.read<AuthBloc>().add(AuthLocalOnError(error: state.error));
        }
        if (state is WebSocketLocalAuthError) {
          setState(() {
            _textLog = state.message!['status']!['msg'];
          });
          context
              .read<AuthBloc>()
              .add(AuthLocalOnError(error: state.message!['status']!['msg']));
        }
      },
      builder: (context, state) {
        return Scaffold(
            body: BlocConsumer<AuthBloc, AuthState>(
          listener: (context, state) {
            if (state is AuthLocalError) {
              setState(() {
                _textLog = state.error;
              });
            }

            if (state is AuthLocalOnProcess) {
              setState(() {
                _textLog = state.message;
              });
            }

            if (state is AuthLocalSuccess) {
              setState(() {
                _textLog = "Connected! Redirecting to dashboard...";
              });

              final model = state.model?['status']?['model'] ?? -1;

              if (model == 'MICUM1') {
                Navigator.pushAndRemoveUntil(
                  context,
                  MaterialPageRoute(builder: (context) => const M1Screen()),
                  (route) => false,
                );
              }
            }
          },
          builder: (context, state) {
            return Scaffold(
                // Assuming you want to position it within a standard screen layout
                body: LayoutBuilder(builder: (context, constraints) {
              double maxWidth =
                  constraints.maxWidth > 800 ? 800 : constraints.maxWidth;
              return Center(
                // Center the content within the Scaffold's body
                child: ConstrainedBox(
                  constraints: BoxConstraints(maxWidth: maxWidth),
                  child: Padding(
                    padding: const EdgeInsets.all(20.0),
                    child: SingleChildScrollView(
                      child: Column(
                        mainAxisAlignment:
                            MainAxisAlignment.center, // Center vertically
                        children: [
                          SizedBox(
                            width: 90.0, // Set your desired width
                            height: 90.0, // Set your desired height
                            child: Image.asset('assets/images/logo.png'),
                          ),
                          const SizedBox(height: 10),
                          const Text("MICU Smart Grow",
                              style: TextStyle(
                                fontWeight: FontWeight.w100,
                                fontSize: 22,
                                color: Colors.green,
                              )),
                          const SizedBox(height: 15),
                          const SizedBox(height: 25),
                          Row(
                            children: [
                              Expanded(
                                child: ElevatedButton.icon(
                                    onPressed: kIsWeb
                                        ? null
                                        : () async {
                                            final selectedDevice =
                                                await showDialog<MdnsDevice>(
                                              context: context,
                                              builder: (context) =>
                                                  const SelectionPopup(),
                                            );

                                            if (selectedDevice != null) {
                                              // Do something with the selectedItem
                                              _selectedDevice = selectedDevice
                                                  .address.address;
                                              _doConnect();
                                            }
                                          },
                                    icon: const Icon(Icons.search),
                                    label: const Text("Pindai")),
                              ),
                              const SizedBox(
                                  width: 10), // Add spacing between buttons
                            ],
                          ),
                          const SizedBox(height: 25),
                          if (_textLog
                              .isNotEmpty) // Check if there's a log message
                            SizedBox(
                              height: 50,
                              child: Center(
                                // Add the Center widget here
                                child: Row(
                                  children: [
                                    if (state is AuthLocalOnProcess)
                                      const SizedBox(
                                        width: 20,
                                        height: 20,
                                        child: CircularProgressIndicator(),
                                      ),
                                    const SizedBox(width: 10),
                                    Expanded(child: Text(_textLog)),
                                  ],
                                ),
                              ),
                            ),
                        ],
                      ),
                    ),
                  ),
                ),
              );
            }));
          },
        ));
      },
    );
  }
}

class SelectionPopup extends StatefulWidget {
  const SelectionPopup({super.key});

  @override
  SelectionPopupState createState() => SelectionPopupState();
}

class SelectionPopupState extends State<SelectionPopup> {
  @override
  void initState() {
    super.initState();
    context.read<AuthBloc>().add(AuthDeviceScannerOnRequested());
  }

  @override
  Widget build(BuildContext context) {
    return BlocBuilder<AuthBloc, AuthState>(
      builder: (context, state) {
        if (state is AuthDeviceScannerStarted) {
          return const AlertDialog(
              title: Text("Pemindai Perangkat"),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [Text("Memulai mesin pemindai...")],
              ));
        } else if (state is AuthDeviceScannerOnProcess) {
          return AlertDialog(
              title: const Text("Mencari perangkat MICU"),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Text(
                      "Ditemukan: ${state.mdnsDevice.name} di ${state.mdnsDevice.address.address}")
                ],
              ));
        } else if (state is AuthDeviceScannerFinished) {
          return AlertDialog(
            title: const Text("Pilihan MICU"),
            content: Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                for (final device in state.mdnsDeviceList)
                  ListTile(
                    title: Text("${device.name} - ${device.address.address}"),
                    onTap: () {
                      Navigator.pop(context, device);
                    },
                  ),
              ],
            ),
            actions: [
              TextButton(
                onPressed: () => Navigator.pop(context),
                child: const Text('Batal'),
              ),
              ElevatedButton(
                onPressed: () {
                  context.read<AuthBloc>().add(AuthDeviceScannerOnRequested());
                },
                child: const Text('Pindai Lagi'),
              ),
            ],
          );
        } else {
          return const AlertDialog(
              title: Text("Pilih MICU"),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [Text("Oh tidak, ada galat yang tak diketahui!")],
              ));
        }
      },
    );
  }
}
