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
- chip search from main window
- delay override from main window
- flash size override from main window
- automatic test before read or write
- try to build on a clean machine
- erase (not implemented in libezp2023plus)
- ctrl-z for hex editor
- translations