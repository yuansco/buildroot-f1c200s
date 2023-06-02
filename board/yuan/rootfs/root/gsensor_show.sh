# busybox
# gsensor_show.sh
# Created by Yu-An Chen on 2023/06/19
# Last modified on 2023/06/19
# Vertion: 1.0
#

VERSION="1.0"                # script version
LAST_MODIFIED="2023/06/19"   # last modified date

HELP_STRING="
show g-sensor value and motion angle
Created by Yu-An Chen on 2023/06/19
Last modified on $LAST_MODIFIED
Vertion: $VERSION

Usage:
 gsensor_show.sh --loop --oled_show

Options:
 -e, --echo           print g-sensor value and motion angle on terminal
 -h, --help           print help message
 -l, --loop           continue print g-sensor value and motion angle
 -o, --oled-show      print g-sensor value and motion angle on oled module
 -v, --version        print script version
"


# process argument
for i in "$@"
do
  # loop all argument
  case $i in
  -h|-H|help|--help)
    echo "$HELP_STRING"
    exit 0
  ;;
  -v|-V|version|--version)
    echo "Ver $VERSION"
    exit 0
  ;;
  -e|--echo)
    ECHO_MODE="Enable"
  ;;
  -o|--oled-show)
    OLED_SHOW="Enable"
  ;;
  -l|--loop)
    LOOP_MODE="Enable"
  ;;
  *)
    # Unknown argument
    echo "Unknown argument: $i"
  ;;
  esac
done

IIO_DEVICE="/sys/bus/iio/devices/iio:device0/sampling_frequency"

if [[ ! -f "$IIO_DEVICE" ]]
then
  echo "IIO Device 0 is not exist!"
  exit 0
fi


if [[ "$OLED_SHOW" == "Enable" ]]
then
  ssd1306_bin -c
fi


motion_angle -f 100

while :
do
  data=$(motion_angle -a)

  angle=$(echo $data |cut -d ':' -f 1)
  xyz_value=$(echo $data |cut -d ':' -f 2)

  if [[ "$angle" == "500" ]]
  then
    angle="unreliable"
  fi

  X=$(echo $xyz_value |cut -d '=' -f 2|cut -d ',' -f 1)
  Y=$(echo $xyz_value |cut -d '=' -f 3|cut -d ',' -f 1)
  Z=$(echo $xyz_value |cut -d '=' -f 4 )

  if [[ "$ECHO_MODE" == "Enable" ]]
  then
    echo "Angle: $angle, X= $X, Y= $Y, Z= $Z"
  fi

  if [[ "$OLED_SHOW" == "Enable" ]]
  then
    ssd1306_bin -m "Angle: $angle   \nX= $X       \nY= $Y         \nZ= $Z         \n"
  fi

  if [[ "$LOOP_MODE" != "Enable" ]]
  then
    exit 0
  fi

  sleep 0.3
done

