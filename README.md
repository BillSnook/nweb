nweb - a simple web interface and interface control testing tool
=========

Code for Intel Edison.


After configure_edison --setup
and wifi is setup, update and
setup Edison for development

----------------

// Setup .ssh file on device
scp ~/.ssh/Saturn_id-rsa.pub root@192.168.0.9:/home/root/.ssh/authorized_users

----------------

// Setup opkg update config
//  in /etc/opkg/base-feeds.conf

src/gz all http://repo.opkg.net/edison/repo/all
src/gz edison http://repo.opkg.net/edison/repo/edison
src/gz core2-32 http://repo.opkg.net/edison/repo/core2-32

----

// Setup mraa update config
//  in /etc/opkg/mraa-upm.conf

src mraa-upm http://iotdk.intel.com/repos/1.1/intelgalactic

----

// Perform updates
opkg upgrade

opkg install nano
opkg install libmraa0

----------------

// Upgrade mraa

// On board - libmraa

// On dev machine - path to latest header files
// update on edison, cp to host
// get header files from github

----------------

// Script run at shell startup - setup shell commands
// In ~/.profile

alias l="ls -al"
alias np="nano ~/.profile"

alias co="cd /opt/ea-web"
alias go="systemctl start ea-web"
alias no="systemctl stop ea-web"
alias ws="systemctl -l status ea-web"

----------------

// Stop current web server by commenting out last line
//  in usr/lib/edison_config_tools/edison-config-server.js 
// This should also work, not tried yet:
// systemctl stop    edison_config
// systemctl disable edison_config


// Create directory for nweb service executable file
cd /opt
mkdir ea-web
cd ea-web
mkdir www           // Newer root directory for web server
mkdir web_src       // Deprecated - replace methodically with www

// Create service
// In /etc/systemd/system/multi-user.target.wants/, create symbolic link
ln -s /lib/systemd/system/ea-web.service /etc/systemd/system/multi-user.target.wants/ea-web.service

// In /lib/systemd/system/ create file ea-web.service:

start of file >>>
[Unit]
Description=ea-web - command and control service for Edison development
ConditionPathExists=/opt/ea-web/www
After=network.target

[Service]
Type=simple
# Start web/command dev tool
ExecStart=/opt/ea-web/nweb 80 /opt/ea-web/www
Restart=on-failure

[Install]
WantedBy=multi-user.target
<<< end of file

----

// Then use these calls
systemctl enable ea-web
systemctl start ea-web

systemctl status ea-web

----------------

----------------

Development on Mac for latest mraa library - 0.6 currently

----

// Put latest .h and .hpp files
//  in /Users/bill/Code/Edison/iotdk-ide-mac/devkit-x86.sysroots/i586-poky-linux/usr/include/
// Move libmraa.so.0.6.0 from board location /usr/lib/libmraa.so.0.6.0
//  to /Users/bill/Code/Edison/iotdk-ide-mac/devkit-x86.sysroots/i586-poky-linux/usr/lib/
//  and make /usr/lib/libmraa.so point to it with symbolic link
ln -s /Users/bill/Code/Edison/iotdk-ide-mac/devkit-x86/sysroots/i586-poky-linux/usr/lib/libmraa.so.0.6.0 /Users/bill/Code/Edison/iotdk-ide-mac/devkit-x86/sysroots/i586-poky-linux/usr/lib/libmraa.so.0
// or
cd /Users/bill/Code/Edison/iotdk-ide-mac/devkit-x86/sysroots/i586-poky-linux/usr/lib
ln -s libmraa.so.0.6.0 libmraa.so.0


----------------
