import 'package:flutter/material.dart';
import 'package:shrimp_farming/providers/shrimp_provider.dart';
import 'package:provider/provider.dart';
import 'package:shrimp_farming/widgets/info_card.dart';

class HomeScreen extends StatelessWidget {
  const HomeScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final provider = context.watch<ShrimpProvider>();
    return Scaffold(
      body: Container(
        decoration: BoxDecoration(
          gradient:  LinearGradient(
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
              colors: [Color(0xFFFFD1D1), Colors.white])
        ),
        child:
            Column(
              children: [
                SizedBox(height: 100,),
                Text("Fish Aura Farming",style: TextStyle(color: Colors.black,fontSize: 30,fontWeight: FontWeight.bold),),
                Expanded(
                  child:
                  GridView.builder(
                    padding: EdgeInsets.all(10),
                      gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
                          crossAxisCount: 2,
                          mainAxisSpacing: 10,
                          crossAxisSpacing: 10,
                        childAspectRatio: 0.65,
                  ),
                      itemCount: provider.sensors.length,
                      itemBuilder: (context, index)
                      => InfoCard(sensor: provider.sensors[index]),
                  ),
                ),
              ],
            )
      ),
    );
  }
}
