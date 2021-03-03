#!/bin/bash

timestamp=$(date '+%Y%m%d_%H%M%S')

backup_wd=/home/pi/backups

backup_dir=hass_$timestamp

backup_filename=$backup_dir.tar.gz

pushd $backup_wd

mkdir $backup_dir

cp --parents -r /home/homeassistant/.homeassistant/ $backup_dir

cp --parents -r /home/homeassistant/dehydrated/ $backup_dir

cp --parents -r /opt/zigbee2mqtt/data/ $backup_dir

cp --parents -r /home/pi/.gnupg $backup_dir

cp --parents -r /home/pi/.config/rclone $backup_dir

p --parents -r /home/pi/.config/backup_script $backup_dir

cp --parents /etc/motion/motion.conf $backup_dir

cp --parents /etc/systemd/system/zigbee2mqtt.service $backup_dir

cp --parents /etc/systemd/system/hass.service $backup_dir

cp --parents /etc/systemd/system/hass_backup.service $backup_dir

cp --parents /etc/systemd/system/dehydrated.service $backup_dir

cp --parents /etc/systemd/system/cron_daily.timer $backup_dir

# Create Tar.gz
cd $backup_dir && tar -cvzf ../$backup_filename . && cd ..

rm -rf $backup_dir

popd
