struct servent {
    char    *s_name;        /* official service name */
    char    **s_aliases;    /* alias list            */
    int     s_port;         /* port number           */
    char    *s_proto;       /* protocol to use       */
};

