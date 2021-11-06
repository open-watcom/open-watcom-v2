struct netent {
    char        *n_name;        /* official network name */
    char        **n_aliases;    /* alias list            */
    int         n_addrtype;     /* address type          */
    uint32_t    n_net;          /* network number        */
};

