# aptback

[Download aptback v1.0.0](https://github.com/carles-garcia)

A tool to search, install, remove and upgrade packages logged by apt.


#### Install

`make install`

It is installed by default in /usr/local

#### Examples

To see all options type `aptback --help`

The following examples show the long and short version of the commands.

* Show apt log in 2016

`aptback -d 2016`

* Show upgraded packages on February 25th 2016 at 12:00

`aptback --select upgraded --date 2016-2-25-12`

`aptback -s u -d 2016-2-31-12`

* Install packages removed or purged in December 2015

`aptback install --select removed,purged --date 2015-12`

`aptback install -s r,p -d 2015-12`

* Remove packages installed between two days (these two included)

`aptback remove --select installed --date 2016-1-1 --until 2016-1-14`

`aptback remove -s i -d 2016-1-1 -u 2016-1-14`

* Downgrade packages upgraded on a specific date

`aptback --select upgraded --date 2016-2-15 --export-version | xargs apt-get install`

`aptback -s u -d 2016-2-15 -v | xargs apt-get install`

* If you want to use advanced apt-get/apt-cache options, you can export the output of aptback and use xargs like this

`aptback -s u,i -d 2016 -e | xargs apt-get purge`


###### License

GNU General Public License v3

![GPLv3 Logo](http://www.gnu.org/graphics/gplv3-127x51.png "GPLv3 Logo")