# Introduction #

Crossed Words build on Windows is a little bit tricky. This page explains how to build Crosed Words on Windows step-by-step.


# Download necessary software #

  * Get Tortoise SVN (http://tortoisesvn.tigris.org/)
  * Get GTK+ development libraries from http://www.gtk.org/download-windows.html - grab "bundle" package and unzip it into "C:\gtk\"
  * Get Code::Blocks IDE. Select package with MinGW compiler: http://www.codeblocks.org/


# Get sources from Subversion Repository #

In some folder, right click, select "SVN Checkout..." Enter URL: http://crossed-words.googlecode.com/svn/trunk/


# Launch Code::Blocks #

Launch Code::Blocks and open crossed-words.cbp, then click "Build" icon.


# Get data files #

Copy DLL and data files from binary Crossed Words release to bin\release folder.