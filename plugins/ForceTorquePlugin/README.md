Force Torque Plugin
===================

A plugin to load and display force and torque data from a data csv file.

# Column Format

Every row of the csv represents a data frame at a current time. The first
column is expected to contain the time. Every row is expected to have the
same amount of columns.

The columns after should always equal a multiple of 9. Defining a point in *3D
space* + a *force* at that point + a *torque* at that point. The magnitude of the
force/torque vector is used to scale the size of the arrow used to represent
it. And direction of the vector is used to align the arrow, in the case of
the torque the direction describes the axis of rotation.

``` csv
Time | [POS_X | POS_Y | POS_Z | FORCE_X | FORCE_Y | FORCE_Z | TORQUE_X | TORQUE_Y | TORQUE_Z] +
``` 

# Settings

This Plugin respects the following Settings loaded from the **rbdl-toolkit** Settings.

1. CSV Parameters
```
[FileReaderOptions]
csv.seperator=(default ",")
```

2. Render Options
```
[RenderOptions]
arrow.arrow_scale_factor=(0.001)
arrow.display_threshold=(0.001)
arrow.pos_at_tip=(false)
force.color=(red)
torque.color=(green)
```
