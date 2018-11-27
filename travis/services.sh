# Disable services enabled by default
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo service 
    sudo service postgresql stop
    sudo service mysql stop
fi
# Display services informations
if [ "$OWTRAVIS_DEBUG" = "1" ]; then
    if [ "$TRAVIS_OS_NAME" = "linux" ]; then
        sudo initctl list
        service --status-all
    elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
        sudo launchctl list
        sudo top -l 1
    fi
fi
