dnl PATH_AVILIB([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for avilib, and define AVILIB_CFLAGS and AVILIB_LIBS
dnl
AC_DEFUN(PATH_AVILIB,
[dnl 
dnl Get the cflags and libraries
dnl

AVILIB_CFLAGS="-Iavilib -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
AVILIB_CXXFLAGS="-Iavilib -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
AVILIB_LIBS="-Lavilib -lavi"

  AC_SUBST(AVILIB_CFLAGS)
  AC_SUBST(AVILIB_CXXFLAGS)
  AC_SUBST(AVILIB_LIBS)
])


# Configure paths for libogg
# Jack Moffitt <jack@icecast.org> 10-21-2000
# Shamelessly stolen from Owen Taylor and Manish Singh

dnl XIPH_PATH_OGG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for libogg, and define OGG_CFLAGS and OGG_LIBS
dnl
AC_DEFUN(XIPH_PATH_OGG,
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(ogg-prefix,[  --with-ogg-prefix=PFX         Prefix where libogg is installed (optional)], ogg_prefix="$withval", ogg_prefix="")
AC_ARG_ENABLE(oggtest, [  --disable-oggtest             Do not try to compile and run a test Ogg program],, enable_oggtest=yes)

  if test "x$ogg_prefix" != "x"; then
    ogg_args="$ogg_args --prefix=$ogg_prefix"
    OGG_CFLAGS="-I$ogg_prefix/include"
    OGG_LIBS="-L$ogg_prefix/lib"
  elif test "x$prefix" != "xNONE"; then
    ogg_args="$ogg_args --prefix=$prefix"
    OGG_CFLAGS="-I$prefix/include"
    OGG_LIBS="-L$prefix/lib"
  fi

  OGG_LIBS="$OGG_LIBS -logg"

  AC_MSG_CHECKING(for Ogg)
  no_ogg=""


  if test "x$enable_oggtest" = "xyes" ; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $OGG_CFLAGS"
    LIBS="$LIBS $OGG_LIBS"
dnl
dnl Now check if the installed Ogg is sufficiently new.
dnl
      rm -f conf.oggtest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogg/ogg.h>

int main ()
{
  system("touch conf.oggtest");
  return 0;
}

],, no_ogg=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_ogg" = "x" ; then
     AC_MSG_RESULT(yes)
     echo '#define HAVE_OGG 1' > config.h
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     echo '/*#define HAVE_OGG 1*/' > config.h
     if test -f conf.oggtest ; then
       :
     else
       echo "*** Could not run Ogg test program, checking why..."
       CFLAGS="$CFLAGS $OGG_CFLAGS"
       LIBS="$LIBS $OGG_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <ogg/ogg.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding Ogg or finding the wrong"
       echo "*** version of Ogg. If it is not finding Ogg, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means Ogg was incorrectly installed"
       echo "*** or that you have moved Ogg since it was installed. In the latter case, you"
       echo "*** may want to edit the ogg-config script: $OGG_CONFIG" ])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     OGG_CFLAGS=""
     OGG_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_SUBST(OGG_CFLAGS)
  AC_SUBST(OGG_LIBS)
  rm -f conf.oggtest
])
# Configure paths for libvorbis
# Jack Moffitt <jack@icecast.org> 10-21-2000
# Shamelessly stolen from Owen Taylor and Manish Singh

