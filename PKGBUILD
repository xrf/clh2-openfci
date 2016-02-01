# Maintainer: Fei Yuan <yuan@nscl.msu.edu>
pkgname=clh2-openfci
pkgver=1.0.1
pkgrel=1
pkgdesc="Tabulation provider for coulomb_ho2d using OpenFCI"
arch=(i686 x86_64)
url=https://github.com/xrf/clh2-openfci
license=(GPL3)
depends=(lpp)
makedepends=(curl git)
conflicts=(clh2-openfci-git)
source=($pkgname::git://github.com/xrf/clh2-openfci)
sha256sums=(SKIP)

build() {
    cd "$srcdir/$pkgname"
    git checkout v1
    make all
}

package() {
    cd "$srcdir/$pkgname"
    make DESTDIR="$pkgdir" PREFIX=/usr install
    install -Dm644 COPYING "$pkgdir/usr/share/licenses/$pkgname/COPYING"
}
