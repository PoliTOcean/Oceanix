if ! pgrep -x "mosquitto" > /dev/null
then
  echo "Mosquitto is not running. Starting mosquitto..."
  mosquitto -d
else
  echo "Mosquitto is already running."
fi
./build/Oceanix test