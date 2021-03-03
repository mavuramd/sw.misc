#!/bin/bash

backup_wd=/home/pi/backups

pushd $backup_wd

rm -rf *.asc

gpg --encrypt --yes --sign --armor -r dilip426426@gmail.com *

# Copy to google drive
rclone copy *.asc "gdrive:/rclone_backups/hass" || exit 2

rm -rf *

popd
