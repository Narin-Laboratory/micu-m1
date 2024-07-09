import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:micu/features/main/presentation/bloc/auth_bloc.dart';

class AppBarWidget extends StatefulWidget implements PreferredSizeWidget {
  final GlobalKey<ScaffoldState>
      scaffoldKey; // Add a property for the scaffold key
  final String title; // Add a property for the title
  final String uptime;

  const AppBarWidget(
      {super.key,
      required this.scaffoldKey,
      required this.title,
      required this.uptime});

  @override
  State<AppBarWidget> createState() => _AppBarWidgetState();

  @override
  Size get preferredSize => const Size.fromHeight(kToolbarHeight);
}

class _AppBarWidgetState extends State<AppBarWidget> {
  @override
  Widget build(BuildContext context) {
    return AppBar(
      title: Text(
        widget.title,
        style: const TextStyle(fontWeight: FontWeight.bold),
      ),
      centerTitle: true,
      leading: IconButton(
        // Add leading button
        icon: const Icon(Icons.menu),
        onPressed: () => widget.scaffoldKey.currentState!.openDrawer(),
      ),
      actions: [
        Text(
          widget.uptime,
          style: const TextStyle(fontSize: 8),
        ),
        IconButton(
          onPressed: () {
            context.read<AuthBloc>().add(AuthLocalOnSignOut());
          },
          icon: const Icon(Icons.logout),
        ),
      ],
    );
  }
}
