# Summary #

[libtcs](http://code.google.com/p/libtcs/source/browse/#svn/trunk) is a system independent C library for reading, creating and modifying [TCS](http://www.tcsub.cn/tcs/spec.htm) files. It is part of the teco project.


# Introduction #

[TCS](http://www.tcsub.cn/tcs/spec.htm) is short for Ternary Color Subtitle, it is specially built for the TCSub, and it’s part of the teco project. [TCS](http://www.tcsub.cn/tcs/spec.htm) is a file format that mainly stores raw pixel data as well as its position and living interval (consider of the GIF file format or any video file format). The default filename extension of a [TCS](http://www.tcsub.cn/tcs/spec.htm) file is .TCS. TCS FX stands for TCS [karaoke effects](http://code.google.com/p/libtcs/wiki/libtcs#Karaoke_effect), which means a karaoke effects that is stored in a [TCS](http://www.tcsub.cn/tcs/spec.htm) file. [TCS](http://www.tcsub.cn/tcs/spec.htm) is very different from the well-known subtitle file format SSA/ASS ([SubStation Alpha](http://en.wikipedia.org/wiki/SubStation_Alpha)/[Advanced SubStation Alpha](http://en.wikipedia.org/wiki/SubStation_Alpha)), it is not written but produced, if you want to create meaningful TCS FX you’ll have to use some special softwares that are compatible with [TCS](http://www.tcsub.cn/tcs/spec.htm). Theoretically, [TCS](http://www.tcsub.cn/tcs/spec.htm) allows you to draw anything to video frames, although it is mainly created to store karaoke effects. After parsing the data stored in [TCS](http://www.tcsub.cn/tcs/spec.htm) file, you’ll get TCS frames, which are going to overlay on video frames. See also [libtcs](http://code.google.com/p/libtcs) and [TcsFilter](http://code.google.com/p/tcsfilter). [Aegisub Manual](http://aegisub.cellosoft.com/docs/Main_Page) is also a good stuff to read.


# Related Projects #

  * [tcsFilter](http://code.google.com/p/tcsfilter)
  * [tcxCore](http://code.google.com/p/tcxcore)


# Programming Languages #
  * [C](http://en.wikipedia.org/wiki/C_(programming_language))