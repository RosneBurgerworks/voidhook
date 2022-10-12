git submodule update --init --recursive

proccount=$(grep -c '^processor' /proc/cpuinfo)
mkdir -p ./build
# Update cathook
pushd build && cmake .. && cmake --build . --target cathook -- -j$proccount || { echo -e "\033[1;31m \n \nFailed to compile cathook\n\033[0m"; exit 1; }
# Update data
cwd="$(pwd)"
cmake --build . --target data || { echo -e "\033[1;31m\nFailed to update /opt/voidhook/data directory! Trying with root rights!\n\033[0m"; $SUDO bash -c "cd \"$cwd\"; cmake --build . --target data" || { echo -e "\033[1;31m\nFailed to update /opt/voidhook/data directory\n\033[0m"; exit 1; } }
popd
echo -e "\n\033[1;34mCathook updated successfully\n\033[0m"
