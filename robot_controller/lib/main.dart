import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:web_socket_channel/io.dart';

void main() {
  runApp(const RobotControllerApp());
}

class RobotControllerApp extends StatelessWidget {
  const RobotControllerApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      home: const RobotControllerHomePage(),
    );
  }
}

class RobotControllerHomePage extends StatefulWidget {
  const RobotControllerHomePage({super.key});
  @override
  State<RobotControllerHomePage> createState() =>
      _RobotControllerHomePageState();
}

class _RobotControllerHomePageState extends State<RobotControllerHomePage> {
  late IOWebSocketChannel webSocketChannel;

  int speed = 160;
  String direction = "S";
  bool isMoving = false;
  Timer? holdTimer;

  @override
  void initState() {
    super.initState();

    webSocketChannel = IOWebSocketChannel.connect("ws://192.168.20.1:81");
  }

  void sendCommand(String cmd, {int? spd}) {
    final data = {"cmd": cmd, "speed": spd ?? speed};

    webSocketChannel.sink.add(jsonEncode(data));
  }

  void startHolding(String cmd) {
    holdTimer?.cancel();

    holdTimer = Timer.periodic(const Duration(milliseconds: 100), (_) {
      sendCommand(cmd, spd: speed);
    });
  }

  void stopHolding() {
    holdTimer?.cancel();
    holdTimer = null;
    sendCommand("S", spd: 0);
  }

  Widget controlButton({
    required double size,
    required String label,
    required String cmd,
    required IconData icon,
  }) {
    return GestureDetector(
      onTapDown: (_) {
        setState(() {
          direction = cmd;
          isMoving = true;
        });

        startHolding(cmd);
      },
      onTapUp: (_) {
        setState(() {
          direction = "S";
          isMoving = false;
        });
      
        stopHolding();
      },
      onTapCancel: () {
        setState(() {
          direction = "S";
          isMoving = false;
        });

        stopHolding();
      },
      child: Container(
        width: size,
        height: size,
        decoration: BoxDecoration(
          color: Colors.blueGrey,
          borderRadius: BorderRadius.circular(size * 0.1),
        ),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(icon, color: Colors.white, size: size * 0.35),
            Text(
              label,
              style: TextStyle(color: Colors.white, fontSize: size * 0.12),
            ),
          ],
        ),
      ),
    );
  }

  @override
  void dispose() {
    webSocketChannel.sink.close();
    holdTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final size = MediaQuery.of(context).size;
    final width = size.width;
    final height = size.height;

    final buttonSize = (width < height ? width : height) * 0.22;

    return Scaffold(
      //backgroundColor: Colors.black,
      // appBar: AppBar(
      //   title: Text(
      //     "Robot Controller",
      //     style: TextStyle(color: Colors.white, fontSize: buttonSize * 0.15),
      //   ),
      //   backgroundColor: Colors.black,
      // ),
      body: Center(
        child: SingleChildScrollView(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Padding(
                padding: EdgeInsets.all(width * 0.04),
                child: Column(
                  children: [
                    Text(
                      "Speed: $speed",
                      style: TextStyle(fontSize: buttonSize * 0.15),
                    ),
                    Slider(
                      value: speed.toDouble(),
                      min: 0,
                      max: 255,
                      divisions: 255,
                      label: speed.toString(),
                      onChanged: (value) {
                        setState(() {
                          speed = value.toInt();
                        });
                        if (isMoving) {
                          sendCommand(direction, spd: speed);
                        }
                      },
                    ),
                  ],
                ),
              ),
              controlButton(
                size: buttonSize,
                label: "FORWARD",
                cmd: "F",
                icon: Icons.arrow_upward,
              ),
              SizedBox(height: buttonSize * 0.15),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  controlButton(
                    size: buttonSize,
                    label: "LEFT",
                    cmd: "L",
                    icon: Icons.arrow_back,
                  ),
                  SizedBox(width: buttonSize * 0.15),
                  controlButton(
                    size: buttonSize,
                    label: "STOP",
                    cmd: "S",
                    icon: Icons.stop,
                  ),
                  SizedBox(width: buttonSize * 0.15),
                  controlButton(
                    size: buttonSize,
                    label: "RIGHT",
                    cmd: "R",
                    icon: Icons.arrow_forward,
                  ),
                ],
              ),
              SizedBox(height: buttonSize * 0.15),
              controlButton(
                size: buttonSize,
                label: "BACK",
                cmd: "B",
                icon: Icons.arrow_downward,
              ),
            ],
          ),
        ),
      ),
    );
  }
}
