win32:deploy {
    CONFIG(debug, debug|release): BUILD_TYPE = "debug"
    else:                         BUILD_TYPE = "release"

    # Make deploy directory
    DEPLOY_DIR = $$shell_path($${OUT_PWD}/../../RTKLib_winapp_qt)
    DEPLOY_TARGET = $$shell_path($${OUT_PWD}/$${BUILD_TYPE}/$${TARGET}.exe)

    mk_deploy_dir.commands = $$sprintf($${QMAKE_MKDIR_CMD}, $$shell_path($${DEPLOY_DIR}))
    QMAKE_EXTRA_TARGETS += mk_deploy_dir
    POST_TARGETDEPS += mk_deploy_dir

    #Copy binary file
    QMAKE_POST_LINK += $$QMAKE_COPY $$quote($${DEPLOY_TARGET}) \
                                    $$quote($${DEPLOY_DIR})    \
                                    $$escape_expand(\\n\\t)

    # Deploy
    QT_BIN_DIR = $$dirname(QMAKE_QMAKE)
    WIN_DEPLOY_QT = $$shell_path($${QT_BIN_DIR}/windeployqt)
    WIN_DEPLOY_QT_CONFIG +=                    \
            --compiler-runtime                 \
            --dir=$$shell_path($${DEPLOY_DIR}) \ # destination Qt libs
            --$${BUILD_TYPE}                   \ # build type of qt libs
            $$shell_path($${DEPLOY_TARGET})      # path to main binary (for extracting dependecies)

    QMAKE_POST_LINK += $${WIN_DEPLOY_QT} $${WIN_DEPLOY_QT_CONFIG} $$escape_expand(\\n\\t)
}
