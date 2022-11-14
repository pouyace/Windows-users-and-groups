defineTest(copyFile) {
    !isEmpty(xd_copyfile.commands): xd_copyfile.commands += && \\$$escape_expand(\n\t)
    xd_copyfile.commands += ( $(COPY_FILE) \"$$shell_path($$1)\" \"$$shell_path($$2)\" || echo \"copy failed\" )
    first.depends *= xd_copyfile
    QMAKE_EXTRA_TARGETS *= first xd_copyfile

    export(first.depends)
    export(xd_copyfile.commands)
    export(QMAKE_EXTRA_TARGETS)
}

defineTest(copyDir) {
    !isEmpty(xd_copydir.commands): xd_copydir.commands += && \\$$escape_expand(\n\t)
    xd_copydir.commands += ( $(COPY_DIR) \"$$shell_path($$1)\" \"$$shell_path($$2)\" || echo \"copy failed\" )
    first.depends *= xd_copydir
    QMAKE_EXTRA_TARGETS *= first xd_copydir

    export(first.depends)
    export(xd_copydir.commands)
    export(QMAKE_EXTRA_TARGETS)
}

defineTest(placeInstalls){
    win32:for(item,INSTALLS){
        ml = $${item}.path
        ml2 = $$dirname($$ml)/$$basename($$ml)
        mkpath($$ml2)
        for(filePath,$${item}.files){
            srcFile = $$absolute_path($$filePath)
            dstFile = $$ml2/$$basename(filePath)
            copyFile($$srcFile,$$dstFile)
        }
    }
}
