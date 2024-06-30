Software for EZP2023+(and maybe other EZP*) programmer

**Attention:** Work in progress. Some basic features are still in development and there are some bugs :)

## Current features

* Test, read and write chips
* View and edit dump files. Undo and redo operations are available
* Open and save dump files
* Chips database editor
* Chips database file format is the same as in original Chinese software  

## Dependencies

### Directly used in code

    gio
    glib2
    glibc
    cairo
    gtk4
    libadwaita
    libusb
    meson (make)

### On clean Arch Linux setup

    libusb (just to make sure. it already installed with package 'pacman')
    libadwaita (other gtk packages will be installed as dependencies)
    meson (make)

## Setup

### Clone

    $ git clone https://github.com/alexandro-45/ezp2023plus.git
    $ cd ezp2023plus

### Build

    $ meson setup buildDir .
    $ meson compile -C buildDir

### Install

    $ sudo meson install -C buildDir
