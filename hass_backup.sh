#!/bin/bash

timestamp=$(date '+%Y%m%d_%H%M%S')

backup_filename=hass_$timestamp.tar.gz

# Create Tar.gz
tar -cvzf $backup_filename /home/homeassistant/.homeassistant/

# Copy to google drive
rclone --config=/home/pi/.config/rclone/rclone.conf copy $backup_filename "gdrive:/rclone_backups/hass" || exit 2

# check on google drive
rclone --config=/home/pi/.config/rclone/rclone.conf ls gdrive:/ | grep $backup_filename || exit 3

# encryption
cat keyfile | gpg --homedir /home/pi/.gnupg --encrypt --pinentry-mode loopback --yes --sign --armor -r dilip426426@gmail.com --passphrase-fd 0 $backup_filename || exit 4

# Copy to google drive
rclone --config=/home/pi/.config/rclone/rclone.conf copy $backup_filename.asc "gdrive:/rclone_backups/hass" || exit 4

# check on google drive
rclone --config=/home/pi/.config/rclone/rclone.conf ls gdrive:/ | grep $backup_filename.asc || exit 6
