echo "Running 3DEngine"

set -e
cd build
cmake --build .
./3DEngine
cd ../

