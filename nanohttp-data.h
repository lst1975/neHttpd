/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
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
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
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
#ifndef __nanohttp_data_h
#define __nanohttp_data_h

#define __HTTPD_LICENSE \
    "######################################################################\n" \
    "         Embedded HTTP Server for Web Configuraion Framwork           \n" \
    "                      _    _ _______ _______ _____  _____             \n" \
    "                     | |  | |__   __|__   __|  __ \\|  __ \\            \n" \
    "           _ __   ___| |__| |  | |     | |  | |__) | |  | |           \n" \
    "          | '_ \\ / _ \\  __  |  | |     | |  |  ___/| |  | |           \n" \
    "          | | | |  __/ |  | |  | |     | |  | |    | |__| |           \n" \
    "          |_| |_|\\___|_|  |_|  |_|     |_|  |_|    |_____/            \n" \
    "                                                                      \n" \
    "                                                                      \n" \
    "                 https://github.com/lst1975/neHttpd                   \n" \
    "                                                                      \n" \
    "        Embedded HTTP Server with Web Configuraion Framework          \n" \
    "                   Copyright (C) 2025 Songtao Liu                     \n" \
    "                          980680431@qq.com                            \n" \
    "                         All Rights Reserved                          \n" \
    "######################################################################\n"

#define favorICON \
  "AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAABAAACMuAAAjLgAAAAAAAAAAAAAAAAANAAAADQAA" \
  "AA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAA" \
  "AA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAAwAAwwMABFSEgAQ" \
  "TRIAAggLAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAAMAAcgDQATWBMACi8PAAAACwAA" \
  "AAwAAQcMABFPEgARTxIAAQcLAAAADAAAAA0AAAANAAAADQAAAA0AAAAMACSpIAA072kANvmWADb5lAAz7WMAIZwcAAAADAAA" \
  "AA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADAAqwisANfR6ADb6mQAy6YYAJaw7ABhzFQAy610ANvmVADb5lQAz" \
  "610AGnkWAAAADAAAAA0AAAANAAAADAAioB4ANvmTADf/ywA3/8QAN//FADf/ygA194kAHYYZAAAADAAAAA0AAAANAAAADQAA" \
  "AA0AAAANAAAADQAAAAwAJrAiADb7nwA3/8sANvvDAC/c0AAmsbwAL9yKADf/xQA3/8UAN//FADf/yAA08W0ABRgOAAAADQAA" \
  "AA0AAAALADPsXgA3/8oANvqaAC7YPgAv3EMANvujADf/yAAx5lIAAAAKAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAEFDQA0" \
  "8W0AN//MADb5kQAszTcAJrSEACav2AA08s0AN/64ADHkTgAx5E4AN/66ADb9rwAkpyEAAAAMAAAADQADDw4ANfaFADf/yAAx" \
  "5EkAAAAIAAAACAAy6lcAN//LADT0eQAAAAwAAAANAAAADQAAAA0AAAANAAAADQAAAAsAK8kuADf9tQA3/rsALtY3AAAABQAe" \
  "jkoALdPMADf/ygA083QAAw4LAByCFQA2+6AAN/66ACm+KwAAAAsAAAANAAcjDwA194oAN//FAC7ZPAAAAAoAAAAKADHjSgA3" \
  "/8kANfaAAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAszDwA19XwAN//MADT0eAAIKA4AAAAJACvLTwA1+MkANv2vACnAKQAA" \
  "AAsAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19oAAAAANAAAADQAA" \
  "AA0AAAANAAAADQAAAAsALtc6ADf+vAA3/bIAKsYsAAAACgAYcBQANvmOADf9zgAv240ABRoPAAAADAAYcRYANvufADf+ugAp" \
  "vysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw4UsAN//JADX2gAAAAA0AAAANAAAADQAAAA0AAAANABRgEwA1" \
  "+IoAN//LADTwaQAAAAwAAAAKADHkTAA3/8IAM+zPACe3jwADEQ4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA1" \
  "94oAN//FAC7XPQAAAAoAAAAKADDhSwA3/8kANfaAAAAADQAAAA0AAAANAAAADQAAAAoAMOFHADf/wwA2/KgAJrIiAAAACwAf" \
  "kxoANvqcADf9ywAt0NQAKsSNAAYcDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAA" \
  "AAoAMOFLADf/yQA19oAAAAANAAAADQAAAA0AAAAMAB2HGAA2+pcAN//JADPrWwAAAAsAAAALADLrWgA3/8YAM+zCACzN0wAx" \
  "5YkABhsOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw4UsAN//JADX2gAAA" \
  "AA0AAAANAAAADQAAAAsAMulUADf/xwA2+p0AIJUbAAAACwAlryIANvyoADf/wAAv3YIAMeXJADX1hwAFGA4AAAAMABhxFgA2" \
  "+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3/8kANfaAAAAADQAAAA0AAAAMACOlHwA2" \
  "+6MAN//FADHlTAAAAAoAAAAMADTwaQA3/8sANfiIAC7WSwA2+sgANfeHAAUXDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAA" \
  "AA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19oAAAAANAAAADQAAAAsAM+5jADf/ywA1+JAAGHAVAAAACgAq" \
  "xCsAN/2yADf+vQAu1TgAL9xBADf/xwA194cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu" \
  "1z0AAAAKAAAACgAw4UsAN//JADX2gAAAAA0AAAALACi9KAA2/K4AN/6/AC/cPwAAAAoACCQOADX0dwA3/8wANfV9AAAADAAv" \
  "3UMAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3" \
  "/8kANfaAAAAADQAEEg0ANPJyADf/zAA19oEADkQQAAAACgAt1DYAN/66ADf9tQAsyy8AAAAIAC/dRAA3/8cANfeHAAUXDgAA" \
  "AAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19YAAAAALACzOMgA3" \
  "/bcAN/24ACzPMwAAAAoAElMRADX3hgA3/8wANPJuAAIJDQAAAAoAL91EADf/xwA194cABRcOAAAADAAYcRYANvufADf+ugAp" \
  "vysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw4UsAN//JADX1fwANPxAANfaAADf/zAA083MABRYNAAAACgAw" \
  "30MAN/7BADb8rAAnuCUAAAALAAAACgAv3UQAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA1" \
  "94oAN//FAC7XPQAAAAoAAAAKADDhSwA3/skAM+5+AC/aPgA3/r8ANv2vACm/KAAAAAoAG30WADb5lAA3/8oAM+1gAAAACwAA" \
  "AA0AAAAKAC/dRAA3/8cANfeHAAUXDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAA" \
  "AAoAMeJLADX3ygAu2IwANfiOADf/ygAz72QAAAALAAAACwAy5lAAN//GADb7oQAioB0AAAAMAAAADQAAAAoAL91EADf/xwA1" \
  "94cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw3ksAMN/MAC3TvAA3" \
  "/8MANvukACSnHwAAAAsAIp4dADb7oAA3/8YAMudRAAAACwAAAA0AAAANAAAACgAv3UQAN//HADX3hwAFFw4AAAAMABhxFgA2" \
  "+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKACrFTQAryNAAMunNADf/xQAy6VUAAAALAAAACwAz" \
  "7V8AN//KADb5lAAbfxcAAAAMAAAADQAAAA0AAAAKAC/dRAA3/8cANfeHAAUXDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAA" \
  "AA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAJKdPACzPzgA3/csANvqXAB2IGAAAAAsAJ7clADb8qwA3/sEAMN9EAAAACgAA" \
  "AA0AAAANAAAADQAAAAoAL91EADf/xwA194cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu" \
  "1z0AAAAKAAAACgAmsE4AM+7KADf/wAAw4UcAAAAKAAEGDQA08m4AN//MADX3hwASVhIAAAANAAAADQAAAA0AAAANAAAACgAv" \
  "3UQAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAMADPtbAA3" \
  "/swANPGUABVjFAAAAAoAK8ovADf9tQA3/rsALtU3AAAACwAAAA0AAAANAAAADQAAAA0AAAAKAC/dRAA3/8cANfeHAAQWDgAA" \
  "AAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByMPADX3igA3/8UAL9w8AAAABwAqwykAN/2wADX0zwAntocAAAALAAs1DwA1" \
  "9XwAN//MADT0eQAIKA4AAAANAAAADQAAAA0AAAANAAAADQAAAAoAL9tBADf/xwA1+IwACzcOAAAACwAeixcANvujADf+ugAp" \
  "vSoAAAALAAAADQABBw0ANfaCADf/ygAz610AG34YADT0eQA3/8oALdDUACOjpAAdhyAAL91DADf+vQA3/bMAKsUsAAAACwAA" \
  "AA0AAAANAAAADQAAAA0AAAANAAAACwApwCwAN/23ADf9tAAu1z0AGHEWADHkTgA3/sAANvynACCVHQAAAAwAAAANAAAACwAx" \
  "5E4AN//EADf+vQA2+6cAN//FADb6twAms7wAKsPTADPsrAA3/rcAN//JADPwZwAAAAwAAAANAAAADQAAAA0AAAANAAAADQAA" \
  "AA0AAAANAAovDwA083QAN//KADf+ugA2+6MAN/6/ADf/xQAz7F0AAAAMAAAADQAAAA0AAAANABNZEgAz7WIANvysADf+uwA2" \
  "+6EAMN9GACGaOgAv2ZkANvq6ADf9sgA08nMAHYkYAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAAMAB6KGQA0" \
  "8W0AN/ytADf+uwA2/KcAM+xfABViEwAAAAwAAAANAAAADQAAAA0AAAAMAAcfDQAioiAAKcArAB6OGwABBAwAAAALABlzFgAo" \
  "vSoAJrAkAA07DwAAAAwAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAAMAAgmDQAjoyAAKcAsACCWHAAE" \
  "FAwAAAAMAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADAAAAAsAAAAMAAAADQAAAA0AAAAMAAAACwAAAAsAAAANAAAADQAA" \
  "AA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADAAAAAsAAAAMAAAADQAAAA0AAAANAAAADQAA" \
  "AA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAA" \
  "AA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAAAAAAAAAAAAAAAAAAAAAA" \
  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" \
  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA="

