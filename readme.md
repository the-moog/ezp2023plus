Work in progress

# Dependencies

    gio
    glib
    gtk4
    libadwaita
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
- delete hardcoded path in main.c
- erase (not implemented in libezp2023plus)
- ctrl-z for hex editor
- translations