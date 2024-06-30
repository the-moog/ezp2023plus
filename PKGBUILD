pkgname=ezp2023plus
pkgver=0.9
pkgrel=1
pkgdesc='Software for EZP2023 programmer'
arch=('x86_64')
url='https://github.com/alexandro-45/ezp2023plus'
license=('GPL-2.0-or-later')
makedepends=('meson')
depends=('libusb' 'libadwaita' 'gtk4')
source=("https://github.com/alexandro-45/ezp2023plus/archive/refs/tags/v${pkgver}.tar.gz")
sha256sums=('194c393d26c7e0669fee9562d36a9d5d4c5f8c27b3dee14c3a26446e0a8fa46c')

build() {
    arch-meson $pkgname-$pkgver build --wrap-mode default
    meson compile -C build
}

check() {
    meson test -C build --print-errorlogs
}

package() {
    meson install -C build --destdir "$pkgdir"
}