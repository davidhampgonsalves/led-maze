<img src="https://raw.githubusercontent.com/davidhampgonsalves/led-maze/main/maze-interaction.gif" align="right" width="200" />

```
░░░╗   ░░░╗ ░░░░░╗ ░░░░░░░╗░░░░░░░╗
░░░░╗ ░░░░║░░╔══░░╗╚══░░░╔╝░░╔════╝
░░╔░░░░╔░░║░░░░░░░║  ░░░╔╝ ░░░░░╗
░░║╚░░╔╝░░║░░╔══░░║ ░░░╔╝  ░░╔══╝
░░║ ╚═╝ ░░║░░║  ░░║░░░░░░░╗░░░░░░░╗
╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝
```

[video](https://www.instagram.com/p/DXWZpEjEaJM/)

You are a marble, the last of its kind. Make it through 10 levels and win your freedom.

Scan the QR code and tilt your phone to control your marble, dodging lava, teleporting through warps and locate hidden exits.

Its the most fun you can have with 300 pixels.

## Hard/Software
<img src="https://raw.githubusercontent.com/davidhampgonsalves/led-maze/main/ui.gif" align="right" width="200" />

An ESP32 powers the game logic and led driving. It also hosts a webserver which your phone connects to (through a seperatly hosted https proxy). This is required because iPhones will only allow access to the orientation sensor data over https.

The webpage "types" all its [contents out visually](https://github.com/davidhampgonsalves/led-maze/blob/main/sd-card/index.html#L171) and aims for a retro Comodore PET astetic.

Websockets are used for near realtime input, to trigger audio and vibration haptics.

### Levels
bmp files are 6x50 24 bit color.

### Web
https://maze.davidhampgonsalves.com:9000/

_Web UI ASCII title font is: ANSII SHADOW_

<img src="https://raw.githubusercontent.com/davidhampgonsalves/led-maze/main/maze-level.gif" align="right" width="200" />
### Console
Tap top left of webpage

### Bramble Decalf Coffee Recipe
`18.5 in, 45 out, over 20-23s`
