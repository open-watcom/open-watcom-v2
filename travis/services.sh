# Disable services enabled by default
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo service postgresql stop
    sudo service mysql stop
fi
# Display services informations
if [ "$OWTRAVIS_DEBUG" = "1" ]; then
    if [ "$TRAVIS_OS_NAME" = "linux" ]; then
        echo "'sudo initctl list'"
        sudo initctl list
        echo "'service --status-all'"
        service --status-all
    elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
        echo "'sudo launchctl list'"
        sudo launchctl list
        echo "'launchctl list'"
        launchctl list
        echo "'sudo top -l 1 -o MEM'"
        sudo top -l 1 -o MEM
    fi
fi
