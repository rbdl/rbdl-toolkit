Inverse Kinematics and Dynamics
=================

## Inverse Kinematics 

Given marker information, Toolkit is able to perform inverse kinematics to estimate the joint angle. An set of 
example files can be found in `examples/jumping`. To load the Lua model and the marker file into rbdl-toolkit, the 
command is `rbdl-toolkit --model model_deLeva_reduced_meshes_updated.lua
--motionmarker pendulum.csv`.

To perform IK, the model file must contain a set of model marker data, which tells rbdl where the markers are 
expected to be, denoted as the marker parameter in this example. 

```
  name   = "Shank_R",
  parent = "Thigh_R",
  visuals = {{
    src  = "meshes/shank.obj",
    dimensions  =    { 0.09620384090780759, 0.09620384090780759, 0.5772230454468454,},
    mesh_center =    { 0.0, 0.0, -0.24050960226951895,},
    color       =    { 0.2, 0.7, 0.3,},
    },},
  joint_frame = {
    r =    { 0.0, 0.0, -0.49261621870323574,},
    E = 
      {{ 1.0, 0.0, 0.0,},
       { 0.0, 1.0, 0.0,},
       { 0.0, 0.0, 1.0,},},
  },
  body = {
    mass = 3.8835180205509205,
    com =    { 0.0, 0.0, -0.21140794039490715,},
    inertia = 
      {{ 0.05661057932661182, 0.0, 0.0,},
       { 0.0, 0.054377641918846385, 0.0,},
       { 0.0, 0.0, 0.009348684422692806,},},
  },
  markers = {
    R_FAX =  { 0.0, -0.051740928075740725, -0.047799526307102866,},
    R_TTC =  { 0.044597633995881035, 0.0, -0.10467188566080729,},
    R_FAL =  { 0.0, -0.04605324082023483, -0.4766302737782796,},
    R_TAM =  { 0.0, 0.04605324082023483, -0.46825823322041826,},
  },
```

RBDL uses Levenberg Marquardt least squares to perform this fit, and require the adjustment of the 
IK `lambda` (damping factor on the fit), `max_step` (max number of iteration per frame), and `tol` (step tolerance 
per iteration)

## Inverse Dynamics

Pending