#!/usr/bin/env bash

unset CDPATH

# target directory
DJGPP_PREFIX=${DJGPP_PREFIX-/usr/local/djgpp}

# enabled languages
ENABLE_LANGUAGES=${ENABLE_LANGUAGES-c,c++}

DJGPP_DOWNLOAD_BASE="http://www.mirrorservice.org/sites/ftp.delorie.com/pub"
FTPMIRROR_GNU_DOWNLOAD_BASE="http://www.mirrorservice.org/sites/ftp.gnu.org/gnu"
FTP_GNU_DOWNLOAD_BASE="http://www.mirrorservice.org/sites/ftp.gnu.org/gnu"

# source tarball versions
BINUTILS_VERSION=230
DJCRX_VERSION=205
DJLSR_VERSION=205
DJDEV_VERSION=205

GCC_VERSION=12.1.0
GCC_VERSION_SHORT=12.10
GMP_VERSION=6.2.1
MPFR_VERSION=4.1.0
MPC_VERSION=1.2.1
AUTOCONF_VERSION=2.69
AUTOMAKE_VERSION=1.15.1

CC=gcc
CXX=g++

# use gmake under FreeBSD
if [ `uname` = "FreeBSD" ]; then
  MAKE=gmake
  CC=clang
  CXX=clang++
else
  MAKE=make
fi
#CFLAGS="-O2 -g $CFLAGS -std=gnu11"

export CC CXX CFLAGS MAKE

# tarball location
BINUTILS_ARCHIVE="http://www.mirrorservice.org/sites/ftp.delorie.com/pub/djgpp/deleted/v2gnu/bnu${BINUTILS_VERSION}s.zip"
DJCRX_ARCHIVE="http://www.mirrorservice.org/sites/ftp.delorie.com/pub/djgpp/current/v2/djcrx${DJCRX_VERSION}.zip"
DJLSR_ARCHIVE="http://www.mirrorservice.org/sites/ftp.delorie.com/pub/djgpp/current/v2/djlsr${DJLSR_VERSION}.zip"
DJDEV_ARCHIVE="http://www.mirrorservice.org/sites/ftp.delorie.com/pub/djgpp/current/v2/djdev${DJDEV_VERSION}.zip"

DJCROSS_GCC_ARCHIVE="http://www.mirrorservice.org/sites/ftp.delorie.com/pub/djgpp/rpms/djcross-gcc-${GCC_VERSION}/djcross-gcc-${GCC_VERSION}.tar.bz2"
GCC_ARCHIVE="http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"
GMP_ARCHIVE="http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/gmp/gmp-${GMP_VERSION}.tar.xz"
MPFR_ARCHIVE="http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/mpfr/mpfr-${MPFR_VERSION}.tar.xz"
MPC_ARCHIVE="http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/mpc/mpc-${MPC_VERSION}.tar.gz"
AUTOCONF_ARCHIVE="http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/autoconf/autoconf-${AUTOCONF_VERSION}.tar.xz"
AUTOMAKE_ARCHIVE="http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/automake/automake-${AUTOMAKE_VERSION}.tar.xz"


