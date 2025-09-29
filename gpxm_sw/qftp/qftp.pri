

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -lQt5Ftp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -lQt5Ftpd
else:unix:!macx: LIBS += -L$$PWD/ -lQt5Ftp


