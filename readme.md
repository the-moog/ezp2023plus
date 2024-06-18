Work in progress

# Dependencies

## Directly used in code

    gio
    glib2
    glibc
    cairo
    gtk4
    libadwaita
    libusb
    cmake (make)
    meson (make)

## On clean Arch Linux setup

    libusb (just to make sure. it already installed with package 'pacman')
    libadwaita (other gtk packages will be installed as dependencies)
    cmake (make)
    meson (make)

# Clone

    $ git clone https://github.com/alexandro-45/ezp2023plus.git

# Build

    $ meson setup buildDir .
    $ meson compile -C buildDir

# Install

    $ sudo meson install -C buildDir

# TODO list
- chip search from main window
- delay override from main window
- flash size override from main window
- erase (not implemented in libezp2023plus)
- translations
- saving chip editor changes into a file
- deactivate save button if chip_id is invalid
- prev/next switching in chip editor
- windows build