# djcross-gcc-X.XX-tar.* maybe moved from /djgpp/rpms/ to /djgpp/deleted/rpms/ directory.
OLD_DJCROSS_GCC_ARCHIVE=${DJCROSS_GCC_ARCHIVE/rpms\//deleted\/rpms\/}

# download source files
ARCHIVE_LIST="$BINUTILS_ARCHIVE $DJCRX_ARCHIVE $DJLSR_ARCHIVE $DJDEV_ARCHIVE
              $SED_ARCHIVE $DJCROSS_GCC_ARCHIVE $OLD_DJCROSS_GCC_ARCHIVE $GCC_ARCHIVE
              $GMP_ARCHIVE $MPFR_ARCHIVE $MPC_ARCHIVE
              $AUTOCONF_ARCHIVE $AUTOMAKE_ARCHIVE"

echo "Download source files..."
mkdir -p download || exit 1
cd download

for ARCHIVE in $ARCHIVE_LIST; do
  FILE=`basename $ARCHIVE`
  if ! [ -f $FILE ]; then
    echo "Download $ARCHIVE ..."
    if [ ! -z $USE_WGET ]; then
      DL_CMD="wget -U firefox $ARCHIVE"
    else
      DL_CMD="curl -f $ARCHIVE -L -o $FILE"
    fi
    echo "Command : $DL_CMD"
    if ! eval $DL_CMD; then
      if [ "$ARCHIVE" == "$DJCROSS_GCC_ARCHIVE" ]; then
        echo "$FILE maybe moved to deleted/ directory."
      else
        rm $FILE
        echo "Download $ARCHIVE failed."
        exit 1
      fi
    fi
  fi
done
cd ..

# create target directory, check writable.
echo "Make prefix directory : $DJGPP_PREFIX"
mkdir -p $DJGPP_PREFIX

if ! [ -d $DJGPP_PREFIX ]; then
  echo "Unable to create prefix directory"
  exit 1
fi

if ! [ -w $DJGPP_PREFIX ]; then
  echo "prefix directory is not writable."
  exit 1
fi

# make build dir
echo "Make build dir"
rm -rf build || exit 1
mkdir -p build || exit 1
cd build

# build binutils
echo "Building binutils"
mkdir bnu${BINUTILS_VERSION}s
cd bnu${BINUTILS_VERSION}s
unzip ../../download/bnu${BINUTILS_VERSION}s.zip || exit 1
cd gnu/binutils-* || exit

# exec permission of some files are not set, fix it.
for EXEC_FILE in install-sh missing; do
  echo "chmod a+x $EXEC_FILE"
  chmod a+x $EXEC_FILE || exit 1
done

sh ./configure \
           --prefix=$DJGPP_PREFIX \
           --target=i586-pc-msdosdjgpp \
           --program-prefix=i586-pc-msdosdjgpp- \
           --disable-werror \
           --disable-nls \
           || exit 1

${MAKE} configure-bfd || exit 1
${MAKE} -C bfd stmp-lcoff-h || exit 1
${MAKE} || exit 1

if [ ! -z $MAKE_CHECK ]; then
  echo "Run ${MAKE} check"
  ${MAKE} check || exit 1
fi

${MAKE} install || exit 1

cd ../../..
# binutils done

# prepare djcrx
echo "Prepare djcrx"
mkdir djcrx${DJCRX_VERSION}
cd djcrx${DJCRX_VERSION}
unzip ../../download/djcrx${DJCRX_VERSION}.zip || exit 1
patch -p1 -u < ../../patch/patch-djcrx205.txt || exit 1

cd src/stub
${CC} -O2 stubify.c -o stubify || exit 1
${CC} -O2 stubedit.c -o stubedit || exit 1

cd ../..

mkdir -p $DJGPP_PREFIX/i586-pc-msdosdjgpp/sys-include || exit 1
cp -rp include/* $DJGPP_PREFIX/i586-pc-msdosdjgpp/sys-include/ || exit 1
cp -rp lib $DJGPP_PREFIX/i586-pc-msdosdjgpp/ || exit 1
cp -p src/stub/stubify $DJGPP_PREFIX/i586-pc-msdosdjgpp/bin/ || exit 1
cp -p src/stub/stubedit $DJGPP_PREFIX/i586-pc-msdosdjgpp/bin/ || exit 1

cd ..
# djcrx done

# build gcc
tar -xjvf ../download/djcross-gcc-${GCC_VERSION}.tar.bz2 || exit 1
cd djcross-gcc-${GCC_VERSION}/

BUILDDIR=`pwd`

echo "Building autoconf"
cd $BUILDDIR
tar xJf ../../download/autoconf-${AUTOCONF_VERSION}.tar.xz || exit 1
cd autoconf-${AUTOCONF_VERSION}/
./configure --prefix=$BUILDDIR/tmpinst || exit 1
${MAKE} all install || exit 1

echo "Building automake"
cd $BUILDDIR
tar xJf ../../download/automake-${AUTOMAKE_VERSION}.tar.xz || exit 1
cd automake-${AUTOMAKE_VERSION}/
PATH="$BUILDDIR//tmpinst/bin:$PATH" \
./configure --prefix=$BUILDDIR/tmpinst || exit 1
PATH="$BUILDDIR//tmpinst/bin:$PATH" \
${MAKE} all install || exit 1

# build GNU sed if needed.
SED=sed
if [ ! -z $SED_VERSION ]; then
  echo "Building sed"
  cd $BUILDDIR
  tar xjf ../../download/sed-${SED_VERSION}.tar.bz2 || exit 1
  cd sed-${SED_VERSION}/
  ./configure --prefix=$BUILDDIR/tmpinst || exit 1
  ${MAKE} all install || exit 1
  SED=$BUILDDIR/tmpinst/bin/sed
fi

cd $BUILDDIR
tar xJf ../../download/gmp-${GMP_VERSION}.tar.xz || exit 1
tar xJf ../../download/mpfr-${MPFR_VERSION}.tar.xz || exit 1
tar xzf ../../download/mpc-${MPC_VERSION}.tar.gz || exit 1

# gcc 4.8 or above unpack-gcc.sh needs to be patched for OSX
# patch from :
#   ( cd gnu && tar xf $top/$archive $tar_param && echo $archive >$top/s-sources )
# to :
#   ( cd gnu && tar xJf $top/$archive && echo $archive >$top/s-sources )
echo "Patch unpack-gcc.sh"
$SED -i "s/\(cd gnu && tar x\)\(f [^ ]* \)\([^ ]* \)/\1J\2/" unpack-gcc.sh || exit 1

# In compressed file gcc-12.1.0.tar.xz, its directory :
# gcc-12.1.0/gcc/testsuite/go.test/test/fixedbugs/issue27836.dir/
# contains files with non-ASCII filename. They can't be decompressed in
# Windows system.
# Use tar's --exclude option to skip this directory.
# patch from :
#   ( cd gnu && tar xJf $top/$archive && echo $archive >$top/s-sources )
# to :
#   ( cd gnu && tar xJf $top/$archive  --exclude=issue27836.dir && echo $archive >$top/s-sources )
if uname|grep "^MINGW32" > /dev/null; then
  $SED -i "s/\(cd gnu && tar [^\&]*\)/\1 --exclude=issue27836.dir /" unpack-gcc.sh || exit 1
fi

if [ `uname` = "FreeBSD" ]; then
  # The --verbose option is not recognized by BSD patch
  $SED -i 's/patch --verbose/patch/' unpack-gcc.sh || exit 1
fi

echo "Running unpack-gcc.sh"
PATH="$BUILDDIR/tmpinst/bin:$PATH" sh unpack-gcc.sh --no-djgpp-source ../../download/gcc-${GCC_VERSION}.tar.xz || exit 1

if uname|grep "^MINGW32" > /dev/null; then
  echo "Patch GCC for MinGW"
  (cd $BUILDDIR && patch -p0 < $BUILDDIR/../../patch/patch-gcc12-mingw.txt) || exit 1
fi

# patch gnu/gcc-X.XX/gcc/doc/gcc.texi
echo "Patch gcc/doc/gcc.texi"
cd gnu/gcc-*/gcc/doc || exit 1
$SED -i "s/[^^]@\(\(tex\)\|\(end\)\)/\n@\1/g" gcc.texi || exit 1
cd -

# copy stubify programs
cp $DJGPP_PREFIX/i586-pc-msdosdjgpp/bin/stubify $BUILDDIR/tmpinst/bin

echo "Building gmp"
cd $BUILDDIR/gmp-*/
# On some systems, building gmp will fail if CFLAGS is set.
# Unset CFLAGS during building gmp.
env -u CFLAGS ./configure --enable-fat --prefix=$BUILDDIR/tmpinst --enable-static --disable-shared || exit 1
${MAKE} all || exit 1
if [ ! -z $MAKE_CHECK ]; then
  echo "Run ${MAKE} check"
  ${MAKE} check || exit 1
fi
${MAKE} install || exit 1

echo "Building mpfr"
cd $BUILDDIR/mpfr-${MPFR_VERSION}/
./configure --prefix=$BUILDDIR/tmpinst --with-gmp=$BUILDDIR/tmpinst --enable-static --disable-shared || exit 1
${MAKE} all || exit 1
if [ ! -z $MAKE_CHECK ]; then
  echo "Run ${MAKE} check"
  ${MAKE} check || exit 1
fi
${MAKE} install || exit 1

echo "Building mpc"
cd $BUILDDIR/mpc-${MPC_VERSION}/
./configure --prefix=$BUILDDIR/tmpinst --with-gmp=$BUILDDIR/tmpinst --with-mpfr=$BUILDDIR/tmpinst --enable-static --disable-shared || exit 1
${MAKE} all || exit 1
if [ ! -z $MAKE_CHECK ]; then
  echo "Run ${MAKE} check"
  ${MAKE} check || exit 1
fi
${MAKE} install || exit 1

echo "Building gcc"
cd $BUILDDIR/

mkdir djcross
cd djcross

PATH="$BUILDDIR//tmpinst/bin:$PATH" \
../gnu/gcc-${GCC_VERSION_SHORT}/configure \
                                 --target=i586-pc-msdosdjgpp \
                                 --program-prefix=i586-pc-msdosdjgpp- \
                                 --prefix=$DJGPP_PREFIX \
                                 --disable-nls \
                                 --disable-plugin \
                                 --disable-lto \
                                 --enable-lto \
                                 --enable-libstdcxx-filesystem-ts \
                                 --enable-libquadmath-support \
                                 --with-gmp=$BUILDDIR/tmpinst \
                                 --with-mpfr=$BUILDDIR/tmpinst \
                                 --with-mpc=$BUILDDIR/tmpinst \
                                 --enable-version-specific-runtime-libs \
                                 --enable-languages=${ENABLE_LANGUAGES} \
                                 || exit 1

${MAKE} j=4 "PATH=$BUILDDIR/tmpinst/bin:$PATH" || exit 1

# Workaround for MinGW. Need to fix some .deps/*.Plo files.
# Fix from
#  e\:/build-djgpp\...
# to
#  /e/build-djgpp\...

if uname|grep "^MINGW32" > /dev/null; then
  for i in i586-pc-msdosdjgpp/libssp/.deps/*.Plo; do sed -e 's/^ \([a-zA-Z]\)\\\:/ \/\1/' $i -i; done
  for i in i586-pc-msdosdjgpp/libquadmath/*/.deps/*.Plo; do sed -e 's/^ \([a-zA-Z]\)\\\:/ \/\1/' $i -i; done
fi

${MAKE} install-strip || exit 1

echo "Copy long name executables to short name."
(
  cd $DJGPP_PREFIX || exit 1
  SHORT_NAME_LIST="gcc g++ c++ addr2line c++filt cpp size strings"
  for SHORT_NAME in $SHORT_NAME_LIST; do
    if [ -f bin/i586-pc-msdosdjgpp-gcc ]; then
      cp bin/i586-pc-msdosdjgpp-$SHORT_NAME i586-pc-msdosdjgpp/bin/$SHORT_NAME
    fi
  done
) || exit 1

# gcc done

# build djlsr (for dxegen / exe2coff)
echo "Prepare djlsr"
cd $BUILDDIR
cd ..
rm -rf djlsr${DJLSR_VERSION}
mkdir djlsr${DJLSR_VERSION}
cd djlsr${DJLSR_VERSION}
unzip ../../download/djlsr${DJLSR_VERSION}.zip || exit 1
unzip -o ../../download/djdev${DJDEV_VERSION}.zip "include/*/*" || exit 1
unzip -o ../../download/djdev${DJDEV_VERSION}.zip "include/*" || exit 1
patch -p1 -u < ../../patch/patch-djlsr205.txt || exit 1
if [ "$CC" == "gcc" ]; then
  echo "Building DXE tools."
  cd src
  PATH=$DJGPP_PREFIX/bin/:$PATH make || exit 1
  cp dxe/dxegen dxe/dxe3gen dxe/dxe3res $DJGPP_PREFIX/i586-pc-msdosdjgpp/bin/ || exit 1
  cd ..
else
  echo "Building DXE tools requires gcc, skip."
fi
cd src/stub
${CC} -o exe2coff exe2coff.c || exit 1
cp -p exe2coff $DJGPP_PREFIX/i586-pc-msdosdjgpp/bin/ || exit 1
cd ../../..
# djlsr done

# copy setenv script
(cd $BUILDDIR/../../setenv/ && ./copyfile.sh $DJGPP_PREFIX) || exit 1

echo "Testing DJGPP."
cd $BUILDDIR
cd ..
echo "Use DJGPP to build a test C program."
$DJGPP_PREFIX/bin/i586-pc-msdosdjgpp-gcc ../hello.c -o hello || exit 1

for x in $(echo $ENABLE_LANGUAGES | tr "," " ")
do
  case $x in
    c++)
      echo "Use DJGPP to build a test C++ program."
      $DJGPP_PREFIX/bin/i586-pc-msdosdjgpp-c++ ../hello-cpp.cpp -o hello-cpp || exit 1
      ;;
  esac
done

echo "build-djgpp.sh done."
