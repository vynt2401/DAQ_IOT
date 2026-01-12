import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:shrimp_farming/home/home_screen.dart';
import 'package:shrimp_farming/providers/shrimp_provider.dart';




void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(create: (context) => ShrimpProvider(),
    child: MaterialApp(
      debugShowCheckedModeBanner: false,
      home: HomeScreen(),
    ),
    );
  }
}