# Disable services enabled by default
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo service 
    sudo service postgresql stop
    sudo service mysql stop
fi
#list status all services
if [ "$OWTRAVIS_DEBUG" = "1" ]; then
    sudo initctl list
    service --status-all
fi
