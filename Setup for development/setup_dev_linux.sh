echo "This is the setup script for the ambi light (raspberry pi) project made by floows.";
sudo apt update && sudo apt upgrade

# cmake
echo "Installing cmake..."
sudo apt install -y cmake

# --- Librarys ---
mkdir ../Librarys && cd ../Librarys

# rpi_ws2811x
echo "Installing and building rpi_ws281x..."
git clone https://github.com/jgarff/rpi_ws281x.git
cd rpi_ws281x
mkdir build && cd build
cmake -D BUILD_SHARED=OFF ..
cmake --build .
cd ../../

# OpenCV
echo "About to install and build OpenCV..."
echo "The OpenCV lib takes a long time to build on a raspberry pi! It took me more than a few hours."
while true; do
    read -p "Do you wish to continue? (y/n) " yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

# Note: needed for opencv to build
sudo apt install -y libgtk2.0-dev pkg-config 
sudo apt install -y g++ wget unzip
cd ../Librarys

# Download and build
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip
unzip opencv.zip
sudo rm -rf opencv.zip
cd opencv-4.x
mkdir -p build && cd build
cmake -D BUILD_SHARED_LIBS=OFF ..
cmake --build .
cd ../../../

# --- Project ---
echo "Everything is installed!" 
echo "Building project..."
mkdir Build && cd Build
cmake -G "Unix Makefile" ..
make

echo "Finished!"
