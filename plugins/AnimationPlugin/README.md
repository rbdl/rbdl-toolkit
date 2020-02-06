Animation Plugin
================

This plugin enables reading of meshup animation files or csv files to bring an rbdl model to life.

# Animation Files

For creating new animation files please stick to a standard csv file. 

We only support the custom meshup animation file header to a certain extent,
in order to be able to read the data section of the file. It does **not**
how ever make an attempt to apply respect the ordering of data defined in the
header. It expects the data to be order the same way the joints are ordered in
the rbdl model, which is the default order the data gets exported in. The
header should only exist to inform human readers of the file what value
corresponds to what column!

The amount of columns a animation file has is required to be (dof of model)
+ 1. The first column being the time of the data frame. The other columns
represent the values of at the model joints at that time.

# Settings
This Plugin respects the following Settings loaded from the **rbdl-toolkit** Settings.

1. CSV Parameters
```
[FileReaderOptions]
csv.seperator=(default ",")
```
