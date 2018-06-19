**Install SDK**

Install sdk following this URL from AGL website.
```
http://docs.automotivelinux.org/docs/getting_started/en/dev/reference/setup-sdk-environment.html
```

**Download source code**

Download launcher source code from AGL gerrit.
```
$ git clone https://gerrit.automotivelinux.org/gerrit/apps/launcher
```

**Build launcher application**

Source SDK envirment and make.
```
$ source /xdt/sdk/environment-setup-<your_target>
$ cd launcher
$ mkdir build
$ cd build
$ qmake ..
$ make
```
**Install launcher wgt**

```
$ scp package/launcher.wgt root@<your_target_ip>
$ ssh root@<your_target_ip>
$ afm-util install launcher
```

Reboot target board and launcher application will start by default.
