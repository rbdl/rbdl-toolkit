Settings
========

In this wiki I will describe each of the settings, as well as denote their default values. The description of each of the field is below: 

```
[General]
plugins.load.* : load various plugins

[CameraOptions]
Camera.default_distance : distance 
Camera.position : 
Camera.reset_enabled : 
Camera.view_center : 

[FileReaderOptions]
csv.seperator : csv delimiter character
csv.trim : 

[InverseKinematicsOptions]
IK.lambda : lambda to use for the inverese kinematics
IK.max_steps : maximum amount of steps to perform when running IK
IK.tolerance : tolerance for error on the calculated marker positions

[MarkerOptions]
marker.color : color of mocap data markers
marker.color_model : color of markers set on the model
marker.size : size of the sphere mesh to be displayed at the marker positions

[RenderOptions]
arrow.arrow_scale_factor : 
arrow.display_threshold : 
arrow.pos_at_tip : 
force.color : 
frame_axis.diameter : diameter of XYZ frame axes for each frame
frame_axis.size : length of XYZ frame axes from the frame origin for each frame
modeltree.color : colour of the wireframe linking each frame
modeltree.size : size of wireframe inking each frame
torque.color : 
```

The parameters can be found in `~/.config/ORB/rbdl-toolkit.conf`. The default values are as follows: 

```
[General]
plugins.load.AnimationPlugin=true
plugins.load.CameraOperatorPlugin=true
plugins.load.ForceTorquePlugin=true
plugins.load.InverseKinematicsPlugin=true
plugins.load.MotionMarkerPlugin=true
plugins.load.RenderPlugin=true
plugins.load.VisualExtrasPlugin=true

[CameraOptions]
Camera.default_distance=@Variant(\0\0\0\x87tare)
Camera.position=@Variant(\0\0\0T@\xc0\0\0@@\0\0@\xc0\0\0)
Camera.reset_enabled=true
Camera.view_center=@Variant(\0\0\0T\0\0\0\0\0\0\0\0\0\0\0\0)

[FileReaderOptions]
csv.seperator=44
csv.trim=false

[InverseKinematicsOptions]
IK.lambda=0.05
IK.max_steps=200
IK.tolerance=1e-08

[MarkerOptions]
marker.color=4278190335
marker.color_model=4294967295
marker.size=@Variant(\0\0\0\x87<#\xd7\n)

[RenderOptions]
arrow.arrow_scale_factor=@Variant(\0\0\0\x87:\x83\x12o)
arrow.display_threshold=@Variant(\0\0\0\x87:\x83\x12o)
arrow.pos_at_tip=false
force.color=3875471360
frame_axis.diameter=@Variant(\0\0\0\x87:\x83\x12o)
frame_axis.size=@Variant(\0\0\0\x87<\xa3\xd7\n)
modeltree.color=16776960
modeltree.size=@Variant(\0\0\0\x87:\x83\x12o)
torque.color=3858824960
```