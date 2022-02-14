#
# Script that extracts Minisat and QMaxSAT, merges them, applies the patches to them and builds the solver
#
ROOT_DIR=$(pwd)

# Build the source code
./scripts/build_src.sh ./src_tmp

# Apply the proof logging patch
cd src_tmp
patch -p1 < $ROOT_DIR/patches/prooflogging.patch

# Build, cleanup and move binary to root
cd core
make
rm *.o
mv qmaxsat ../..
cd $ROOT_DIR
rm -rf src_tmp