dnl XIPH_PATH_VORBIS([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for libvorbis, and define VORBIS_CFLAGS and VORBIS_LIBS
dnl
AC_DEFUN(XIPH_PATH_VORBIS,
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(vorbis-prefix,[  --with-vorbis-prefix=PFX      Prefix where libvorbis is installed (optional)], vorbis_prefix="$withval", vorbis_prefix="")
AC_ARG_ENABLE(vorbistest, [  --disable-vorbistest          Do not try to compile and run a test Vorbis program],, enable_vorbistest=yes)

  if test "x$vorbis_prefix" != "x" ; then
    vorbis_args="$vorbis_args --prefix=$vorbis_prefix"
    VORBIS_CFLAGS="-I$vorbis_prefix/include"
    VORBIS_LIBDIR="-L$vorbis_prefix/lib"
  elif test "x$prefix" != "xNONE"; then
    vorbis_args="$vorbis_args --prefix=$prefix"
    VORBIS_CFLAGS="-I$prefix/include"
    VORBIS_LIBDIR="-L$prefix/lib"
  fi

  VORBIS_LIBS="$VORBIS_LIBDIR -lvorbis -lm"
  VORBISFILE_LIBS="-lvorbisfile"

  AC_MSG_CHECKING(for Vorbis)
  no_vorbis=""


  if test "x$enable_vorbistest" = "xyes" ; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $VORBIS_CFLAGS"
    LIBS="$LIBS $VORBIS_LIBS $OGG_LIBS"
dnl
dnl Now check if the installed Vorbis is sufficiently new.
dnl
      rm -f conf.vorbistest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/codec.h>

int main ()
{
  system("touch conf.vorbistest");
  return 0;
}

],, no_vorbis=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_vorbis" = "x" ; then
     AC_MSG_RESULT(yes)
     echo '#define HAVE_VORBIS 1' >> config.h
     if test "x$no_vorbis" = "x" ; then
        echo '#define HAVE_OGGVORBIS 1' >> config.h
     else
        echo '/*#define HAVE_OGGVORBIS 1*/' >> config.h
     fi
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     echo '/*#define HAVE_VORBIS 1*/' >> config.h
     echo '/*#define HAVE_OGGVORBIS 1*/' >> config.h
     if test -f conf.vorbistest ; then
       :
     else
       echo "*** Could not run Vorbis test program, checking why..."
       CFLAGS="$CFLAGS $VORBIS_CFLAGS"
       LIBS="$LIBS $VORBIS_LIBS $OGG_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <vorbis/codec.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding Vorbis or finding the wrong"
       echo "*** version of Vorbis. If it is not finding Vorbis, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means Vorbis was incorrectly installed"
       echo "*** or that you have moved Vorbis since it was installed." ])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     VORBIS_CFLAGS=""
     VORBIS_LIBS=""
     VORBISFILE_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_SUBST(VORBIS_CFLAGS)
  AC_SUBST(VORBIS_LIBS)
  AC_SUBST(VORBISFILE_LIBS)
  rm -f conf.vorbistest
])

dnl AC_TRY_CFLAGS (CFLAGS, [ACTION-IF-WORKS], [ACTION-IF-FAILS])
dnl check if $CC supports a given set of cflags
AC_DEFUN([AC_TRY_CFLAGS],
    [AC_MSG_CHECKING([if $CC supports $1 flags])
    SAVE_CFLAGS="$CFLAGS"
    CFLAGS="$1"
    AC_TRY_COMPILE([],[],[ac_cv_try_cflags_ok=yes],[ac_cv_try_cflags_ok=no])
    CFLAGS="$SAVE_CFLAGS"
    AC_MSG_RESULT([$ac_cv_try_cflags_ok])
    if test x"$ac_cv_try_cflags_ok" = x"yes"; then
        ifelse([$2],[],[:],[$2])
    else
        ifelse([$3],[],[:],[$3])
    fi])

AC_DEFUN(PATH_DEBUG,[
AC_ARG_ENABLE([debug],
    [  --enable-debug                compile with debug information])
if test x"$enable_debug" = x"yes"; then
    dnl debug information
    DEBUG_CFLAGS="-g"
    echo '#define DEBUG' >> config.h
else
    DEBUG_CFLAGS=""
    echo '/*#define DEBUG*/' >> config.h
fi
  AC_SUBST(DEBUG_CFLAGS)
])

AC_DEFUN(PATH_PROFILING,[
AC_ARG_ENABLE([profiling],
    [  --enable-profiling            compile with profiling information])
if test x"$enable_profiling" = x"yes"; then
    dnl profiling information
    PROFILING_CFLAGS="-pg"
    PROFILING_LIBS="-lc_p"
else
    PROFILING_CFLAGS=""
    PROFILING_LIBS=""
fi
  AC_SUBST(PROFILING_CFLAGS)
  AC_SUBST(PROFILING_LIBS)
])

AC_DEFUN(PATH_DMALLOC,[
AC_ARG_ENABLE([dmalloc],
    [  --enable-dmalloc              link against dmalloc])
if test x"$enable_dmalloc" = x"yes"; then
    dnl debug information
    DMALLOC_CFLAGS="-DDMALLOC"
    DMALLOC_LIBS="-ldmalloc"
else
    DMALLOC_CFLAGS=""
    DMALLOC_LIBS=""
fi
  AC_SUBST(DMALLOC_CFLAGS)
  AC_SUBST(DMALLOC_LIBS)
])

