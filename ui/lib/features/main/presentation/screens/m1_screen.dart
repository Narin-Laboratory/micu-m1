import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:micu/features/main/data/models/device_attributes_model.dart';
import 'package:micu/features/main/data/models/device_config_model.dart';
import 'package:micu/features/main/data/models/device_telemetry_model.dart';
import 'package:micu/features/main/data/models/micum1_state_model.dart';
import 'package:micu/features/main/data/models/micum1_stream_model.dart';
import 'package:micu/features/main/presentation/bloc/auth_bloc.dart';
import 'package:micu/features/main/presentation/bloc/websocket_bloc.dart';
import 'package:micu/features/main/presentation/screens/home_screen.dart';
import 'package:micu/features/main/presentation/widgets/appbar_widget.dart';

class M1Screen extends StatefulWidget {
  const M1Screen({super.key});

  @override
  State<M1Screen> createState() => _M1ScreenState();
}

class _M1ScreenState extends State<M1Screen> {
  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey(); // Add a key

  DeviceTelemetry devTel = DeviceTelemetry();
  DeviceAttributes attr = DeviceAttributes();
  DeviceConfig cfg = DeviceConfig();
  MicuM1State micuM1State = MicuM1State();
  MicuM1Stream micuM1Stream = MicuM1Stream();
  // Plant Profiles (You should probably load this from a database or file)
  final Map<int, List<dynamic>> profiles = {
    1: [1, "Arugula", 1209600000],
    2: [2, "Broccoli", 864000000],
    3: [3, "Cabbage", 864000000],
    4: [4, "Cilantro", 1209600000],
    5: [5, "Kale", 1036800000],
    6: [6, "Kohlrabi", 604800000],
    7: [7, "Mustard", 691200000],
    8: [8, "Pea Shoots", 1209600000],
    9: [9, "Radish", 604800000],
    10: [10, "Sunflower", 864000000],
    11: [11, "Amaranth", 691200000],
    12: [12, "Beet", 1555200000],
    13: [13, "Buckwheat", 604800000],
    14: [14, "Chard", 1036800000],
    15: [15, "Chia", 691200000],
    16: [16, "Fenugreek", 691200000],
    17: [17, "Lettuce", 864000000],
    18: [18, "Mizuna", 864000000],
    19: [19, "Pak Choi", 864000000],
    20: [20, "Watercress", 691200000],
  };

  DateTime _sowingDatetime = DateTime.now(); // Default to current time
  int _dayToHarvest = 0;
  int _hourToHarvest = 0;
  int _minuteToHarvest = 0;
  int _secondToHarvest = 0;

  @override
  void initState() {
    super.initState(); // Always call super.initState() first!

    Map<String, dynamic> payload = {'cmd': 'syncClientAttr'};
    context
        .read<AuthBloc>()
        .add(AuthLocalOnSendCommandReceived(payload: payload));
  }

  void _setSowingTS(String datetime) {
    Map<String, dynamic> payload = {
      'cmd': 'setSowingDatetime',
      'datetime': datetime,
    };
    context
        .read<AuthBloc>()
        .add(AuthLocalOnSendCommandReceived(payload: payload));
  }

