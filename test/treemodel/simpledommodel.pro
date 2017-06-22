HEADERS     = domitem.h      \
              dommodel.h     \
              mainwindow.h
SOURCES     = domitem.cpp    \
              dommodel.cpp   \
              main.cpp       \
              mainwindow.cpp
LIBS        = -L$(QTDIR)     \
              -lQt5Widgets   \
              -lQt5Gui       \
              -lQt5Xml       \
              -lQt5Core      \
              -lQt5OpenGL

CONFIG  += qt
QT      += xml

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/simpledommodel
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/simpledommodel
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

