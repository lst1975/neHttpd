/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 **************************************************************************************
 *                              
 *                    https://github.com/lst1975/TDMA-ng-Wifi
 *                              
 **************************************************************************************
 */
/*************************************************************************************
 *                               ngRTOS Kernel V2.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 *************************************************************************************
 *                              https://github.com/lst1975/ngRTOS
 *                              https://github.com/lst1975/neHttpd
 **************************************************************************************
 */
/* 
 * Copyright (c) 2016 Lammert Bies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ============
 * Release: 2.0
 */

#include "nanohttp-defs.h"
#include "nanohttp-buffer.h"
#include "nanohttp-list.h"
#include "nanohttp-error.h"
#include "nanohttp-logging.h"

struct _mime_types {
	ng_block_s extension;
	ng_block_s mime_type;
  ng_list_head_s link;
};
typedef struct _mime_types mime_types_s;
#define _MIME_HASH_TBLSZ 1024
#define _MIME_HASH_MASK  1023

static const ng_block_s _mime_type_default = DECL_CONST_STR("application/octet-stream");
static ng_list_head_s _mime_types_htbl[_MIME_HASH_TBLSZ];
static size_t NUM_MIME_TYPES;		

#define __E(x) .extension = DECL_CONST_STR(x)
#define __T(x) .mime_type = DECL_CONST_STR(x)

