# Installing CI Builds

To avoid producing large artifacts, 'portable' packaging has not been performed on this CI build. To install:

## Dependencies
### QT5
  * Qt base and Qt Serial Port support is required.
  * **MacOS** Use homebrew:
    ```brew install qt5```
  * **Ubuntu**
    ```apt install qt5-default```
  * **Other** *Please extend this list per your experience*

### RTKLIB Shared Library
  * This needs to be moved to somewhere in the dynamic linker path.
  * `rtklib.h` isn't needed unless you plan to link against the library.
  * **MacOS**
    ```cp lib/libRTKLib.dylib /usr/local/lib```
 * **Linux**
    ```cp lib/libRTKLib.so /usr/local/lib```

### Applications
  * Copy CLI apps to somewhere in your system path
  * **'nix**
  ```cp bin/* /usr/local/bin```
  * Copy QT GUI applications to wherever is convenient