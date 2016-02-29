# PS3EventClient for Kodi

This implements the API to process PS3 controller input to Kodi's Event Server.

## Requirements

You need to have your PS3 controller paired with the Raspberry Pi, using sixad.
You need to allow your user to use the command `service` without password by editing your sudoers file:
```
user (ALL) = (root) NOPASSWD: /usr/bin/service
```

- First, you need to add a `udev` rule to `/etc/udev/rules.d`, you can find it in the [resources](resources/99-zps3event.rules) folder.
- Change the `RUN` command to match the path to this [script](resources/udevscript.sh) which you will have made executable with `chmod +x`
- Compile the client with `make`
- You'll need to add lines in `/etc/init/xbmc.conf`. At the end of the pre-start script, before `exit 0`, add:
```
/path/to/ps3event_clientd
```
and at the end of the post-stop script,
```
pkill -F "/var/run/ps3event_clientd.pid"
```

## How it works

When you turn on your controller, the first udev rule will map your controller to `/dev/ps3controller` and launch the script to launch the service `xbmc`.
Before starting, the service `xbmc` will launch `ps3event_clientd`. It will act as a daemon, and grab the device `/dev/ps3controller`, this is needed to prevent Kodi to grab all input events. Then each 100 milliseconds, it will read the inputs and send them to Kodi, to the joystick keymap `PLAYSTATION(R)3 Controller`.
When you turn off your controller by maintening 10 seconds the PS button, the second udev rule will stop the `xbmc` service. This one will, after stopping, kill the process of `ps3event_clientd`.
