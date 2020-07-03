#!/usr/bin/python

with open("lglcfg.h", "w") as f:
    f.write("#define LGL_OS \"Windows\"\n")
    f.write("#define LGL_COMPILED \"Date\"\n")
    f.write("#define LGL_RELEASED \"Date\"\n")
    f.write("#define LGL_VERSION  \"Version\"\n")
    f.write("#define LGL_ID  \"ID\"\n")


