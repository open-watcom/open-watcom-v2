.386
.model small
zero = 0
one = 1
foo equ "hello"
bar equ "hello"
sam equ "HeLlo"
blank equ ""
.code
    .err
    .erre zero
    .erre one
    .errnz zero
    .errnz one
    .errdef zero
    .errdef two
    .errndef zero
    .errndef two
    .errb <zero>
    .errb <>
    .errb <blank>
    .errnb <zero>
    .errnb <>
    .errnb <blank>
    .errdif <foo>, <bar>
    .errdif foo, bar
    .erridn <foo>, <bar>
    .erridn foo, bar
    .errdif <foo>, <sam>
    .errdif foo, sam
    .erridn <foo>, <sam>
    .erridn foo, sam
    .errdifi <foo>, <sam>
    .errdifi foo, sam
    .erridni <foo>, <sam>
    .erridni foo, sam

    include a.inc

    end