# Configure paths for libebml

dnl PATH_EBML([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for libebml, and define EBML_CFLAGS and EBML_LIBS
dnl
AC_DEFUN(PATH_EBML,
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(ebml-prefix,[  --with-ebml-prefix=PFX        Prefix where libebml is installed (optional)], ebml_prefix="$withval", ebml_prefix="")
AC_ARG_WITH(ebml-include,[  --with-ebml-include=DIR       Path to where the libebml include files installed (optional)], ebml_include="$withval", ebml_include="")
AC_ARG_WITH(ebml-lib,[  --with-ebml-lib=DIR           Path to where the libebml library installed (optional)], ebml_lib="$withval", ebml_lib="")
AC_ARG_ENABLE(ebmltest, [  --disable-ebmltest            Do not try to compile and run a test EBML program],, enable_ebmltest=yes)

  if test "x$ebml_prefix" != "x"; then
    ebml_args="$ebml_args --prefix=$ebml_prefix"
    if test "x$ebml_include" != "x"; then
      EBML_CFLAGS="-I$ebml_include"
    else
      EBML_CFLAGS="-I$ebml_prefix/include"
    fi
    if test "x$ebml_lib" != "x"; then
      EBML_LIBS="-L$ebml_lib"
    else
      EBML_LIBS="-L$ebml_prefix/lib"
    fi
  elif test "x$prefix" != "xNONE"; then
    ebml_args="$ebml_args --prefix=$prefix"
    if test "x$ebml_include" != "x"; then
      EBML_CFLAGS="-I$ebml_include"
    else
      EBML_CFLAGS="-I$prefix/include"
    fi
    if test "x$ebml_lib" != "x"; then
      EBML_LIBS="-L$ebml_lib"
    else
      EBML_LIBS="-L$prefix/lib"
    fi
  else
    if test "x$ebml_include" != "x"; then
      EBML_CFLAGS="-I$ebml_include"
    fi
    if test "x$ebml_lib" != "x"; then
      EBML_LIBS="-L$ebml_lib"
    fi
  fi

  EBML_LIBS="$EBML_LIBS -lebml"

  AC_MSG_CHECKING(for EBML)
  no_ebml=""


  if test "x$enable_ebmltest" = "xyes" ; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $EBML_CFLAGS"
    LIBS="$LIBS $EBML_LIBS"
dnl
dnl Now check if the installed EBML is sufficiently new.
dnl
      rm -f conf.ebmltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <EbmlConfig.h>

int main ()
{
  system("touch conf.ebmltest");
  return 0;
}

],, no_ebml=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_ebml" = "x" ; then
     AC_MSG_RESULT(yes)
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     if test -f conf.ebmltest ; then
       :
     else
       echo "*** Could not run Ebml test program, checking why..."
       CFLAGS="$CFLAGS $EBML_CFLAGS"
       LIBS="$LIBS $EBML_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <EbmlConfig.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding EBML or finding the wrong"
       echo "*** version of EBML. If it is not finding EBML, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means EBML was incorrectly installed"
       echo "*** or that you have moved EBML since it was installed."
       exit 1 ])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     EBML_CFLAGS=""
     EBML_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_SUBST(EBML_CFLAGS)
  AC_SUBST(EBML_LIBS)
  rm -f conf.ebmltest
])

# Configure paths for libmatroska

