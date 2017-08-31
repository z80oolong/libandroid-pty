# "libandroid-pty.so" -- 擬似端末の問題を回避する動的ライブラリ

## 概要

Debian noroot 等のような Android OS 上で chroot を用いて構築されている Linux 環境において、一部の擬似端末を利用したソフトウェアがデバイスファイル /dev/ptmx を経由せずに擬似端末にアクセスしている為に正常に動作しない問題が発生しています。

この問題は、 C 標準ライブラリの擬似端末関連の関数について、 [bionic][1] 由来のコードの関数に置き換えることにより、上記問題を回避することが可能です。

```libandroid-pty.so``` は、 Android OS の C 標準ライブラリである bionic のうち、擬似端末に関係する以下の C++ 言語のソースコードを修正し、完全な C 言語のコードに記述し直したものです。

- [https://raw.githubusercontent.com/android/platform_bionic/30a36273ab761fb07892f7816b4873b6c48a0cea/libc/bionic/pty.cpp][2]
- [https://raw.githubusercontent.com/android/platform_bionic/30a36273ab761fb07892f7816b4873b6c48a0cea/libc/private/ThreadLocalBuffer.h][3]

## インストール及び使用方法

ソースコードのビルド及び ```libandroid-pty.so``` のインストールには、 ```make``` コマンドを使用します。ソースコード ```libandroid-pty.c``` の存在するディレクトリにおいて、 ```make``` コマンドを以下のように実行します。

```
 $ make
 $ make install INSTALL_DIR="/path/to/lib"  ; # ここで、 /path/to/lib は、 libandroid-pty.so のインストール先のディレクトリ。
```

そして次のように、環境変数 LD_PRELOAD に libandroid-pty.so のインストール先のフルパスを設定して、擬似端末を操作するコマンドを起動すると、擬似端末に関する問題を回避することが出来ます。

```
 $ /usr/bin/env LD_PRELOAD="/path/to/lib/libandroid-pty.so" linux_command
```

## 配布条件

```libandroid-pty.so``` は、 [Android Open Source Project][4] によって開発された Android OS の C 標準ライブラリである bionic の一部の関数を [Z.OOL. (mailto:zool@zool.jpn.org)][5] が完全な C 言語に記述し直すことにより作成されたものです。

```libandroid-pty.so``` を構成するソースコードは、 [Android Open Source Project][4] 及び [Z.OOL. (mailto:zool@zool.jpn.org)][5] が著作権を有し、Android Open Source Project による bionic のライセンスと同一の配布条件に従って配布されるものとします。

なお、 ```bionic``` 及び ```libandroid-pty.so``` の配布条件の詳細については、ドキュメント ```COPYING.md``` を御覧下さい。

<!-- 外部リンク一覧 -->

[1]:https://github.com/android/platform_bionic/
[2]:https://raw.githubusercontent.com/android/platform_bionic/30a36273ab761fb07892f7816b4873b6c48a0cea/libc/bionic/pty.cpp
[3]:https://raw.githubusercontent.com/android/platform_bionic/30a36273ab761fb07892f7816b4873b6c48a0cea/libc/private/ThreadLocalBuffer.h
[4]:https://source.android.com/
[5]:http://zool.jpn.org/
