:DT.level
:I2 refid='lalevel'.lalevel
:DD.This attribute accepts a positive integer number.
    If not specified, a level value of '1'. is assumed.
    Each list level is separately specified.
    For example, if two
    levels of the ordered list are specified,
    the :HP2.&gml.&loc2.:eHP2. tag will be
    specified twice in the layout.
    When some attributes for a new level of a list are not specified,
    the default
    values for those attributes will be the values of the
    first level.
    Since list levels may not be skipped,
    each new level of list must be sequentially defined from
    the last specified level.
:P.
    If there is an ordered, simple, and second ordered
    list nested together in the document,
    the simple and first ordered list will both be
    from level one, while the last ordered list will be level two.
    The appropriate level number is selected based on the nesting level
    of a particular list type.
    If a list type is nested beyond the levels specified in the layout,
    the levels are "cycled".
    For example, if there are two levels of ordered list specified in
    the layout, and there are three ordered lists nested, the third
    level of ordered list will use the attributes of the level one
    ordered list.  A fourth nested list would use the attributes of
    the level two.
