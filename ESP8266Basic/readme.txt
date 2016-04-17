
new commands implemented:

- neopixel on pin 2
add of the command :
serial2begin speed - activate serial 2 output only on pin 2   (ex serial2begin 115200)
serial2print
serial2println

readopenweather(api_url, index) : calibrated for the Weatherwebapi forecast, permit to choose the index from the list when several lines (message is too big)

jasone(json_string,message_to_find) : ex jasone(buff,"temp.max")

unixtime(time_in_unix_format) : decode the unix date time (used by openweatherapi)

the basic files can be updated directly from the file manager (default.bas)

speedup (5 x faster)

save faster

open don't fails anymore

