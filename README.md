# MR
Marker based openGL mixed reality 

## Requirements
### npm
Install `npm` using your package manager <br/>

### OpenCV
Install `OpenCV` using your package manager <br/>

**In Arch Linux**
```zsh
$ sudo pacman -S vtk
$ sudo pacman -S hdf5
```

### OpenGL
In Linux install `freeglut`  <br/>


## Usage
In project directory
```zsh
$ cd app/
$ npm install 
$ npm start
```
First press **Calibration** in order to calibrate the camera.</br>
You'll need to use this [image](https://docs.opencv.org/2.4/_downloads/pattern.png).
Insert the square size in meters and </br>
Simply capture it in differnet angels (press space key to capture).</br></br>
All markers will be downloaded into [Markers](app/Markers).</br></br>
Secondly press **Marker Detection**.</br>
Insert the marker size in meters.</br></br>
Press **End** in order ro close the calibration or the marker detection.