#define __nanohttp_index_html_head_JS_MENU_GUEST \
  "<script class='userjs' type='text/javascript' src='config/menu-guest.js'></script>"
#define __nanohttp_index_html_head_JS_MENU_ADMIN \
  "<script class='userjs' type='text/javascript' src='config/menu-admin.js'></script>"
#define __nanohttp_index_html_head_JS_MENU_SUPER \
  "<script class='userjs' type='text/javascript' src='config/menu-super.js'></script>"
#define __nanohttp_index_html_head_DECL1 \
  "<!DOCTYPE html>" \
  "<html lang='en'>" \
    "<head>" \
      "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>" \
      "<meta id='MetaDescription' name='DESCRIPTION' content=''>" \
      "<meta id='MetaKeywords' name='KEYWORDS' content=''>" \
      "<meta name='msapplication-TileColor' content='#000000'>" \
      "<meta name='msapplication-TileImage' content='config/logo.png'>" \
      "<meta name='viewport' content='initial-scale=1, width=device-width'>" \
      "<meta name='msapplication-tap-highlight' content='no'>" \
      "<meta http-equiv='X-UA-Compatible' content='IE=edge'>" \
      "<meta http-equiv='X-UA-Compatible' content='IE=10'>" \
      "<meta http-equiv='X-UA-Compatible' content='IE=9'>" \
      "<meta http-equiv='cleartype' content='off'>" \
      "<meta name='MobileOptimized' content='320'>" \
      "<meta name='HandheldFriendly' content='True'>" \
      "<meta name='mobile-web-app-capable' content='yes'>" \
      "<link rel='apple-touch-icon' href='config/logo.png'>" \
      "<link rel='icon' href=''>" \
      "<style>" \
      " /* following two viewport lines are equivalent to meta viewport statement above, and is needed for Windows */" \
      " /* see http://www.quirksmode.org/blog/archives/2014/05/html5_dev_conf.html and http://dev.w3.org/csswg/css-device-adapt/ */" \
      " @-ms-viewport { width: 100vw ; min-zoom: 100% ; zoom: 100% ; } @viewport { width: 100vw ; min-zoom: 100% zoom: 100% ; }" \
      " @-ms-viewport { user-zoom: fixed ; min-zoom: 100% ; } @viewport { user-zoom: fixed ; min-zoom: 100% ; }" \
      " /*@-ms-viewport { user-zoom: zoom ; min-zoom: 100% ; max-zoom: 200% ; }   @viewport { user-zoom: zoom ; min-zoom: 100% ; max-zoom: 200% ; }*/" \
      "</style>" \
      "<title></title>" \
      "<!--[if IE]><link rel='shortcut icon' href=''><![endif]-->" \
      "<link rel='stylesheet' type='text/css' href='config/messagebox.css' media='screen'>" \
      "<link rel='stylesheet' type='text/css' href='config/base.css' media='screen'>" \
      "<link rel='stylesheet' type='text/css' href='config/signal.css' media='screen'>" \
      "<link rel='stylesheet' type='text/css' href='config/jquery.csspiechart.css' media='screen'>" \
      "<script type='text/javascript' src='config/jquery-3.7.1.js'></script>" \
      "<script type='text/javascript' src='config/messagebox.js'></script>" \
      "<script type='text/javascript' src='config/lang.js'></script>" \
      "<script type='text/javascript' src='config/data.js'></script>" \
      "<script type='text/javascript' src='config/utils.js'></script>" \
      "<script type='text/javascript' src='config/main.upgrade.js'></script>" \
      "<script type='text/javascript' src='config/main.system.js'></script>" \
      "<script type='text/javascript' src='config/main.device.js'></script>"

