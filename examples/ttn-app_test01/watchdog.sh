#!/bin/bash

WD_LOG=/source-code/github.com_stephenbeauchamp/lmic_pi/examples/ttn-app_test01/log.txt
WD_AGE_LIM_SECONDS=30
WD_KILL_SERVICE=ttn-app_test01

echo "=== WATCHDOG: STARTS WATCHING.... "
echo "=== WATCHDOG: STARTS WATCHING.... " >> $WD_LOG

while true; do
  sleep  5
  WD_FLE_AGE_SECONDS=$(($(date +%s) - $(date +%s -r /source-code/github.com_stephenbeauchamp/lmic_pi/examples/ttn-app_test01/log.txt)))
  echo "log file last modified ${WD_FLE_AGE_SECONDS} ago..."
  if  [ "${WD_FLE_AGE_SECONDS}" -gt "${WD_AGE_LIM_SECONDS}"  ]; then
    echo "=== WATCHDOG: GROWL, ${WD_FKE_AGE_SECONDS} SECONDS SINCE LAST LOG MESSAGE..."
    echo "=== WATCHDOG: GROWL, ${WD_FLE_AGE_SECONDS} SECONDS SINCE LAST LOG MESSAGE ..." >> $WD_LOG
    systemctl stop ttn-app_test01
    sleep 5
    pkill ${WD_KILL_SERVICE}
    sleep 5
    systemctl start ttn-app_test01
    echo "=== WATCHDOF: WOOOF (SERVICE RESTARTED)..."
    echo "=== WATCHDOG: WOOOF (SERVICE RESTARTED)..." >> $WD_LOG
  fi
done
