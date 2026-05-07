 // Application uses overlay where "app_led" is tagged to the original LED on the board. Using that allias controls the LED using the overlay
 
west build l4-task1 -t menuconfig -p always -b disco_l475_iot1 

west build -b disco_l475_iot1 l4-task1 -p always -- -DEXTRA_DTC_OVERLAY_FILE=src/app.overlay

west flash