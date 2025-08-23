import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:flutter_native_splash/flutter_native_splash.dart';
import 'package:mobile_app/utils/utils.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  BluetoothDevice? _device;

  List<FlSpot> tempData = [];
  List<FlSpot> humData = [];
  List<FlSpot> motorData = [];
  double timeX = 0;

  bool ledState = false;

  @override
  void initState() {
    super.initState();
    FlutterNativeSplash.remove();
    scanDevices();
  }

  void scanDevices() async {
    FlutterBluePlus.startScan(timeout: const Duration(seconds: 4));
    FlutterBluePlus.scanResults.listen((results) async {
      for (ScanResult r in results) {
        // ignore: deprecated_member_use
        if (r.device.name.isNotEmpty) {
          // ignore: deprecated_member_use
          if (r.device.name.contains("HC")) {
            await FlutterBluePlus.stopScan();
            setState(() => _device = r.device);
            await _device!.connect();
            discoverServices();
          }
        }
      }
    });
  }

  void discoverServices() async {
    if (_device == null) return;
    List<BluetoothService> services = await _device!.discoverServices();
    for (var s in services) {
      for (var c in s.characteristics) {
        if (c.properties.notify || c.properties.read) {
          await c.setNotifyValue(true);
          // ignore: deprecated_member_use
          c.value.listen((value) {
            try {
              String str = utf8.decode(value);
              var data = jsonDecode(str);
              setState(() {
                timeX += 1;
                tempData.add(
                  FlSpot(timeX, (data["temperature"] ?? 0).toDouble()),
                );
                humData.add(FlSpot(timeX, (data["humidity"] ?? 0).toDouble()));
                motorData.add(
                  FlSpot(timeX, (data["motorSpeed"] ?? 0).toDouble()),
                );
                if (tempData.length > 20) {
                  tempData.removeAt(0);
                  humData.removeAt(0);
                  motorData.removeAt(0);
                }
              });
            } catch (e) {
              logger("Parse error: $e");
            }
          });
        }
      }
    }
  }

  Widget buildChart(List<FlSpot> data, String label, Color color) {
    return SizedBox(
      height: 200,
      child: LineChart(
        LineChartData(
          titlesData: FlTitlesData(show: true),
          gridData: FlGridData(show: true),
          borderData: FlBorderData(show: true),
          lineBarsData: [
            LineChartBarData(
              spots: data,
              isCurved: true,
              color: color,
              dotData: FlDotData(show: false),
            ),
          ],
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("Arduino Bluetooth Monitor")),
      body: _device == null
          ? const Center(child: Text("Scanning for device..."))
          : SingleChildScrollView(
              child: Column(
                children: [
                  buildChart(tempData, "Temperature", Colors.red),
                  buildChart(humData, "Humidity", Colors.blue),
                  buildChart(motorData, "Motor Speed", Colors.green),
                  const SizedBox(height: 20),
                ],
              ),
            ),
    );
  }
}
