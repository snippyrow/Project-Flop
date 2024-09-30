# Project Flop
*"A game that, you know, will flop!"*

This is a game written on it's own kernel/bootloader, and therefore does not require an OS to run. Only certain computers are capable of running this nativly, such as on a USB. Through my testing, a Thinkpad will work best.
Other options include using an emulator such as QEMU, where you can simply launch the "game.img" file. A uniquely hard game, nobody has ever beaten it before!
- Adding any more code will cause the program to run into the buffer, so don't change anything to much!

In order to write the binary to a USB pen drive, use the following: (all data will be wiped permanently)
```
sudo dd if=game.img of=/dev/sdX bs=4M status=progress && sync
```


# Controls
Space to jump, Q to enter cheat mode.

Collect all the coins and survive as long as you can!

You win once you hit 50000 points, and it gets harder each time you die!

To survive, jump between the grey blades on the walls, and avoid touching the floor and ceiling!

