# check if bin is there, otherwise give error maybe say build the file...
# move bin into sbin
# make it launch on start up
[Unit]
Description=Dream frame service | Example: Run startup script for webserver

[Service]
WorkingDirectory=${DREAM_FRAME_FILES_PATH}
ExecStart=/usr/local/sbin/startup.sh

[Install]
# multi-user.target makes the service run when the system finished booting,
# but just before starting the GUI.
WantedBy=multi-user.target 
