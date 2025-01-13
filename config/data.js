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
var svgXmlHeader = 		 
		 "<?xml version='1.0' encoding='UTF-8' standalone='no'?>"
		+"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 20010904//EN' "
		+" 'http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd'>";
function getSvgDataURI(svg)
{
  return "data:image/svg+xml;base64,"+btoa(svgXmlHeader + svg);
}
var __icons={
"config/favicon.ico":
   "data:image/vnd.microsoft.icon;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAABA"
	+"AACMuAAAjLgAAAAAAAAAAAAAAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAA"
	+"ADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAA"
	+"ADQAAAA0AAAANAAAADQAAAAwAAwwMABFSEgAQTRIAAggLAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAA"
	+"ADQAAAA0AAAAMAAcgDQATWBMACi8PAAAACwAAAAwAAQcMABFPEgARTxIAAQcLAAAADAAAAA0AAAANAAAADQAAAA0AAAAMACS"
	+"pIAA072kANvmWADb5lAAz7WMAIZwcAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADAAqwisANfR6ADb"
	+"6mQAy6YYAJaw7ABhzFQAy610ANvmVADb5lQAz610AGnkWAAAADAAAAA0AAAANAAAADAAioB4ANvmTADf/ywA3/8QAN//FADf"
	+"/ygA194kAHYYZAAAADAAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAAwAJrAiADb7nwA3/8sANvvDAC/c0AAmsbwAL9yKADf"
	+"/xQA3/8UAN//FADf/yAA08W0ABRgOAAAADQAAAA0AAAALADPsXgA3/8oANvqaAC7YPgAv3EMANvujADf/yAAx5lIAAAAKAAA"
	+"ADQAAAA0AAAANAAAADQAAAA0AAAANAAEFDQA08W0AN//MADb5kQAszTcAJrSEACav2AA08s0AN/64ADHkTgAx5E4AN/66ADb"
	+"9rwAkpyEAAAAMAAAADQADDw4ANfaFADf/yAAx5EkAAAAIAAAACAAy6lcAN//LADT0eQAAAAwAAAANAAAADQAAAA0AAAANAAA"
	+"ADQAAAAsAK8kuADf9tQA3/rsALtY3AAAABQAejkoALdPMADf/ygA083QAAw4LAByCFQA2+6AAN/66ACm+KwAAAAsAAAANAAc"
	+"jDwA194oAN//FAC7ZPAAAAAoAAAAKADHjSgA3/8kANfaAAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAszDwA19XwAN//MADT"
	+"0eAAIKA4AAAAJACvLTwA1+MkANv2vACnAKQAAAAsAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAA"
	+"ACgAAAAoAMOFLADf/yQA19oAAAAANAAAADQAAAA0AAAANAAAADQAAAAsALtc6ADf+vAA3/bIAKsYsAAAACgAYcBQANvmOADf"
	+"9zgAv240ABRoPAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw4UsAN//JADX"
	+"2gAAAAA0AAAANAAAADQAAAA0AAAANABRgEwA1+IoAN//LADTwaQAAAAwAAAAKADHkTAA3/8IAM+zPACe3jwADEQ4AAAAMABh"
	+"xFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3/8kANfaAAAAADQAAAA0AAAANAAA"
	+"ADQAAAAoAMOFHADf/wwA2/KgAJrIiAAAACwAfkxoANvqcADf9ywAt0NQAKsSNAAYcDgAAAAwAGHEWADb7nwA3/roAKb8rAAA"
	+"ACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19oAAAAANAAAADQAAAA0AAAAMAB2HGAA2+pcAN//JADP"
	+"rWwAAAAsAAAALADLrWgA3/8YAM+zCACzN0wAx5YkABhsOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf"
	+"/xQAu1z0AAAAKAAAACgAw4UsAN//JADX2gAAAAA0AAAANAAAADQAAAAsAMulUADf/xwA2+p0AIJUbAAAACwAlryIANvyoADf"
	+"/wAAv3YIAMeXJADX1hwAFGA4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADD"
	+"hSwA3/8kANfaAAAAADQAAAA0AAAAMACOlHwA2+6MAN//FADHlTAAAAAoAAAAMADTwaQA3/8sANfiIAC7WSwA2+sgANfeHAAU"
	+"XDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMOFLADf/yQA19oAAAAANAAA"
	+"ADQAAAAsAM+5jADf/ywA1+JAAGHAVAAAACgAqxCsAN/2yADf+vQAu1TgAL9xBADf/xwA194cABRcOAAAADAAYcRYANvufADf"
	+"+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw4UsAN//JADX2gAAAAA0AAAALACi9KAA2/K4AN/6/AC/"
	+"cPwAAAAoACCQOADX0dwA3/8wANfV9AAAADAAv3UMAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAc"
	+"iDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3/8kANfaAAAAADQAEEg0ANPJyADf/zAA19oEADkQQAAAACgAt1DYAN/66ADf"
	+"9tQAsyy8AAAAIAC/dRAA3/8cANfeHAAUXDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAA"
	+"ACgAAAAoAMOFLADf/yQA19YAAAAALACzOMgA3/bcAN/24ACzPMwAAAAoAElMRADX3hgA3/8wANPJuAAIJDQAAAAoAL91EADf"
	+"/xwA194cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAw4UsAN//JADX"
	+"1fwANPxAANfaAADf/zAA083MABRYNAAAACgAw30MAN/7BADb8rAAnuCUAAAALAAAACgAv3UQAN//HADX3hwAFFw4AAAAMABh"
	+"xFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKADDhSwA3/skAM+5+AC/aPgA3/r8ANv2vACm"
	+"/KAAAAAoAG30WADb5lAA3/8oAM+1gAAAACwAAAA0AAAAKAC/dRAA3/8cANfeHAAUXDgAAAAwAGHEWADb7nwA3/roAKb8rAAA"
	+"ACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAMeJLADX3ygAu2IwANfiOADf/ygAz72QAAAALAAAACwAy5lAAN//GADb"
	+"7oQAioB0AAAAMAAAADQAAAAoAL91EADf/xwA194cABRcOAAAADAAYcRYANvufADf+ugApvysAAAALAAAADQAHIg8ANfeKADf"
	+"/xQAu1z0AAAAKAAAACgAw3ksAMN/MAC3TvAA3/8MANvukACSnHwAAAAsAIp4dADb7oAA3/8YAMudRAAAACwAAAA0AAAANAAA"
	+"ACgAv3UQAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAciDwA194oAN//FAC7XPQAAAAoAAAAKACr"
	+"FTQAryNAAMunNADf/xQAy6VUAAAALAAAACwAz7V8AN//KADb5lAAbfxcAAAAMAAAADQAAAA0AAAAKAC/dRAA3/8cANfeHAAU"
	+"XDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByIPADX3igA3/8UALtc9AAAACgAAAAoAJKdPACzPzgA3/csANvqXAB2"
	+"IGAAAAAsAJ7clADb8qwA3/sEAMN9EAAAACgAAAA0AAAANAAAADQAAAAoAL91EADf/xwA194cABRcOAAAADAAYcRYANvufADf"
	+"+ugApvysAAAALAAAADQAHIg8ANfeKADf/xQAu1z0AAAAKAAAACgAmsE4AM+7KADf/wAAw4UcAAAAKAAEGDQA08m4AN//MADX"
	+"3hwASVhIAAAANAAAADQAAAA0AAAANAAAACgAv3UQAN//HADX3hwAFFw4AAAAMABhxFgA2+58AN/66ACm/KwAAAAsAAAANAAc"
	+"iDwA194oAN//FAC7XPQAAAAoAAAAMADPtbAA3/swANPGUABVjFAAAAAoAK8ovADf9tQA3/rsALtU3AAAACwAAAA0AAAANAAA"
	+"ADQAAAA0AAAAKAC/dRAA3/8cANfeHAAQWDgAAAAwAGHEWADb7nwA3/roAKb8rAAAACwAAAA0AByMPADX3igA3/8UAL9w8AAA"
	+"ABwAqwykAN/2wADX0zwAntocAAAALAAs1DwA19XwAN//MADT0eQAIKA4AAAANAAAADQAAAA0AAAANAAAADQAAAAoAL9tBADf"
	+"/xwA1+IwACzcOAAAACwAeixcANvujADf+ugApvSoAAAALAAAADQABBw0ANfaCADf/ygAz610AG34YADT0eQA3/8oALdDUACO"
	+"jpAAdhyAAL91DADf+vQA3/bMAKsUsAAAACwAAAA0AAAANAAAADQAAAA0AAAANAAAACwApwCwAN/23ADf9tAAu1z0AGHEWADH"
	+"kTgA3/sAANvynACCVHQAAAAwAAAANAAAACwAx5E4AN//EADf+vQA2+6cAN//FADb6twAms7wAKsPTADPsrAA3/rcAN//JADP"
	+"wZwAAAAwAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAovDwA083QAN//KADf+ugA2+6MAN/6/ADf/xQAz7F0AAAAMAAA"
	+"ADQAAAA0AAAANABNZEgAz7WIANvysADf+uwA2+6EAMN9GACGaOgAv2ZkANvq6ADf9sgA08nMAHYkYAAAADAAAAA0AAAANAAA"
	+"ADQAAAA0AAAANAAAADQAAAA0AAAAMAB6KGQA08W0AN/ytADf+uwA2/KcAM+xfABViEwAAAAwAAAANAAAADQAAAA0AAAAMAAc"
	+"fDQAioiAAKcArAB6OGwABBAwAAAALABlzFgAovSoAJrAkAA07DwAAAAwAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAA"
	+"ADQAAAA0AAAAMAAgmDQAjoyAAKcAsACCWHAAEFAwAAAAMAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADAAAAAsAAAAMAAA"
	+"ADQAAAA0AAAAMAAAACwAAAAsAAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAA"
	+"ADAAAAAsAAAAMAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAA"
	+"ADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAAADQAAAA0AAAANAAA"
	+"ADQAAAA0AAAANAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
	+"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=",
"config/login.png":
   '<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="610.000000pt" height="607.000000pt"'
	+' viewBox="0 0 610.000000 607.000000" preserveAspectRatio="xMidYMid meet">'
	+'<g transform="translate(0.000000,607.000000) scale(0.100000,-0.100000)" fill="#ffffff" stroke="none">'
	+'<path d="M2919 6060 c-715 -68 -1293 -641 -1413 -1400 -33 -212 -28 -444 14 -649 123 -588 534 -1070 1063 '
	+'-1246 179 -59 250 -70 467 -70 217 0 288 11 466 70 522 173 930 643 1058 1216 89 399 40 819 -139 1186 '
	+'-159 325 -414 586 -729 745 -102 51 -288 113 -396 132 -113 19 -285 26 -391 16z"/><path d="M1765 2867 '
	+'c-179 -86 -253 -127 -395 -222 -593 -396 -1013 -951 -1228 -1625 -90 -282 -142 -619 -142 -922 l0 -98 '
	+'3050 0 3051 0 -2 138 c-9 612 -179 1160 -522 1672 -210 314 -536 635 -857 845 -121 79 -332 194 -432 '
	+'235 l-78 33 -76 -77 c-208 -208 -453 -331 -769 -386 -371 -64 -760 -13 -1059 140 -127 64 -233 141 '
	+'-332 240 -43 44 -83 80 -89 80 -5 0 -59 -24 -120 -53z"/>'
	+'</g>'
	+'</svg>',
"config/logo.png":
   "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAAAA7XpUWHRSYXcgcHJvZmlsZSB0eX"
	+"BlIGV4aWYAAHjajVHbDcMwCPz3FB0BA34wjvOSukHH7yXGSVOpUpFM4MD4uIT19dzCYzeuHDSVmi1ngqmpcUNQqdt0+Eh6+M"
	+"NGCfkND2eBAQm+0lNjx1fgiKPn5hfi6B+DRhAbonQVWnN8uuOTD+T6PcgZSOwv0+IXfJCwM9Kez84oWy231ZaZ7lavo1I4px"
	+"yLwitTKdkQVyYt0HPZiW4z2zEodUFPYOSjlcGJV4lC8CK1s5T9qDTgER77BDSiCQmjbS904Qm/EhTA3PyhRqeYn9pcGv2wf9"
	+"YKbywCdqzvxHTPAAABhWlDQ1BJQ0MgcHJvZmlsZQAAeJx9kT1Iw1AUhU9TpaIVB4uIOGSo4mAXFXEsVSyChdJWaNXB5KV/0K"
	+"QhSXFxFFwLDv4sVh1cnHV1cBUEwR8QZwcnRRcp8b6k0CLGC4/3cd49h/fuA4RGhalmVxRQNctIxWNiNrcqBl7RBx8EDGFCYq"
	+"aeSC9m4Flf99RLdRfhWd59f1a/kjcZ4BOJo0w3LOIN4tlNS+e8TxxiJUkhPieeNOiCxI9cl11+41x0WOCZISOTmicOEYvFDp"
	+"Y7mJUMlXiGOKyoGuULWZcVzluc1UqNte7JXxjMaytprtMaRRxLSCAJETJqKKMCCxHaNVJMpOg85uEfcfxJcsnkKoORYwFVqJ"
	+"AcP/gf/J6tWZiecpOCMaD7xbY/xoDALtCs2/b3sW03TwD/M3Cltf3VBjD3SXq9rYWPgIFt4OK6rcl7wOUOMPykS4bkSH5aQq"
	+"EAvJ/RN+WAwVugd82dW+scpw9Ahma1fAMcHALjRcpe93h3T+fc/u1pze8He2pyqlguZk4AAA12aVRYdFhNTDpjb20uYWRvYm"
	+"UueG1wAAAAAAA8P3hwYWNrZXQgYmVnaW49Iu+7vyIgaWQ9Ilc1TTBNcENlaGlIenJlU3pOVGN6a2M5ZCI/Pgo8eDp4bXBtZX"
	+"RhIHhtbG5zOng9ImFkb2JlOm5zOm1ldGEvIiB4OnhtcHRrPSJYTVAgQ29yZSA0LjQuMC1FeGl2MiI+CiA8cmRmOlJERiB4bW"
	+"xuczpyZGY9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkvMDIvMjItcmRmLXN5bnRheC1ucyMiPgogIDxyZGY6RGVzY3JpcHRpb2"
	+"4gcmRmOmFib3V0PSIiCiAgICB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyIKICAgIHhtbG"
	+"5zOnN0RXZ0PSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvc1R5cGUvUmVzb3VyY2VFdmVudCMiCiAgICB4bWxuczpkYz"
	+"0iaHR0cDovL3B1cmwub3JnL2RjL2VsZW1lbnRzLzEuMS8iCiAgICB4bWxuczpHSU1QPSJodHRwOi8vd3d3LmdpbXAub3JnL3"
	+"htcC8iCiAgICB4bWxuczp0aWZmPSJodHRwOi8vbnMuYWRvYmUuY29tL3RpZmYvMS4wLyIKICAgIHhtbG5zOnhtcD0iaHR0cD"
	+"ovL25zLmFkb2JlLmNvbS94YXAvMS4wLyIKICAgeG1wTU06RG9jdW1lbnRJRD0iZ2ltcDpkb2NpZDpnaW1wOmQzZTI2MzgxLT"
	+"gwNTAtNDJhYi1iN2M0LTczNzY1ZTQxNGEwZiIKICAgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDoxMjJhMjQ4MC1kOTNjLT"
	+"Q3MjctYmFjZi05MGY0MTE2MTE5NDQiCiAgIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDozZjEyYmIwZi05NT"
	+"kyLTRmZTItOTMyMi1jODRjZTVhMDdhNjUiCiAgIGRjOkZvcm1hdD0iaW1hZ2UvcG5nIgogICBHSU1QOkFQST0iMi4wIgogIC"
	+"BHSU1QOlBsYXRmb3JtPSJXaW5kb3dzIgogICBHSU1QOlRpbWVTdGFtcD0iMTczMDQyNzc5MzkxMDQyMSIKICAgR0lNUDpWZX"
	+"JzaW9uPSIyLjEwLjM4IgogICB0aWZmOk9yaWVudGF0aW9uPSIxIgogICB4bXA6Q3JlYXRvclRvb2w9IkdJTVAgMi4xMCIKIC"
	+"AgeG1wOk1ldGFkYXRhRGF0ZT0iMjAyNDoxMTowMVQxMDoyMzoxMCswODowMCIKICAgeG1wOk1vZGlmeURhdGU9IjIwMjQ6MT"
	+"E6MDFUMTA6MjM6MTArMDg6MDAiPgogICA8eG1wTU06SGlzdG9yeT4KICAgIDxyZGY6U2VxPgogICAgIDxyZGY6bGkKICAgIC"
	+"Agc3RFdnQ6YWN0aW9uPSJzYXZlZCIKICAgICAgc3RFdnQ6Y2hhbmdlZD0iLyIKICAgICAgc3RFdnQ6aW5zdGFuY2VJRD0ieG"
	+"1wLmlpZDozNTZhMWM2OS05Mjg0LTQ5ZjctYTdkMy1kZTY1ZmFiNmJhZWYiCiAgICAgIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9Ik"
	+"dpbXAgMi4xMCAoV2luZG93cykiCiAgICAgIHN0RXZ0OndoZW49IjIwMjQtMTEtMDFUMTA6MjM6MTMiLz4KICAgIDwvcmRmOl"
	+"NlcT4KICAgPC94bXBNTTpIaXN0b3J5PgogIDwvcmRmOkRlc2NyaXB0aW9uPgogPC9yZGY6UkRGPgo8L3g6eG1wbWV0YT4KIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCi"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIA"
	+"ogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC"
	+"AgCiAgICAgICAgICAgICAgICAgICAgICAgICAgIAo8P3hwYWNrZXQgZW5kPSJ3Ij8+Drcu5wAAAAZiS0dEAP8A/wD/oL2nkw"
	+"AAAAlwSFlzAAAuIwAALiMBeKU/dgAAAAd0SU1FB+gLAQIXDYEZSX4AAAAZdEVYdENvbW1lbnQAQ3JlYXRlZCB3aXRoIEdJTV"
	+"BXgQ4XAAAYpklEQVR42u2deXhV1bn/P2vtM58kJCEkTIYphHlSHGitYh2qdaqiIiCipfanv+deH7331s7W1lZrh9v2/mp/re"
	+"JQVJzrUKnVKiJFoaIgQ5jCPCSEACHjmfZe6/6RRFMKa58DISfI/j7PeU5ysrLWPnt997ve913vel8B5OLhpIX0boFHAA8eAT"
	+"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"
	+"Jggg==",
"config/system.png":
   '<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="402.000000pt" height="402.000000pt"'
	+' viewBox="0 0 402.000000 402.000000" preserveAspectRatio="xMidYMid meet">'
	+'<g transform="translate(0.000000,402.000000) scale(0.100000,-0.100000)" fill="#000000" stroke="none">'
	+'<path d="M1893 3618 c-60 -43 -67 -66 -73 -248 -4 -146 -8 -170 -28 -212 -74 -150 -244 -219 -402 -162 '
	+'-44 15 -76 39 -165 125 -60 58 -120 111 -132 118 -30 15 -98 14 -131 -3 -47 -24 -120 -110 -132 -155 '
	+'-18 -68 0 -102 115 -221 127 -132 150 -175 150 -285 0 -60 -6 -91 -22 -127 -30 -66 -96 -133 -162 -164 '
	+'-52 -25 -68 -28 -218 -32 -189 -5 -212 -14 -245 -89 -27 -61 -23 -178 8 -219 48 -65 61 -69 242 -74 190 '
	+'-6 231 -19 307 -94 64 -64 89 -126 89 -221 1 -115 -25 -165 -157 -302 -114 -118 -126 -143 -107 -214 15 '
	+'-56 101 -143 158 -160 75 -22 112 -3 234 121 122 124 171 150 281 150 87 0 151 -25 219 -88 83 -78 98 '
	+'-123 98 -305 0 -128 2 -147 23 -187 32 -63 65 -83 142 -88 35 -2 80 -1 99 2 41 8 93 48 109 85 6 14 13 '
	+'100 17 191 5 152 8 169 32 215 35 67 90 118 162 151 52 23 67 26 140 22 109 -6 144 -26 269 -152 117 -119 '
	+'155 -139 228 -117 57 17 143 103 160 159 21 71 3 105 -113 224 -128 131 -153 179 -152 293 0 96 26 158 91 '
	+'221 77 76 115 87 305 93 181 6 194 11 242 75 30 40 35 177 8 226 -38 68 -55 74 -235 81 -115 5 -176 11 '
	+'-209 24 -66 23 -141 94 -175 163 -24 48 -28 69 -28 137 0 113 22 154 145 279 150 153 162 208 68 312 -61 '
	+'66 -93 84 -148 84 -58 0 -72 -9 -185 -121 -73 -71 -116 -106 -160 -126 -155 -71 -339 -2 -417 155 -20 42 '
	+'-24 66 -28 212 -5 149 -7 169 -28 201 -32 53 -74 69 -174 69 -70 0 -88 -3 -115 -22z m335 -931 c105 -37 '
	+'176 -82 257 -163 79 -79 142 -186 171 -294 28 -103 24 -264 -9 -365 -38 -114 -90 -196 -179 -280 -151 '
	+'-142 -351 -206 -546 -176 -144 22 -250 74 -358 174 -377 350 -219 983 279 1118 106 28 284 22 385 -14z"/>'
	+'<path d="M1941 2308 c-72 -20 -144 -91 -170 -167 -31 -92 2 -208 78 -272 48 -42 103 -60 176 -60 136 0 '
	+'245 114 245 256 -1 170 -164 291 -329 243z"/>'
	+'</g>'
	+'</svg>',
"config/upgrade.png":
   '<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="474.000000pt" height="474.000000pt"'
	+' viewBox="0 0 474.000000 474.000000" preserveAspectRatio="xMidYMid meet">'
	+'<g transform="translate(0.000000,474.000000) scale(0.100000,-0.100000)" fill="#000000" stroke="none">'
	+'<path d="M2154 4146 c-663 -96 -1215 -525 -1460 -1136 -74 -184 -134 -467 -134 -632 l0 -38 -281 0 -281 '
	+'0 64 -67 c36 -38 147 -158 247 -268 664 -727 827 -904 833 -904 3 -1 73 76 155 171 170 196 680 784 758 '
	+'873 29 33 77 89 108 125 l55 65 -293 3 -294 2 5 33 c87 479 375 917 784 1192 108 72 317 174 440 214 118 '
	+'39 307 78 424 87 102 8 102 9 -19 72 -181 94 -332 148 -520 188 -149 31 -444 41 -591 20z"/>'
	+'<path d="M3404 3364 c-104 -119 -223 -256 -264 -304 -41 -47 -142 -164 -225 -260 -82 -95 -205 -236 -272 '
	+'-314 l-122 -141 294 -3 294 -2 -5 -33 c-71 -391 -300 -793 -599 -1048 -301 -257 -673 -414 -1050 -444 '
	+'-108 -9 -109 -7 80 -100 230 -113 416 -165 672 -187 236 -20 491 11 721 88 671 225 1154 819 1238 1524 '
	+'7 63 14 134 14 158 l0 42 281 0 281 0 -55 58 c-30 31 -184 199 -342 372 -159 173 -334 365 -390 425 -56 '
	+'61 -157 171 -225 245 -67 74 -126 136 -130 138 -4 2 -93 -95 -196 -214z"/>'
	+'</g>'
	+'</svg>',
"config/config.png":
   '<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="511.000000pt" height="511.000000pt" '
	+'viewBox="0 0 511.000000 511.000000" preserveAspectRatio="xMidYMid meet">'
	+'<g transform="translate(0.000000,511.000000) scale(0.100000,-0.100000)" fill="#000000" '
	+'stroke="none"><path d="M2723 4500 c-45 -27 -56 -54 -97 -240 -21 -90 -39 -166 -41 -168 -2 -2 -53 -8 '
	+'-113 -12 -60 -5 -136 -14 -171 -21 -34 -6 -63 -10 -65 -8 -2 2 -29 84 -60 181 -61 192 -70 207 -148 '
	+'230 -34 10 -50 8 -116 -11 -133 -39 -240 -90 -269 -128 -38 -49 -33 -90 35 -305 l59 -188 -53 -40 -53 '
	+'-40 -153 140 c-164 150 -195 168 -258 150 -37 -10 -122 -89 -221 -203 -53 -61 -59 -72 -59 -112 0 -24 '
	+'5 -55 11 -69 7 -14 83 -91 170 -170 l158 -146 -50 -97 c-27 -53 -51 -98 -52 -100 -2 -1 -93 18 -202 42 '
	+'-110 25 -212 45 -228 45 -38 0 -95 -28 -112 -54 -18 -28 -50 -150 -70 -264 -15 -89 -15 -96 2 -130 30 '
	+'-60 56 -71 284 -122 l214 -47 6 -59 c4 -32 7 -82 8 -110 l1 -50 -207 -65 c-273 -84 -301 -108 -281 -239 '
	+'10 -61 73 -253 95 -287 18 -27 72 -53 112 -53 17 0 123 29 235 65 112 36 207 65 210 65 3 0 24 -32 47 '
	+'-70 l41 -70 -108 -118 c-60 -64 -130 -140 -156 -168 -54 -60 -71 -107 -56 -162 7 -28 37 -64 121 -146 '
	+'122 -120 158 -141 224 -131 41 6 53 17 195 171 83 90 156 164 161 164 4 0 27 -13 50 -29 23 -15 75 -45 '
	+'115 -65 56 -28 72 -41 68 -54 -3 -9 -23 -100 -46 -202 -69 -308 -62 -320 234 -380 118 -24 138 -25 181 '
	+'-4 47 22 70 75 110 259 19 88 38 170 40 183 5 21 10 23 73 20 l67 -3 57 -184 c32 -101 66 -197 76 -215 '
	+'9 -17 35 -40 56 -50 38 -19 42 -19 129 -2 146 28 247 64 284 101 49 49 46 92 -26 313 -75 234 -102 279 '
	+'-290 496 l-95 109 -71 -9 c-247 -30 -454 12 -657 131 -257 150 -432 447 -454 768 -22 325 155 671 439 '
	+'854 393 255 939 177 1252 -177 154 -175 244 -470 223 -731 -4 -51 -6 -94 -5 -95 1 0 112 -97 247 -215 '
	+'l245 -214 132 -29 c149 -32 186 -30 231 12 34 32 42 56 78 231 27 131 25 164 -8 206 -30 38 -60 50 -218 '
	+'85 -80 18 -148 35 -151 39 -4 3 -9 65 -11 137 l-4 132 141 42 c171 53 191 62 215 103 35 57 22 152 -42 '
	+'325 -23 61 -40 90 -63 108 -55 41 -79 40 -246 -11 -85 -26 -157 -49 -161 -51 -3 -2 -28 31 -53 73 -26 '
	+'43 -67 103 -91 134 l-43 57 105 113 c134 144 155 190 120 257 -24 45 -204 209 -263 241 -30 15 -88 14 '
	+'-121 -3 -15 -8 -77 -67 -137 -133 -60 -65 -113 -120 -118 -122 -12 -5 -187 81 -187 92 0 5 16 78 35 '
	+'163 19 84 35 163 35 175 0 35 -46 105 -77 117 -76 29 -271 72 -328 72 -16 0 -44 -9 -62 -20z"/>'
	+'<path d="M2535 3311 c-112 -14 -106 -31 84 -225 87 -90 161 -171 165 -182 11 -36 56 -267 56 -288 0 '
	+'-15 -43 -65 -130 -150 l-130 -129 -42 6 c-24 3 -95 18 -158 33 l-115 26 -166 164 c-160 158 -196 184 '
	+'-214 153 -16 -25 -24 -157 -15 -238 23 -201 122 -387 264 -495 132 -100 312 -137 559 -115 131 12 139 '
	+'12 162 -7 21 -17 145 -160 723 -834 232 -270 265 -304 329 -346 249 -162 590 0 632 299 20 141 -23 259 '
	+'-136 368 -37 37 -297 268 -578 514 -280 247 -515 456 -522 465 -10 14 -9 43 3 141 24 193 3 371 -58 '
	+'494 -54 106 -200 236 -329 293 -100 43 -272 67 -384 53z m1668 -2202 c72 -77 -8 -202 -109 -169 -94 '
	+'31 -101 156 -10 196 42 18 85 9 119 -27z"/>'
	+'</g>'
	+'</svg>'
}
function getIconSvg(n)
{
  return __icons[n] ? getSvgDataURI(__icons[n]) : n;
}
