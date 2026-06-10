# RagCam

Hi! I'm the guy who put out the original RagCam. 

The program that lets you fly though Ragnarok Online maps has been updated to support the latest maps.

**NOTE: THIS IS THE ORIGINAL CODE THAT HAS SEVERAL ISSUES AND BUGS AND I WILL NOT BE UPDATING THE CODE TO FIX THESE - INSTEAD I AM CREATING A NEW VERSION FROM SCRATCH WITHOUT THE LEGACY CODE TO GET IN THE WAY**

# Known Issues

* Some incorrect textures
* No water (was a hack before)
* Some incorrect animations
* Buggy UI

# Usage

Dowload `ragcam.exe`

Create a file called `ragcam.arg`, and something like below.

```
C:\Gravity\Ragnarok Online\
data\alberta.rsw
```

First line: Path that contains data.grf. MUST end in a backslash
Second line: data path to the map / gnd / model

Run the program.

Click inside the program and press Space to capture the mouse. 

Use WASD and the mouse to move

Press Space to release the mouse

F1 for help.


# Acknowledgements

defeufeu -the original coder behind rsmview, the base code behind ragcam

The reference I used to fix loading new data.grf, and the code I will be basing the new RagCam on - https://github.com/Tokeiburu/GRFEditor