#define __nanohttp_index_html_head_DECL2 \
      "<script type='text/javascript' src='config/main.js'></script>" \
      "<script type='text/javascript' src='config/jquery.input-ip-address-control.js'></script>" \
      "<script type='text/javascript' src='config/signal.js'></script>" \
      "<script type='text/javascript' src='config/jquery.csspiechart.js'></script>" \
      "<script type='text/javascript' src='config/config.js'></script>" \
      "<script type='text/javascript' src='config/jquery.mousewheel.js'></script>" \
      "<script type='text/javascript' src='config/alloy_finger.js'></script>" \
    "</head>"

#define __nanohttp_index_html_head_LOGIN \
    "<body>"  \
      "<div id='id01' class='__login modal'>"  \
        "<div class='modal-content animate' action='config/secure' method='get'>" \
          "<div class='container'>"  \
            "<label for='uname'><b>Username</b></label>"  \
            "<input class='name' type='text' value='' placeholder='Enter Username' name='uname' required>" \
            "<label for='psw'><b>Password</b></label>"  \
            "<input class='password' type='password' value='' placeholder='Enter Password' name='psw' required>" \
            "<button class='login' type='submit'>Login</button>" \
          "</div>"  \
        "</div>"  \
      "</div>"  \
    "</body>"  \
  "</html>"

