:DT.date_form
:I2 refid='ladate_'.ladate_
:DD.The date_form attribute accepts a character string value
    which defines the format of the date string.
    The year, month and day may be specified separately
    and in any order by special date sequences.
    These date sequences are started with a dollar($) sign and followed
    by one to three characters.
    Text which is not recognized as a date sequence can be entered
    to tailor the format of the resulting date.
    :P.
    The first character in a date sequence is a
    :HP1.Y:eHP1. for the year,
    an :HP1.M:eHP1. for the month, or a
    :HP1.D:eHP1. for the day.
    The next character
    is the :HP1.L:eHP1. or :HP1.S:eHP1. character to specify the long
    or short form of the date sequence.
    If neither of these characters are present, the long form is used.
    When the length specifier is present, the :HP1.N:eHP1. character
    is used to format the month or the day as a number.
    If the length specified is not present, the month and
    day values are created in character form.
    The year is always formatted as a number.
