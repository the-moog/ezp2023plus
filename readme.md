Work in progress

# Dependencies

    [TODO: some gtk deps]
    libusb
    cmake (make)
    meson (make)

# Clone

    $ git clone --recursive https://github.com/alexandro-45/ezp2023plus.git

# Build

    $ meson setup buildDir .
    $ meson compile -C buildDir

# Install

    $ sudo meson install -C buildDir

# TODO list
- add gtk deps
- write
- erase (not implemented in libezp2023plus)
- basic hex editor operations(edit bytes, ctrl-z)
- translations