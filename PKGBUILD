# Maintainer: Fei Yuan <yuan@nscl.msu.edu>
pkgname=clh2-openfci
pkgver=1.0.1
pkgrel=1
pkgdesc="Tabulation provider for coulomb_ho2d using OpenFCI"
arch=(i686 x86_64)
url=https://github.com/xrf/clh2-openfci
license=(GPL3)
depends=(lpp)
makedepends=(curl)
conflicts=(clh2-openfci-git)
source=(https://github.com/xrf/clh2-openfci/archive/$pkgver.tar.gz)
sha256sums=('85c21de3d6d57f863341ddc29019f14861ce0dd4641371af0f2674f7c115ff86')

build() {
    cd "$srcdir/$pkgname-$pkgver"
    make all
}

package() {
    cd "$srcdir/$pkgname-$pkgver"
    make DESTDIR="$pkgdir" PREFIX=/usr install
    install -Dm644 COPYING "$pkgdir/usr/share/licenses/$pkgname/COPYING"
}
