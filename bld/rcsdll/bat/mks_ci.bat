@echo off
@rem name=%1, file contatining message=%2, project=%3, target=%4
ci -u -y! %1 < %2
