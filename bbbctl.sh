#!/bin/bash

# Common path for all GPIO access
BASE_GPIO_PATH=/sys/class/gpio

# Assign names to GPIO pin numbers for each pin
PWR_BUT=23
BOOT_MODE=24

# Assign names to states
ON="1"
OFF="0"

# Utility function to export a pin if not already exported
exportPin()
{
  if [ ! -e $BASE_GPIO_PATH/gpio$1 ]; then
    echo "$1" > $BASE_GPIO_PATH/export
  fi
}

# Utility function to set a pin as an output
setOutput()
{
  echo "out" > $BASE_GPIO_PATH/gpio$1/direction
}

# Utility function to change state of a pin
setPinState()
{
  echo $2 > $BASE_GPIO_PATH/gpio$1/value
}

# Utility function to turn all lights off
allPinsOn()
{
  setPinState $BOOT_MODE $ON
  setPinState $PWR_BUT $ON
}

# Ctrl-C handler for clean shutdown
shutdown()
{
  allPinsOn
  exit 0
}

trap shutdown SIGINT

# Export pins so that we can use them
exportPin $PWR_BUT
exportPin $BOOT_MODE

if [ $1 == "init" ]
then
    # Set pins as outputs
    setOutput $PWR_BUT
    setOutput $BOOT_MODE

    allPinsOn
    echo "Initialization Done"
elif [ $1 == "reset" ]
then
    echo "PWR_BUT: ON"
    setPinState $PWR_BUT $OFF
    echo "Wait 16 seconds"
    sleep 16
    echo "PWR_BUT: OFF"
    setPinState $PWR_BUT $ON
    echo "Reset Performed"
elif [ $1 == "powerdown" ]
then
    echo "PWR_BUT: ON"
    setPinState $PWR_BUT $OFF
    echo "Wait 10 seconds"
    sleep 10
    echo "PWR_BUT: OFF"
    setPinState $PWR_BUT $ON
    echo "PowerDown Performed"
elif [ $1 == "powerup" ]
then
    echo "PWR_BUT: ON"
    setPinState $PWR_BUT $OFF
    echo "Wait 1 seconds"
    sleep 1
    echo "PWR_BUT: OFF"
    setPinState $PWR_BUT $ON
    echo "PowerUp Performed"
elif [ $1 == "sdboot" ]
then
    echo "BOOT Button: OFF"
    setPinState $BOOT_MODE $OFF
    echo "BOOT MODE: External SD card"
elif [ $1 == "emmcboot" ]
then
    echo "BOOT Button: ON"
    setPinState $BOOT_MODE $ON
    echo "BOOT MODE: EMMC"
else
    echo "Error: Unkown Command"
    exit 1
fi