  @override
  Widget build(BuildContext context) {
    return BlocListener<WebSocketBloc, WebSocketState>(
      listener: (context, state) {
        if (state is WebSocketMessageReadyDeviceAttributes) {
          setState(() {
            attr = state.attributes;
          });
        } else if (state is WebSocketMessageReadyDeviceTelemetry) {
          setState(() {
            devTel = state.telemetry;
          });
        } else if (state is WebSocketMessageReadyDeviceConfig) {
          setState(() {
            cfg = state.config;
          });
        } else if (state is WebSocketMessageReadyMicuM1State) {
          setState(() {
            micuM1State = state.micuM1State;
            _sowingDatetime = DateTime.parse(micuM1State.sowingDatetime);
          });
        } else if (state is WebSocketMessageReadyMicuM1Stream) {
          setState(() {
            micuM1Stream = state.micuM1Stream;
            Duration duration =
                Duration(seconds: state.micuM1Stream.remainingTimeToHarvest);
            _dayToHarvest = duration.inDays;
            _hourToHarvest = duration.inHours;
            _minuteToHarvest = duration.inMinutes;
            _secondToHarvest = duration.inSeconds;
          });
        } else if (state is WebSocketDisconnect) {
          Navigator.pushAndRemoveUntil(
            context,
            MaterialPageRoute(builder: (context) => const HomeScreen()),
            (route) => false,
          );
        }
      },
      child: Scaffold(
        key: _scaffoldKey, // Assign the key
        appBar: AppBarWidget(
          scaffoldKey: _scaffoldKey,
          title: micuM1State.label,
          uptime: " UT ${devTel.uptime.toString()}",
        ),
        drawer: Drawer(
          child: ListView(
            padding: EdgeInsets.zero,
            children: [
              const DrawerHeader(
                decoration: BoxDecoration(
                  color: Colors.blue,
                ),
                child: Text('MICU Menu'),
              ),
              ListTile(
                title: const Text('Dashboard'),
                onTap: () {
                  // Handle Dashboard navigation (likely do nothing here)
                  Navigator.pop(context);
                },
              ),
              ListTile(
                title: const Text('Settings'),
                onTap: () {
                  // Navigate to Settings screen
                  Navigator.pop(context);
                },
              ),
            ],
          ),
        ),
        body: Center(
          // Wrap with Center widget
          child: SingleChildScrollView(
            padding: const EdgeInsets.all(16.0),
            child: Column(
              mainAxisSize:
                  MainAxisSize.min, // Added to allow content to shrink
              crossAxisAlignment: CrossAxisAlignment.center, // Center the text
              children: [
                Row(
                    mainAxisSize:
                        MainAxisSize.min, // Added to allow row to shrink
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      // Plant Type Dropdown
                      const Text("Jenis Microgreens:"),
                      const SizedBox(width: 8),
                      DropdownButtonHideUnderline(
                        child: DropdownButton<int>(
                          value: micuM1State.selectedProfile,
                          onChanged: (newValue) {
                            setState(() {
                              micuM1State.selectedProfile = newValue!.toInt();
                            });
                            Map<String, dynamic> payload = {
                              'cmd': 'setProfile',
                              'id': newValue!.toInt(),
                            };
                            context.read<AuthBloc>().add(
                                AuthLocalOnSendCommandReceived(
                                    payload: payload));
                          },
                          items: profiles.entries
                              .map((entry) => DropdownMenuItem<int>(
                                    value: entry.value[0] as int,
                                    child: Text(entry.value[1] as String),
                                  ))
                              .toList(),
                        ),
                      ),
                    ]),
                Row(
                    mainAxisSize:
                        MainAxisSize.min, // Added to allow row to shrink
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      // Sowing Time Date & Time Picker
                      const SizedBox(width: 16),
                      const Text("Waktu Semai:"),
                      const SizedBox(width: 8),
                      ElevatedButton(
                        onPressed: () async {
                          final DateTime? picked = await showDatePicker(
                            context: context,
                            initialDate: _sowingDatetime,
                            firstDate: DateTime(2000),
                            lastDate: DateTime(2101),
                          );
                          if (picked != null && picked != _sowingDatetime) {
                            final TimeOfDay? pickedTime = await showTimePicker(
                              context: context,
                              initialTime:
                                  TimeOfDay.fromDateTime(_sowingDatetime),
                            );
                            if (pickedTime != null) {
                              setState(() {
                                _sowingDatetime = DateTime(
                                  picked.year,
                                  picked.month,
                                  picked.day,
                                  pickedTime.hour,
                                  pickedTime.minute,
                                );
                                String formattedDateTime =
                                    "${picked.year}-${picked.month.toString().padLeft(2, '0')}-${picked.day.toString().padLeft(2, '0')} ${pickedTime.hour.toString().padLeft(2, '0')}:${pickedTime.minute.toString().padLeft(2, '0')}:00";
                                _setSowingTS(formattedDateTime);
                              });
                            }
                          }
                        },
                        child: Text(_sowingDatetime.toString()),
                      ),
                    ]),
                const SizedBox(height: 100),
                Row(
                    mainAxisSize:
                        MainAxisSize.min, // Added to allow row to shrink
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      const Text("Mode Operasi:"),
                      const SizedBox(width: 8),
                      Switch(
                        value: micuM1State.mode == 1 ? true : false,
                        onChanged: (newValue) {
                          setState(() {
                            micuM1State.mode = newValue == true ? 1 : 0;
                          });
                          Map<String, dynamic> payload = {
                            'cmd': 'setMode',
                            'mode': newValue == false ? 0 : 1,
                          };
                          context.read<AuthBloc>().add(
                              AuthLocalOnSendCommandReceived(payload: payload));
                        },
                        activeTrackColor: Colors.lightGreenAccent,
                        activeColor: Colors.green,
                        inactiveTrackColor: Colors.grey,
                        inactiveThumbColor: Colors.blueGrey,
                      ),
                      const SizedBox(width: 8),
                      Text(micuM1State.mode == 1 ? 'Auto' : 'Manual'),
                    ]), // Add a select box of operation mode (Manual & Auto) here
                const SizedBox(height: 20),
                if (micuM1State.mode == 1)
                  Row(
                    mainAxisSize:
                        MainAxisSize.min, // Added to allow row to shrink
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      const Icon(
                        Icons.content_cut_outlined,
                        color: Colors.blueGrey,
                        size: 120,
                      ),
                      const SizedBox(
                          width: 16), // Add spacing between Icon and Text
                      Column(
                        mainAxisSize:
                            MainAxisSize.min, // Added to allow column to shrink
                        crossAxisAlignment: CrossAxisAlignment.center,
                        children: [
                          const Text(
                            'Siap panen dalam',
                            style: TextStyle(
                                color: Colors.greenAccent, fontSize: 18),
                          ),
                          Text(
                            '$_dayToHarvest HARI',
                            style: const TextStyle(
                                color: Colors.grey, fontSize: 48),
                          ),
                          Text(
                            '$_hourToHarvest Jam, $_minuteToHarvest Menit, $_secondToHarvest Detik',
                            style: const TextStyle(
                                color: Colors.grey, fontSize: 8),
                          ),
                        ],
                      ),
                    ],
                  ),
                if (micuM1State.mode == 0)
                  Row(
                      mainAxisSize:
                          MainAxisSize.max, // Added to allow row to shrink
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: [
                        Column(
                          mainAxisSize: MainAxisSize
                              .min, // Added to allow column to shrink
                          crossAxisAlignment: CrossAxisAlignment.center,
                          children: [
                            const Text("Lampu Penumbuh:"),
                            const SizedBox(width: 8),
                            Switch(
                              value: micuM1State.growLightState,
                              onChanged: (newValue) {
                                setState(() {
                                  micuM1State.growLightState = newValue;
                                });
                                Map<String, dynamic> payload = {
                                  'cmd': 'setGrowLight',
                                  'brightness': newValue == false ? 0 : 100,
                                };
                                context.read<AuthBloc>().add(
                                    AuthLocalOnSendCommandReceived(
                                        payload: payload));
                              },
                              activeTrackColor: Colors.lightGreenAccent,
                              activeColor: Colors.green,
                              inactiveTrackColor: Colors.grey,
                              inactiveThumbColor: Colors.blueGrey,
                            ),
                            const SizedBox(width: 8),
                            Text(micuM1State.growLightState == true
                                ? 'Nyala'
                                : 'Padam'),
                          ],
                        ),
                        Column(
                          mainAxisSize: MainAxisSize
                              .min, // Added to allow column to shrink
                          crossAxisAlignment: CrossAxisAlignment.center,
                          children: [
                            const Text("Pompa Penyiram:"),
                            const SizedBox(width: 8),
                            Switch(
                              value: micuM1State.pumpState,
                              onChanged: (newValue) {
                                setState(() {
                                  micuM1State.pumpState = newValue;
                                });
                                Map<String, dynamic> payload = {
                                  'cmd': 'setPump',
                                  'power': newValue == false ? 0 : 100,
                                };
                                context.read<AuthBloc>().add(
                                    AuthLocalOnSendCommandReceived(
                                        payload: payload));
                              },
                              activeTrackColor: Colors.lightGreenAccent,
                              activeColor: Colors.green,
                              inactiveTrackColor: Colors.grey,
                              inactiveThumbColor: Colors.blueGrey,
                            ),
                            const SizedBox(width: 8),
                            Text(micuM1State.pumpState == true
                                ? 'Nyala'
                                : 'Padam'),
                          ],
                        ),
                      ]),
                const SizedBox(height: 100),
                Row(
                  mainAxisSize:
                      MainAxisSize.min, // Added to allow row to shrink
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    const Icon(
                      Icons.signal_wifi_statusbar_4_bar,
                      color: Colors.blueAccent,
                      size: 24,
                    ),
                    const SizedBox(
                        width: 8), // Add spacing between Icon and Text
                    Text(
                      'Terhubung ke ${cfg.ap}',
                    ),
                  ],
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
