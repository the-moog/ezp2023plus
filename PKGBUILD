pkgname=ezp2023plus
pkgver=0.9.1
pkgrel=1
pkgdesc='Software for EZP2023 programmer'
arch=('x86_64')
url='https://github.com/alexandro-45/ezp2023plus'
license=('GPL-2.0-or-later')
makedepends=('meson')
depends=('libusb' 'libadwaita' 'gtk4')
source=("https://github.com/alexandro-45/ezp2023plus/archive/refs/tags/v${pkgver}.tar.gz")
sha256sums=('18cd6a5bdcb76b61e2119ac7b66593b78da27a4aab782d2dbbc099a5b0fbd74f')

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