#define __nanohttp_index_html_body  \
    "<div class=\"workingBusy\"></div>" \
    "<table class=\"main-layout\">" \
      "<tr class='head'>" \
        "<td class=\"table-head left\">" \
          "<img class=\"logo\" src=\"\">" \
        "</td>" \
        "<td class=\"table-head right\">" \
          "<span class=\"title\"></span>" \
          "<a class=\"login\"><img src=\"\"></a>" \
        "</td>" \
      "</tr>" \
      "<tr class='main'>" \
        "<td class=\"table-left\">" \
          "<div class=\"table-left-container\">" \
          "</div>" \
        "</td>" \
        "<td class=\"table-right\">" \
          "<div class=\"panel_container\">" \
          "</div>" \
        "</td>" \
    "	</tr>" \
    "</table>" \
    "<script>\n" \
      "var userjs = $('html > head > script.userjs');\n" \
      "if (userjs.length) userjs.remove();\n" \
    "</script>"

#define __nanohttp_secure_html  \
    "<html>" \
      "<head>" \
        "<title>Secure ressource!</title>" \
      "</head>" \
      "<body>" \
        "<h1>Authenticated access!!!</h1>" \
      "</body>" \
    "</html>"

#define __S_ "     %-32s%s"
#define __nanohttp_help  \
      __S_ __S_ __S_ __S_ __S_ __S_ __S_ __S_ __S_ __S_ __S_, \
              NHTTPD_ARG_PORT" [port]",   ": Port the server is listening on\n", \
           NHTTPD_ARG_TERMSIG" [signal]", ": Signal which may used to terminate the server\n", \
           NHTTPD_ARG_MAXCONN" [count]",  ": Maximum number of simultanous connections\n", \
      NHTTPD_ARG_MAXCONN_PEND" [count]",  ": Maximum number of pending connections for listen\n", \
           NHTTPD_ARG_TIMEOUT" [time]",   ": Timeout on reads\n", \
          NHTTPD_ARG_LOGLEVEL" [level]",  ": fatal, error, warn, event, debug, verbose|-v, off.\n", \
         NHTTPD_ARG_DAEMONIZE,            ": Run as a daemon\n", \
                          "-d",           ": Run as a daemon\n", \
                          "-v",           ": Logging verbose\n", \
                          "-h",           ": Help\n", \
                NHTTPD_ARG_HELP,          ": Help\n"

#endif
