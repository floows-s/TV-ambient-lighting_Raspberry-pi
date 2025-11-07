@echo off

mkdir temp
echo Downloading self extracting OpenCV library!
curl -L -o .\temp\opencv-4.12.0-windows.exe https://github.com/opencv/opencv/releases/download/4.12.0/opencv-4.12.0-windows.exe

start /wait .\temp\opencv-4.12.0-windows.exe -o"C:\" -y
rmdir /s /q .\temp
