PythonPlugin
=======================

## Build and Install

With these commands you can build this plugin outside of the rbdl-toolkit src tree, but you will also need to have a rbdl-toolkit build dir.

```shell
mkdir build
cd build
cmake -DCUSTOM_QT_PATH=<QT_Lib_PATH> -DCMAKE_BUILD_TYPE=Release -DTOOLKIT_BUILD_PATH=<Toolkit_Build_DIR> -DTOOLKIT_SOURCE_PATH=<Toolkit_Source_DIR> ..
make
sudo make install
```