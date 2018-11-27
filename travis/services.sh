# Disable services enabled by default
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo /etc/init.d/postgresql stop
    sudo /etc/init.d/mysql stop
fi
