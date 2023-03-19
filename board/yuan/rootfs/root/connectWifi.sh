# busbox

# AP list in /etc/wpa_supplicant.conf

enable_rootlogin=`cat /etc/ssh/sshd_config | grep "PermitRootLogin yes"`

if [[ "$enable_rootlogin" == "" ]]
then
    echo "SSH root login is not enabled!"
fi

echo "Connecting Wi-Fi..."
wpa_supplicant -B -d -i wlan0 -c /etc/wpa_supplicant.conf
echo "Getting IP..."
udhcpc -i wlan0

