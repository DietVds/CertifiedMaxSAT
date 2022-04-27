#
# Script that extracts Minisat and QMaxSAT, merges them, applies the patches to them and builds the solver
#
ROOT_DIR=$(pwd)

# Build the source code
./scripts/building/build_src.sh ./src_tmp

# Apply the proof logging patch
cd src_tmp
patch -p1 < $ROOT_DIR/patches/dev/qmaxsat_with_timing.patch

# Build, cleanup and move binary to root
cd core
make
rm *.o
mv qmaxsat ../../qmaxsat_without_PL
cd $ROOT_DIR
rm -rf src_tmp
