#!/bin/bash

timestamp=$(date '+%Y%m%d_%H%M%S')

backup_filename=hass_$timestamp.tar.gz

# Create Tar.gz
tar -cvzf $backup_filename /home/homeassistant/.homeassistant/ || exit 1

# Copy to google drive
rclone --config=/home/pi/.config/rclone/rclone.conf copy $backup_filename "gdrive:/rclone_backups" || exit 2

# check on google drive
rclone --config=/home/pi/.config/rclone/rclone.conf ls gdrive:/ | grep $backup_filename || exit 3