static mime_types_s _mime_types_names[] = {
  
  { __E(".3dm"),   __T("x-world/x-3dmf")},
  { __E(".3dmf"),  __T("x-world/x-3dmf")},

  { __E(".a"),     __T("application/octet-stream")},
  { __E(".aab"),   __T("application/x-authorware-bin")},
  { __E(".aac"),   __T("audio/aac")},
  { __E(".aam"),   __T("application/x-authorware-map")},
  { __E(".aas"),   __T("application/x-authorware-seg")},
  { __E(".aat"),   __T("application/font-sfnt")},
  { __E(".abc"),   __T("text/vnd.abc")},
  { __E(".acgi"),  __T("text/html")},
  { __E(".afl"),   __T("video/animaflex")},
  { __E(".ai"),    __T("application/postscript")},
  { __E(".aif"),   __T("audio/x-aiff")},
  { __E(".aifc"),  __T("audio/x-aiff")},
  { __E(".aiff"),  __T("audio/x-aiff")},
  { __E(".aim"),   __T("application/x-aim")},
  { __E(".aip"),   __T("text/x-audiosoft-intra")},
  { __E(".ani"),   __T("application/x-navi-animation")},
  { __E(".aos"),   __T("application/x-nokia-9000-communicator-add-on-software")},
  { __E(".aps"),   __T("application/mime")},
  { __E(".arc"),   __T("application/octet-stream")},
  { __E(".arj"),   __T("application/arj")},
  { __E(".art"),   __T("image/x-jg")},
  { __E(".asf"),   __T("video/x-ms-asf")},
  { __E(".asm"),   __T("text/x-asm")},
  { __E(".asp"),   __T("text/asp")},
  { __E(".asx"),   __T("video/x-ms-asf")},
  { __E(".au"),    __T("audio/x-au")},
  { __E(".avi"),   __T("video/x-msvideo")},
  { __E(".avs"),   __T("video/avs-video")},

  { __E(".bcpio"), __T("application/x-bcpio")},
  { __E(".bin"),   __T("application/x-binary")},
  { __E(".bm"),    __T("image/bmp")},
  { __E(".bmp"),   __T("image/bmp")},
  { __E(".boo"),   __T("application/book")},
  { __E(".book"),  __T("application/book")},
  { __E(".boz"),   __T("application/x-bzip2")},
  { __E(".bsh"),   __T("application/x-bsh")},
  { __E(".bz"),    __T("application/x-bzip")},
  { __E(".bz2"),   __T("application/x-bzip2")},

  { __E(".c"),     __T("text/x-c")},
  { __E(".c++"),   __T("text/x-c")},
  { __E(".cat"),   __T("application/vnd.ms-pki.seccat")},
  { __E(".cc"),    __T("text/x-c")},
  { __E(".ccad"),  __T("application/clariscad")},
  { __E(".cco"),   __T("application/x-cocoa")},
  { __E(".cdf"),   __T("application/x-cdf")},
  { __E(".cer"),   __T("application/pkix-cert")},
  { __E(".cff"),   __T("application/font-sfnt")},
  { __E(".cha"),   __T("application/x-chat")},
  { __E(".chat"),  __T("application/x-chat")},
  { __E(".class"), __T("application/x-java-class")},
  { __E(".com"),   __T("application/octet-stream")},
  { __E(".conf"),  __T("text/plain")},
  { __E(".cpio"),  __T("application/x-cpio")},
  { __E(".cpp"),   __T("text/x-c")},
  { __E(".cpt"),   __T("application/x-compactpro")},
  { __E(".crl"),   __T("application/pkcs-crl")},
  { __E(".crt"),   __T("application/x-x509-user-cert")},
  { __E(".csh"),   __T("text/x-script.csh")},
  { __E(".css"),   __T("text/css")},
  { __E(".csv"),   __T("text/csv")},
  { __E(".cxx"),   __T("text/plain")},

  { __E(".dcr"),   __T("application/x-director")},
  { __E(".deepv"), __T("application/x-deepv")},
  { __E(".def"),   __T("text/plain")},
  { __E(".der"),   __T("application/x-x509-ca-cert")},
  { __E(".dif"),   __T("video/x-dv")},
  { __E(".dir"),   __T("application/x-director")},
  { __E(".dl"),    __T("video/x-dl")},
  { __E(".dll"),   __T("application/octet-stream")},
  { __E(".doc"),   __T("application/msword")},
  { __E(".dot"),   __T("application/msword")},
  { __E(".dp"),    __T("application/commonground")},
  { __E(".drw"),   __T("application/drafting")},
  { __E(".dump"),  __T("application/octet-stream")},
  { __E(".dv"),    __T("video/x-dv")},
  { __E(".dvi"),   __T("application/x-dvi")},
  { __E(".dwf"),   __T("model/vnd.dwf")},
  { __E(".dwg"),   __T("image/vnd.dwg")},
  { __E(".dxf"),   __T("image/vnd.dwg")},
  { __E(".dxr"),   __T("application/x-director")},

  { __E(".el"),    __T("text/x-script.elisp")},
  { __E(".elc"),   __T("application/x-bytecode.elisp")},
  { __E(".env"),   __T("application/x-envoy")},
  { __E(".eps"),   __T("application/postscript")},
  { __E(".es"),    __T("application/x-esrehber")},
  { __E(".etx"),   __T("text/x-setext")},
  { __E(".evy"),   __T("application/x-envoy")},
  { __E(".exe"),   __T("application/octet-stream")},

  { __E(".f"),     __T("text/x-fortran")},
  { __E(".f77"),   __T("text/x-fortran")},
  { __E(".f90"),   __T("text/x-fortran")},
  { __E(".fdf"),   __T("application/vnd.fdf")},
  { __E(".fif"),   __T("image/fif")},
  { __E(".fli"),   __T("video/x-fli")},
  { __E(".flo"),   __T("image/florian")},
  { __E(".flx"),   __T("text/vnd.fmi.flexstor")},
  { __E(".fmf"),   __T("video/x-atomic3d-feature")},
  { __E(".for"),   __T("text/x-fortran")},
  { __E(".fpx"),   __T("image/vnd.fpx")},
  { __E(".frl"),   __T("application/freeloader")},
  { __E(".funk"),  __T("audio/make")},

  { __E(".g"),     __T("text/plain")},
  { __E(".g3"),    __T("image/g3fax")},
  { __E(".gif"),   __T("image/gif")},
  { __E(".gl"),    __T("video/x-gl")},
  { __E(".gsd"),   __T("audio/x-gsm")},
  { __E(".gsm"),   __T("audio/x-gsm")},
  { __E(".gsp"),   __T("application/x-gsp")},
  { __E(".gss"),   __T("application/x-gss")},
  { __E(".gtar"),  __T("application/x-gtar")},
  { __E(".gz"),    __T("application/x-gzip")},

  { __E(".h"),     __T("text/x-h")},
  { __E(".hdf"),   __T("application/x-hdf")},
  { __E(".help"),  __T("application/x-helpfile")},
  { __E(".hgl"),   __T("application/vnd.hp-hpgl")},
  { __E(".hh"),    __T("text/x-h")},
  { __E(".hlb"),   __T("text/x-script")},
  { __E(".hlp"),   __T("application/x-helpfile")},
  { __E(".hpg"),   __T("application/vnd.hp-hpgl")},
  { __E(".hpgl"),  __T("application/vnd.hp-hpgl")},
  { __E(".hqx"),   __T("application/binhex")},
  { __E(".hta"),   __T("application/hta")},
  { __E(".htc"),   __T("text/x-component")},
  { __E(".htm"),   __T("text/html")},
  { __E(".html"),  __T("text/html")},
  { __E(".htmls"), __T("text/html")},
  { __E(".htt"),   __T("text/webviewhtml")},
  { __E(".htx"),   __T("text/html")},

  { __E(".ice"),   __T("x-conference/x-cooltalk")},
  { __E(".ico"),   __T("image/x-icon")},
  { __E(".idc"),   __T("text/plain")},
  { __E(".ief"),   __T("image/ief")},
  { __E(".iefs"),  __T("image/ief")},
  { __E(".iges"),  __T("model/iges")},
  { __E(".igs"),   __T("model/iges")},
  { __E(".ima"),   __T("application/x-ima")},
  { __E(".imap"),  __T("application/x-httpd-imap")},
  { __E(".inf"),   __T("application/inf")},
  { __E(".ins"),   __T("application/x-internett-signup")},
  { __E(".ip"),    __T("application/x-ip2")},
  { __E(".isu"),   __T("video/x-isvideo")},
  { __E(".it"),    __T("audio/it")},
  { __E(".iv"),    __T("application/x-inventor")},
  { __E(".ivr"),   __T("i-world/i-vrml")},
  { __E(".ivy"),   __T("application/x-livescreen")},

  { __E(".jam"),   __T("audio/x-jam")},
  { __E(".jav"),   __T("text/x-java-source")},
  { __E(".java"),  __T("text/x-java-source")},
  { __E(".jcm"),   __T("application/x-java-commerce")},
  { __E(".jfif"),   __T("image/jpeg")},
  { __E(".jfif-tbnl"),   __T("image/jpeg")},
  { __E(".jpe"),   __T("image/jpeg")},
  { __E(".jpeg"),  __T("image/jpeg")},
  { __E(".jpg"),   __T("image/jpeg")},
  { __E(".jpm"),   __T("image/jpm")},
  { __E(".jps"),   __T("image/x-jps")},
  { __E(".jpx"),   __T("image/jpx")},
  { __E(".js"),    __T("application/x-javascript")},
  { __E(".json"),  __T("application/json")},
  { __E(".jut"),   __T("image/jutvision")},

  { __E(".kar"),   __T("music/x-karaoke")},
  { __E(".kml"),   __T("application/vnd.google-earth.kml+xml")},
  { __E(".kmz"),   __T("application/vnd.google-earth.kmz")},
  { __E(".ksh"),   __T("text/x-script.ksh")},

  { __E(".la"),    __T("audio/x-nspaudio")},
  { __E(".lam"),   __T("audio/x-liveaudio")},
  { __E(".latex"), __T("application/x-latex")},
  { __E(".lha"),   __T("application/x-lha")},
  { __E(".lhx"),   __T("application/octet-stream")},
  { __E(".lib"),   __T("application/octet-stream")},
  { __E(".list"),  __T("text/plain")},
  { __E(".lma"),   __T("audio/x-nspaudio")},
  { __E(".log"),   __T("text/plain")},
  { __E(".lsp"),   __T("text/x-script.lisp")},
  { __E(".lst"),   __T("text/plain")},
  { __E(".lsx"),   __T("text/x-la-asf")},
  { __E(".ltx"),   __T("application/x-latex")},
  { __E(".lzh"),   __T("application/x-lzh")},
  { __E(".lzx"),   __T("application/x-lzx")},

  { __E(".m"),     __T("text/x-m")},
  { __E(".m1v"),   __T("video/mpeg")},
  { __E(".m2a"),   __T("audio/mpeg")},
  { __E(".m2v"),   __T("video/mpeg")},
  { __E(".m3u"),   __T("audio/x-mpegurl")},
  { __E(".m4v"),   __T("video/x-m4v")},
  { __E(".man"),   __T("application/x-troff-man")},
  { __E(".map"),   __T("application/x-navimap")},
  { __E(".mar"),   __T("text/plain")},
  { __E(".mbd"),   __T("application/mbedlet")},
  { __E(".mc$"),   __T("application/x-magic-cap-package-1.0")},
  { __E(".mcd"),   __T("application/x-mathcad")},
  { __E(".mcf"),   __T("text/mcf")},
  { __E(".mcp"),   __T("application/netmc")},
  { __E(".me"),    __T("application/x-troff-me")},
  { __E(".mht"),   __T("message/rfc822")},
  { __E(".mhtml"), __T("message/rfc822")},
  { __E(".mid"),   __T("audio/x-midi")},
  { __E(".midi"),  __T("audio/x-midi")},
  { __E(".mif"),   __T("application/x-mif")},
  { __E(".mime"),  __T("www/mime")},
  { __E(".mjf"),   __T("audio/x-vnd.audioexplosion.mjuicemediafile")},
  { __E(".mjpg"),  __T("video/x-motion-jpeg")},
  { __E(".mm"),    __T("application/base64")},
  { __E(".mme"),   __T("application/base64")},
  { __E(".mod"),   __T("audio/x-mod")},
  { __E(".moov"),  __T("video/quicktime")},
  { __E(".mov"),   __T("video/quicktime")},
  { __E(".movie"), __T("video/x-sgi-movie")},
  { __E(".mp2"),   __T("video/x-mpeg")},
  { __E(".mp3"),   __T("audio/x-mpeg-3")},
  { __E(".mp4"),   __T("video/mp4")},
  { __E(".mpa"),   __T("audio/mpeg")},
  { __E(".mpc"),   __T("application/x-project")},
  { __E(".mpeg"),  __T("video/mpeg")},
  { __E(".mpg"),   __T("video/mpeg")},
  { __E(".mpga"),  __T("audio/mpeg")},
  { __E(".mpp"),   __T("application/vnd.ms-project")},
  { __E(".mpt"),   __T("application/x-project")},
  { __E(".mpv"),   __T("application/x-project")},
  { __E(".mpx"),   __T("application/x-project")},
  { __E(".mrc"),   __T("application/marc")},
  { __E(".ms"),    __T("application/x-troff-ms")},
  { __E(".mv"),    __T("video/x-sgi-movie")},
  { __E(".my"),    __T("audio/make")},
  { __E(".mzz"),   __T("application/x-vnd.audioexplosion.mzz")},

  { __E(".nap"),   __T("image/naplps")},
  { __E(".naplps"),__T("image/naplps")},
  { __E(".nc"),    __T("application/x-netcdf")},
  { __E(".ncm"),   __T("application/vnd.nokia.configuration-message")},
  { __E(".nif"),   __T("image/x-niff")},
  { __E(".niff"),   __T("image/x-niff")},
  { __E(".nix"),   __T("application/x-mix-transfer")},
  { __E(".nsc"),   __T("application/x-conference")},
  { __E(".nvd"),   __T("application/x-navidoc")},

  { __E(".o"),     __T("application/octet-stream")},
  { __E(".obj"),   __T("application/octet-stream")},
  { __E(".oda"),   __T("application/oda")},
  { __E(".oga"),   __T("audio/ogg")},
  { __E(".ogg"),   __T("audio/ogg")},
  { __E(".ogv"),   __T("video/ogg")},
  { __E(".omc"),   __T("application/x-omc")},
  { __E(".omcd"),  __T("application/x-omcdatamaker")},
  { __E(".omcr"),  __T("application/x-omcregerator")},
  { __E(".otf"),   __T("application/font-sfnt")},

  { __E(".p"),     __T("text/x-pascal")},
  { __E(".p10"),   __T("application/x-pkcs10")},
  { __E(".p12"),   __T("application/x-pkcs12")},
  { __E(".p7a"),   __T("application/x-pkcs7-signature")},
  { __E(".p7c"),   __T("application/x-pkcs7-mime")},
  { __E(".p7m"),   __T("application/x-pkcs7-mime")},
  { __E(".p7r"),   __T("application/x-pkcs7-certreqresp")},
  { __E(".p7s"),   __T("application/pkcs7-signature")},
  { __E(".part"),  __T("application/pro_eng")},
  { __E(".pas"),   __T("text/x-pascal")},
  { __E(".pbm"),   __T("image/x-portable-bitmap")},
  { __E(".pcl"),   __T("application/vnd.hp-pcl")},
  { __E(".pct"),   __T("image/x-pct")},
  { __E(".pcx"),   __T("image/x-pcx")},
  { __E(".pdb"),   __T("chemical/x-pdb")},
  { __E(".pdf"),   __T("application/pdf")},
  { __E(".pfr"),   __T("application/font-tdpfr")},
  { __E(".pfunk"), __T("audio/make")},
  { __E(".pgm"),   __T("image/x-portable-greymap")},
  { __E(".pic"),   __T("image/pict")},
  { __E(".pict"),  __T("image/pict")},
  { __E(".pkg"),   __T("application/x-newton-compatible-pkg")},
  { __E(".pko"),   __T("application/vnd.ms-pki.pko")},
  { __E(".pl"),    __T("text/x-script.perl")},
  { __E(".plx"),   __T("application/x-pixelscript")},
  { __E(".pm"),    __T("text/x-script.perl-module")},
  { __E(".pm4"),   __T("application/x-pagemaker")},
  { __E(".pm5"),   __T("application/x-pagemaker")},
  { __E(".png"),   __T("image/png")},
  { __E(".pnm"),   __T("image/x-portable-anymap")},
  { __E(".pot"),   __T("application/vnd.ms-powerpoint")},
  { __E(".pov"),   __T("model/x-pov")},
  { __E(".ppa"),   __T("application/vnd.ms-powerpoint")},
  { __E(".ppm"),   __T("image/x-portable-pixmap")},
  { __E(".pps"),   __T("application/vnd.ms-powerpoint")},
  { __E(".ppt"),   __T("application/vnd.ms-powerpoint")},
  { __E(".ppz"),   __T("application/vnd.ms-powerpoint")},
  { __E(".pre"),   __T("application/x-freelance")},
  { __E(".prt"),   __T("application/pro_eng")},
  { __E(".ps"),    __T("application/postscript")},
  { __E(".psd"),   __T("application/octet-stream")},
  { __E(".pvu"),   __T("paleovu/x-pv")},
  { __E(".pwz"),   __T("application/vnd.ms-powerpoint")},
  { __E(".py"),    __T("text/x-script.python")},
  { __E(".pyc"),   __T("application/x-bytecode.python")},

  { __E(".qcp"),    __T("audio/vnd.qcelp")},
  { __E(".qd3"),    __T("x-world/x-3dmf")},
  { __E(".qd3d"),   __T("x-world/x-3dmf")},
  { __E(".qif"),    __T("image/x-quicktime")},
  { __E(".qt"),     __T("video/quicktime")},
  { __E(".qtc"),    __T("video/x-qtc")},
  { __E(".qti"),    __T("image/x-quicktime")},
  { __E(".qtif"),   __T("image/x-quicktime")},

  { __E(".ra"),     __T("audio/x-pn-realaudio")},
  { __E(".ram"),    __T("audio/x-pn-realaudio")},
  { __E(".rar"),    __T("application/x-arj-compressed")},
  { __E(".ras"),    __T("image/x-cmu-raster")},
  { __E(".rast"),   __T("image/cmu-raster")},
  { __E(".rexx"),   __T("text/x-script.rexx")},
  { __E(".rf"),     __T("image/vnd.rn-realflash")},
  { __E(".rgb"),    __T("image/x-rgb")},
  { __E(".rm"),     __T("audio/x-pn-realaudio")},
  { __E(".rmi"),    __T("audio/mid")},
  { __E(".rmm"),    __T("audio/x-pn-realaudio")},
  { __E(".rmp"),    __T("audio/x-pn-realaudio")},
  { __E(".rng"),    __T("application/vnd.nokia.ringing-tone")},
  { __E(".rnx"),    __T("application/vnd.rn-realplayer")},
  { __E(".roff"),   __T("application/x-troff")},
  { __E(".rp"),     __T("image/vnd.rn-realpix")},
  { __E(".rpm"),    __T("audio/x-pn-realaudio-plugin")},
  { __E(".rt"),     __T("text/vnd.rn-realtext")},
  { __E(".rtf"),    __T("application/x-rtf")},
  { __E(".rtx"),    __T("application/x-rtf")},
  { __E(".rv"),     __T("video/vnd.rn-realvideo")},

  { __E(".s"),      __T("text/x-asm")},
  { __E(".s3m"),    __T("audio/s3m")},
  { __E(".saveme"), __T("application/octet-stream")},
  { __E(".sbk"),    __T("application/x-tbook")},
  { __E(".scm"),    __T("text/x-script.scheme")},
  { __E(".sdml"),   __T("text/plain")},
  { __E(".sdp"),    __T("application/x-sdp")},
  { __E(".sdr"),    __T("application/sounder")},
  { __E(".sea"),    __T("application/x-sea")},
  { __E(".set"),    __T("application/set")},
  { __E(".sgm"),    __T("text/x-sgml")},
  { __E(".sgml"),   __T("text/x-sgml")},
  { __E(".sh"),     __T("text/x-script.sh")},
  { __E(".shar"),   __T("application/x-shar")},
  { __E(".shtm"),   __T("text/html")},
  { __E(".shtml"),  __T("text/html")},
  { __E(".sid"),    __T("audio/x-psid")},
  { __E(".sil"),    __T("application/font-sfnt")},
  { __E(".sit"),    __T("application/x-sit")},
  { __E(".skd"),    __T("application/x-koan")},
  { __E(".skm"),    __T("application/x-koan")},
  { __E(".skp"),    __T("application/x-koan")},
  { __E(".skt"),    __T("application/x-koan")},
  { __E(".sl"),     __T("application/x-seelogo")},
  { __E(".smi"),    __T("application/smil")},
  { __E(".smil"),   __T("application/smil")},
  { __E(".snd"),    __T("audio/x-adpcm")},
  { __E(".so"),     __T("application/octet-stream")},
  { __E(".sol"),    __T("application/solids")},
  { __E(".spc"),    __T("text/x-speech")},
  { __E(".spl"),    __T("application/futuresplash")},
  { __E(".spr"),    __T("application/x-sprite")},
  { __E(".sprite"), __T("application/x-sprite")},
  { __E(".src"),    __T("application/x-wais-source")},
  { __E(".ssi"),    __T("text/x-server-parsed-html")},
  { __E(".ssm"),    __T("application/streamingmedia")},
  { __E(".sst"),    __T("application/vnd.ms-pki.certstore")},
  { __E(".step"),   __T("application/step")},
  { __E(".stl"),    __T("application/vnd.ms-pki.stl")},
  { __E(".stp"),    __T("application/step")},
  { __E(".sv4cpio"),__T("application/x-sv4cpio")},
  { __E(".sv4crc"), __T("application/x-sv4crc")},
  { __E(".svf"),    __T("image/x-dwg")},
  { __E(".svg"),    __T("image/svg+xml")},
  { __E(".svr"),    __T("x-world/x-svr")},
  { __E(".swf"),    __T("application/x-shockwave-flash")},

  { __E(".t"),      __T("application/x-troff")},
  { __E(".talk"),   __T("text/x-speech")},
  { __E(".tar"),    __T("application/x-tar")},
  { __E(".tbk"),    __T("application/x-tbook")},
  { __E(".tcl"),    __T("text/x-script.tcl")},
  { __E(".tcsh"),   __T("text/x-script.tcsh")},
  { __E(".tex"),    __T("application/x-tex")},
  { __E(".texi"),   __T("application/x-texinfo")},
  { __E(".texinfo"),__T("application/x-texinfo")},
  { __E(".text"),   __T("text/plain")},
  { __E(".tgz"),    __T("application/x-compressed")},
  { __E(".tif"),    __T("image/x-tiff")},
  { __E(".tiff"),   __T("image/x-tiff")},
  { __E(".torrent"),   __T("application/x-bittorrent")},
  { __E(".tr"),    __T("application/x-troff")},
  { __E(".tsi"),   __T("audio/tsp-audio")},
  { __E(".tsp"),   __T("audio/tsplayer")},
  { __E(".tsv"),   __T("text/tab-separated-values")},
  { __E(".ttf"),   __T("application/font-sfnt")},
  { __E(".turbot"),__T("image/florian")},
  { __E(".txt"),   __T("text/plain")},

  { __E(".uil"),   __T("text/x-uil")},
  { __E(".uni"),   __T("text/uri-list")},
  { __E(".unis"),  __T("text/uri-list")},
  { __E(".unv"),   __T("application/i-deas")},
  { __E(".uri"),   __T("text/uri-list")},
  { __E(".uris"),  __T("text/uri-list")},
  { __E(".ustar"), __T("application/x-ustar")},
  { __E(".uu"),    __T("text/x-uuencode")},
  { __E(".uue"),   __T("text/x-uuencode")},

  { __E(".vcd"),   __T("application/x-cdlink")},
  { __E(".vcs"),   __T("text/x-vcalendar")},
  { __E(".vda"),   __T("application/vda")},
  { __E(".vdo"),   __T("video/vdo")},
  { __E(".vew"),   __T("application/groupwise")},
  { __E(".viv"),   __T("video/vnd.vivo")},
  { __E(".vivo"),  __T("video/vnd.vivo")},
  { __E(".vmd"),   __T("application/vocaltec-media-desc")},
  { __E(".vmf"),   __T("application/vocaltec-media-file")},
  { __E(".voc"),   __T("audio/x-voc")},
  { __E(".vos"),   __T("video/vosaic")},
  { __E(".vox"),   __T("audio/voxware")},
  { __E(".vqe"),   __T("audio/x-twinvq-plugin")},
  { __E(".vqf"),   __T("audio/x-twinvq")},
  { __E(".vql"),   __T("audio/x-twinvq-plugin")},
  { __E(".vrml"),  __T("model/vrml")},
  { __E(".vrt"),   __T("x-world/x-vrt")},
  { __E(".vsd"),   __T("application/x-visio")},
  { __E(".vst"),   __T("application/x-visio")},
  { __E(".vsw"),   __T("application/x-visio")},

  { __E(".w60"),   __T("application/wordperfect6.0")},
  { __E(".w61"),   __T("application/wordperfect6.1")},
  { __E(".w6w"),   __T("application/msword")},
  { __E(".wav"),   __T("audio/x-wav")},
  { __E(".wb1"),   __T("application/x-qpro")},
  { __E(".wbmp"),  __T("image/vnd.wap.wbmp")},
  { __E(".web"),   __T("application/vnd.xara")},
  { __E(".webm"),  __T("video/webm")},
  { __E(".wiz"),   __T("application/msword")},
  { __E(".wk1"),   __T("application/x-123")},
  { __E(".wmf"),   __T("windows/metafile")},
  { __E(".wml"),   __T("text/vnd.wap.wml")},
  { __E(".wmlc"),  __T("application/vnd.wap.wmlc")},
  { __E(".wmls"),  __T("text/vnd.wap.wmlscript")},
  { __E(".wmlsc"), __T("application/vnd.wap.wmlscriptc")},
  { __E(".woff"),  __T("application/font-woff")},
  { __E(".word"),  __T("application/msword")},
  { __E(".wp"),    __T("application/wordperfect")},
  { __E(".wp5"),   __T("application/wordperfect")},
  { __E(".wp6"),   __T("application/wordperfect")},
  { __E(".wpd"),   __T("application/wordperfect")},
  { __E(".wq1"),   __T("application/x-lotus")},
  { __E(".wri"),   __T("application/x-wri")},
  { __E(".wrl"),   __T("model/vrml")},
  { __E(".wrz"),   __T("model/vrml")},
  { __E(".wsc"),   __T("text/scriplet")},
  { __E(".wsrc"),  __T("application/x-wais-source")},
  { __E(".wtk"),   __T("application/x-wintalk")},

  { __E(".x-png"), __T("image/png")},
  { __E(".xbm"),   __T("image/x-xbm")},
  { __E(".xdr"),   __T("video/x-amt-demorun")},
  { __E(".xgz"),   __T("xgl/drawing")},
  { __E(".xhtml"), __T("application/xhtml+xml")},
  { __E(".xif"),   __T("image/vnd.xiff")},
  { __E(".xl"),    __T("application/vnd.ms-excel")},
  { __E(".xla"),   __T("application/vnd.ms-excel")},
  { __E(".xlb"),   __T("application/vnd.ms-excel")},
  { __E(".xlc"),   __T("application/vnd.ms-excel")},
  { __E(".xld"),   __T("application/vnd.ms-excel")},
  { __E(".xlk"),   __T("application/vnd.ms-excel")},
  { __E(".xll"),   __T("application/vnd.ms-excel")},
  { __E(".xlm"),   __T("application/vnd.ms-excel")},
  { __E(".xls"),   __T("application/vnd.ms-excel")},
  { __E(".xlt"),   __T("application/vnd.ms-excel")},
  { __E(".xlv"),   __T("application/vnd.ms-excel")},
  { __E(".xlw"),   __T("application/vnd.ms-excel")},
  { __E(".xm"),    __T("audio/xm")},
  { __E(".xml"),   __T("text/xml")},
  { __E(".xmz"),   __T("xgl/movie")},
  { __E(".xpix"),  __T("application/x-vnd.ls-xpix")},
  { __E(".xpm"),   __T("image/x-xpixmap")},
  { __E(".xsl"),   __T("application/xml")},
  { __E(".xslt"),  __T("application/xml")},
  { __E(".xsr"),   __T("video/x-amt-showrun")},
  { __E(".xwd"),   __T("image/x-xwd")},
  { __E(".xyz"),   __T("chemical/x-pdb")},

  { __E(".z"),     __T("application/x-compressed")},
  { __E(".zip"),   __T("application/x-zip-compressed")},
  { __E(".zoo"),   __T("application/octet-stream")},
  { __E(".zsh"),   __T("text/x-script.zsh")},
};

