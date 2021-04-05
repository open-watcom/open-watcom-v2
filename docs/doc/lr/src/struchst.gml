struct hostent {
    char    *h_name;       /* host official name               */
    char    **h_aliases;   /* host alternate names, up to 16,
                              terminated by a NULL pointer     */
    int     h_length;      /* address length in bytes          */
    char    **h_addr_list; /* array of pointers to network
                              addresses in network byte order,
                              terminated by a NULL pointer     */
};

