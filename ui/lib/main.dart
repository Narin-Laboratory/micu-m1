import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:micu/features/main/data/datasources/websocket_data_provider.dart';
import 'package:micu/features/main/presentation/bloc/auth_bloc.dart';
import 'package:micu/features/main/presentation/bloc/websocket_bloc.dart';
import 'package:micu/features/main/presentation/screens/home_screen.dart';
// ignore: depend_on_referenced_packages
import 'package:provider/provider.dart';

void main() {
  //Bloc.observer = AppBlocObserver();
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return MultiBlocProvider(
      providers: [
        BlocProvider(
          create: (context) => WebSocketBloc(),
          lazy: true,
        ),
        ChangeNotifierProvider(create: (context) => WebSocketService(context)),
        BlocProvider(
          create: (context) => AuthBloc(context),
          lazy: true,
        ),
      ],
      child: MaterialApp(
        title: 'MICU Smart Grow',
        theme: ThemeData.dark(
          //colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
          useMaterial3: true,
        ),
        home: HomeScreen(),
      ),
    );
  }
}
