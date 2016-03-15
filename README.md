# aptback

[Download aptback v1.0.0](https://github.com/carles-garcia/aptback/releases/tag/v1.0.0)

A tool to search, install, remove and upgrade packages logged by apt.

aptback can show the apt log in a easy to read format and sorted by date.


#### Install

`make install`

It is installed by default in /usr/local

#### Examples

To see all options type `aptback --help`

The following examples show the long and short version of the commands.

###### Show apt log in 2016

`aptback -d 2016`

Output:

```
2016-02-27  18:06  install    vim  amd64  (7.4.488-7)
2016-02-27  18:06  install    vim-runtime  amd64  (7.4.488-7)  automatic
2016-02-29  23:27  install    libcommons-dbcp-java  amd64  (1.4-5)  automatic
2016-02-29  23:27  install    tomcat8-common  amd64  (8.0.14-1+deb8u1)  automatic
2016-02-29  23:27  install    libcommons-pool-java  amd64  (1.6-2)  automatic
2016-02-29  23:27  install    libtomcat8-java  amd64  (8.0.14-1+deb8u1)  automatic
2016-02-29  23:27  install    tomcat8  amd64  (8.0.14-1+deb8u1)
2016-02-29  23:27  install    authbind  amd64  (2.1.1)  automatic
2016-02-29  23:27  install    libecj-java  amd64  (3.10.1-1)  automatic
2016-03-01  10:23  install    libchm1  amd64  (0.40a-3+b1)  automatic
2016-03-01  10:23  install    chm2pdf  amd64  (0.9.1-1.1)
2016-03-01  10:23  install    htmldoc  amd64  (1.8.27-8+b1)  automatic
2016-03-01  10:23  install    libchm-bin  amd64  (0.40a-3+b1)  automatic
2016-03-01  10:23  install    python-chm  amd64  (0.8.4.1-1)  automatic
2016-03-01  10:23  install    htmldoc-common  amd64  (1.8.27-8)  automatic
2016-03-01  10:23  install    libfltk1.1  amd64  (1.1.10-19+b1)  automatic
2016-03-01  10:25  remove    chm2pdf  amd64  (0.9.1-1.1)
2016-03-01  10:25  remove    libchm1  amd64  (0.40a-3+b1)
2016-03-01  10:25  remove    htmldoc  amd64  (1.8.27-8+b1)
2016-03-01  10:25  remove    libchm-bin  amd64  (0.40a-3+b1)
2016-03-01  10:25  remove    python-chm  amd64  (0.8.4.1-1)
2016-03-01  10:25  remove    htmldoc-common  amd64  (1.8.27-8)
2016-03-01  10:25  remove    libfltk1.1  amd64  (1.1.10-19+b1)
2016-03-01  10:26  install    libchm1  amd64  (0.40a-3+b1)  automatic
2016-03-01  10:26  install    kchmviewer  amd64  (6.0-1)
2016-03-02  09:19  upgrade    perl  amd64  (5.20.2-3+deb8u3)-->(5.20.2-3+deb8u4)
2016-03-02  09:19  upgrade    libssl1.0.0  amd64  (1.0.1k-3+deb8u2)-->(1.0.1k-3+deb8u4)
2016-03-02  09:19  upgrade    perl-base  amd64  (5.20.2-3+deb8u3)-->(5.20.2-3+deb8u4)
2016-03-02  09:19  upgrade    libperl5.20  amd64  (5.20.2-3+deb8u3)-->(5.20.2-3+deb8u4)
2016-03-02  09:19  upgrade    perl-modules  amd64  (5.20.2-3+deb8u3)-->(5.20.2-3+deb8u4)
2016-03-02  09:19  upgrade    openssl  amd64  (1.0.1k-3+deb8u2)-->(1.0.1k-3+deb8u4)
```

###### Show upgraded packages on February 25th 2016 at 12:00

`aptback --select upgraded --date 2016-2-25-12`

`aptback -s u -d 2016-2-31-12`

###### Install packages removed or purged in December 2015

`sudo aptback install --select removed,purged --date 2015-12`

`sudo aptback install -s r,p -d 2015-12`

###### Remove packages installed between two days (these two included)

`sudo aptback remove --select installed --date 2016-1-1 --until 2016-1-14`

`sudo aptback remove -s i -d 2016-1-1 -u 2016-1-14`

###### Downgrade packages upgraded on a specific date

```
aptback --select upgraded --date 2016-2-15 --export-version > list 
xargs --arg-file list sudo apt-get install
```

```
aptback -s u -d 2016-2-15 -v > list
xargs -a list sudo apt-get install
```

###### If you want to use advanced apt-get/apt-cache options, you can export the output of aptback and use xargs like this

```
aptback -s u -d 2016-2 -e > list
xargs -a list sudo apt-get purge
```


#### License

GNU General Public License v3

![GPLv3 Logo](http://www.gnu.org/graphics/gplv3-127x51.png "GPLv3 Logo")