dnl PATH_MATROSKA([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for libmatroska, and define MATROSKA_CFLAGS and MATROSKA_LIBS
dnl
AC_DEFUN(PATH_MATROSKA,
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(matroska-prefix,[  --with-matroska-prefix=PFX    Prefix where libmatroska is installed (optional)], matroska_prefix="$withval", matroska_prefix="")
AC_ARG_WITH(matroska-include,[  --with-matroska-include=DIR   Path to where the libmatroska include files installed (optional)], matroska_include="$withval", matroska_include="")
AC_ARG_WITH(matroska-lib,[  --with-matroska-lib=DIR       Path to where the libmatroska library installed (optional)], matroska_lib="$withval", matroska_lib="")
AC_ARG_ENABLE(matroskatest, [  --disable-matroskatest        Do not try to compile and run a test Matroska program],, enable_matroskatest=yes)

  if test "x$matroska_prefix" != "x"; then
    matroska_args="$matroska_args --prefix=$matroska_prefix"
    if test "x$matroska_include" != "x"; then
      MATROSKA_CFLAGS="-I$matroska_include"
    else
      MATROSKA_CFLAGS="-I$matroska_prefix/include"
    fi
    if test "x$matroska_lib" != "x"; then
      MATROSKA_LIBS="-L$matroska_lib"
    else
      MATROSKA_LIBS="-L$matroska_prefix/lib"
    fi
  elif test "x$prefix" != "xNONE"; then
    matroska_args="$matroska_args --prefix=$prefix"
    if test "x$matroska_include" != "x"; then
      MATROSKA_CFLAGS="-I$matroska_include"
    else
      MATROSKA_CFLAGS="-I$prefix/include"
    fi
    if test "x$matroska_lib" != "x"; then
      MATROSKA_LIBS="-L$matroska_lib"
    else
      MATROSKA_LIBS="-L$prefix/lib"
    fi
  else
    if test "x$matroska_include" != "x"; then
      MATROSKA_CFLAGS="-I$matroska_include"
    fi
    if test "x$matroska_lib" != "x"; then
      MATROSKA_LIBS="-L$matroska_lib"
    fi
  fi

  MATROSKA_LIBS="$MATROSKA_LIBS -lmatroska"

  AC_MSG_CHECKING(for Matroska)
  no_matroska=""


  if test "x$enable_matroskatest" = "xyes" ; then
    ac_save_CXXFLAGS="$CXXFLAGS"
    ac_save_LIBS="$LIBS"
    CXXFLAGS="$CXXFLAGS $MATROSKA_CFLAGS $EBML_CFLAGS"
    LIBS="$LIBS $MATROSKA_LIBS $EBML_LIBS"
dnl
dnl Now check if the installed Matroska is sufficiently new.
dnl
      rm -f conf.matroskatest
      AC_LANG_PUSH(C++)
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <EbmlConfig.h>
#include <KaxTypes.h>

int main ()
{
  system("touch conf.matroskatest");
  return 0;
}

],, no_matroska=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
      AC_LANG_POP
      CXXFLAGS="$ac_save_CXXFLAGS"
      LIBS="$ac_save_LIBS"
  fi

  if test "x$no_matroska" = "x" ; then
     AC_MSG_RESULT(yes)
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     if test -f conf.matroskatest ; then
       :
     else
       echo "*** Could not run Matroska test program, checking why..."
       CFLAGS="$CFLAGS $MATROSKA_CFLAGS"
       LIBS="$LIBS $MATROSKA_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <EbmlConfig.h>
#include <KaxTypes.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding Matroska or finding the wrong"
       echo "*** version of Matroska. If it is not finding Matroska, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means Matroska was incorrectly installed"
       echo "*** or that you have moved Matroska since it was installed."
       exit 1 ])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     MATROSKA_CFLAGS=""
     MATROSKA_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_SUBST(MATROSKA_CFLAGS)
  AC_SUBST(MATROSKA_LIBS)
  rm -f conf.matroskatest
])

dnl
dnl g++ version test
dnl
AC_DEFUN(PATH_CXXVERSION,
[dnl 
  AC_MSG_CHECKING($CXX version)
  CXXVER="`$CXX --version 2>&1 | head -n 1 | sed 's;.*\([[0-9]][[0-9]]*\)\.[[0-9]][[0-9]]*\.[[0-9]][[0-9]]*.*;\1;'`"
  CXXVER_CFLAGS=
  if test "x$CXXVER" == "x2"; then
    CXXVER_CFLAGS=-DGCC2
    AC_MSG_RESULT(v2)
  elif test "x$CXXVER" != "x3"; then
    AC_MSG_RESULT(unknown)
    echo "*** Unknown C++ compiler or version. Please contact Moritz Bunkus"
    echo "*** <moritz@bunkus.org> if compilation fails."
  else
    AC_MSG_RESULT(v$CXXVER)
  fi
  AC_SUBST(CXXVER_CFLAGS)
])
