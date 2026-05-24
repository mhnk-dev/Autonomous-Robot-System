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
    required double width,
    required String label,
    required String cmd,
    required IconData icon,
  }) {
    return GestureDetector(
      onTapDown: (_) => sendCommand(cmd),
      onTapUp: (_) => sendCommand("S"),
      onTapCancel: () => sendCommand("S"),
      child: Container(
        width: 90,
        height: 90,
        decoration: BoxDecoration(
          color: Colors.blueGrey,
          borderRadius: BorderRadius.circular(width * 0.02),
        ),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(icon, color: Colors.white),
            Text(label, style: const TextStyle(color: Colors.white)),
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
    double width = MediaQuery.of(context).size.width;

    return Scaffold(
      backgroundColor: Colors.black,
      appBar: AppBar(
        title: Text(
          "Robot Controller",
          style: TextStyle(color: Colors.white, fontSize: width * 0.016),
        ),
        backgroundColor: Colors.black,
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            controlButton(
              width: width,
              label: "FORWARD",
              cmd: "F",
              icon: Icons.arrow_upward,
            ),
            SizedBox(height: width * 0.02),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                controlButton(
                  width: width,
                  label: "LEFT",
                  cmd: "L",
                  icon: Icons.arrow_back,
                ),
                SizedBox(width: width * 0.02),
                controlButton(
                  width: width,
                  label: "STOP",
                  cmd: "S",
                  icon: Icons.stop,
                ),
                SizedBox(width: width * 0.02),
                controlButton(
                  width: width,
                  label: "RIGHT",
                  cmd: "R",
                  icon: Icons.arrow_forward,
                ),
              ],
            ),
            SizedBox(height: width * 0.02),
            controlButton(
              width: width,
              label: "BACK",
              cmd: "B",
              icon: Icons.arrow_downward,
            ),
          ],
        ),
      ),
    );
  }
}