ng_result_t ng_http_mime_type_init(void) 
{
  size_t i;
  NUM_MIME_TYPES = NG_ITEMS(_mime_types_names);
  for (i = 0; i < _MIME_HASH_TBLSZ; i++)
  {
    ng_INIT_LIST_HEAD(&_mime_types_htbl[i]);
  }
  for (i = 0; i < NUM_MIME_TYPES; i++)
  {
    mime_types_s *m = &_mime_types_names[i];
    uint32_t hash = ng_hash_string(m->extension.cptr, m->extension.len);
    ng_list_head_s *h = &_mime_types_htbl[hash&_MIME_HASH_MASK];
    ng_list_add_tail(&m->link, h);
  }
  log_info("[OK]: ng_http_mime_type_init.");
  return ng_ERR_NONE;
} 

void ng_http_mime_type_free(void) 
{
  log_info("[OK]: ng_http_mime_type_free.");
  return;
}
/*
 * const char *httplib_get_builtin_mime_type( const char *path );
 *
 * The function httplib_get_builtin_mime_type() returns the mime type
 * associated with the file with a given extension which is passed as a
 * parameter. The function performs a binary search through the list of MIME
 * types which is very efficient and only needs 10 steps for 1000 items in the
 * list or 20 steps for 1000000 items.
 *
 * If no matching file extension could be found in the list, the default value
 * of "text/plain" is returned instead.
 */
const ng_block_s *
ng_http_get_mime_type(const ng_block_s *ext) 
{
  uint32_t hash;
  mime_types_s *m;
  ng_list_head_s *h;

  if (ext == NULL || !ext->len)
    return &_mime_type_default;

  hash = ng_hash_string(ext->cptr, ext->len);
  h = &_mime_types_htbl[hash&_MIME_HASH_MASK];
  ng_list_for_each_entry(m,mime_types_s,h,link)
  {
    if (ng_block_isequal_nocase(&m->extension,ext))
      return &m->mime_type;
  }
  
  return &_mime_type_default;
} 

const ng_block_s *
ng_http_get_mime_type_from_file(const ng_block_s *file) 
{
  ng_block_s dot;

  if (file == NULL || !file->len)
    return &_mime_type_default;

  dot.cptr = ng_memchr(file->cptr, '.', file->len);
  dot.len  = dot.buf == NULL ? 0 : ng_block_end(file) - dot.cptr;
  while (1) 
  {
    const char *n = ng_memchr(dot.buf + 1, '.', file->len);
    if (n == NULL)
      break;
    dot.cptr = n;
    dot.len  = ng_block_end(file) - n;
  }

  return ng_http_get_mime_type(&dot);
}
