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

  @override
  void initState() {
    super.initState();

    webSocketChannel = IOWebSocketChannel.connect("ws://192.168.20.1:81");
  }

  void sendCommand(String cmd) {
    webSocketChannel.sink.add(cmd);
  }

  Widget controlButton({
    required double size,
    required String label,
    required String cmd,
    required IconData icon,
  }) {
    return GestureDetector(
      onTapDown: (_) => sendCommand(cmd),
      onTapUp: (_) => sendCommand("S"),
      onTapCancel: () => sendCommand("S"),
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
