QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addcourse.cpp \
    admin.cpp \
    adminlogin.cpp \
    assignteacher.cpp \
    firstlogin.cpp \
    firstlogins.cpp \
    lms.cpp \
    login.cpp \
    logins.cpp \
    main.cpp \
    remove.cpp \
    removecourse.cpp \
    removet.cpp \
    student.cpp \
    sudentdetails.cpp \
    teacher.cpp \
    teacherdetails.cpp

HEADERS += \
    addcourse.h \
    admin.h \
    adminlogin.h \
    assignteacher.h \
    firstlogin.h \
    firstlogins.h \
    lms.h \
    login.h \
    logins.h \
    remove.h \
    removecourse.h \
    removet.h \
    student.h \
    studentdetail.h \
    sudentdetails.h \
    teacher.h \
    teacherdetails.h

FORMS += \
    addcourse.ui \
    admin.ui \
    adminlogin.ui \
    assignteacher.ui \
    firstlogin.ui \
    firstlogins.ui \
    lms.ui \
    login.ui \
    logins.ui \
    remove.ui \
    removecourse.ui \
    removet.ui \
    student.ui \
    sudentdetails.ui \
    teacher.ui \
    teacherdetails.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
