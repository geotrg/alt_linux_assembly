cat > /root/rpmbuild/SPECS/python3-module-twitchio.spec << 'SPECEOF'
Name: python3-module-twitchio
Version: 2.10.0
Release: alt1

Summary: An asynchronous Python IRC and API wrapper for Twitch
Summary(ru_RU.UTF-8): Асинхронная библиотека Python для IRC и API Twitch

License: MIT
Group: Development/Python
Url: https://github.com/TwitchIO/TwitchIO
# Upstream sdist from PyPI is named twitchio-VERSION.tar.gz (no
# "python3-module-" prefix). Source: intentionally does NOT use
# %%name here -- that would resolve to
# python3-module-twitchio-VERSION.tar.gz, which does not match the
# file actually published on PyPI.
Source: twitchio-%{version}.tar.gz

# --------------------------------------------------------------
# Target: ALT Linux 10.4 (p10 branch), system python3 = 3.9.
#
# twitchio's current release line (3.x, e.g. 3.3.2) declares
# Requires-Python >= 3.11 and will not install/build against
# p10's python3 3.9 at all. Package version is pinned to 2.10.0
# instead -- the last 2.x release, and the newest release of any
# kind that still explicitly lists 3.9 (and 3.7/3.8/3.10/3.11) in
# its trove classifiers, i.e. the newest version actually meant to
# run on this platform's Python.
# --------------------------------------------------------------

BuildArch: noarch

BuildRequires: rpm-build-python3
BuildRequires: python3-module-setuptools
BuildRequires: python3-module-wheel

# Runtime dependencies per PKG-INFO's Requires-Dist (the upstream
# 2.10.0 sdist's own requirements.txt is missing -- see %prep).
# Base install only; the "sounds" and "speed" extras (yt-dlp,
# pyaudio, ujson, ciso8601, cchardet...) are optional and left
# unpackaged here.
Requires: python3-module-aiohttp >= 3.6.0
Requires: python3-module-iso8601
Requires: python3-module-typing-extensions

%description
TwitchIO is an asynchronous Python wrapper around the Twitch API and
IRC/EventSub/PubSub systems, with a command-extension module, built on
asyncio.

This package provides the twitchio module for python3 (3.9), as
packaged for ALT Linux 10.4. Version 2.10.0 was chosen deliberately
over the current 3.x line, which requires Python >= 3.11 and is not
installable against this platform's python3.

%description -l ru_RU.UTF-8
TwitchIO - это асинхронная библиотека Python для работы с Twitch API,
IRC, EventSub и PubSub, включающая модуль команд, на основе asyncio.

Этот пакет содержит модуль twitchio для python3 (3.9), собранный для
ALT Linux 10.4. Версия 2.10.0 выбрана намеренно вместо текущей ветки
3.x, которая требует Python >= 3.11 и не устанавливается с python3
данной платформы.

%prep
# Upstream tarball extracts into twitchio-VERSION/, not
# python3-module-twitchio-VERSION/, since Source: above is the
# unmodified upstream sdist.
%setup -n twitchio-%{version}

# Known defect in the published 2.10.0 sdist: setup.py does
#   with open("requirements.txt") as f: ...
# but requirements.txt itself is NOT included in the tarball (verified
# by inspecting its contents directly), so a plain build fails with
# FileNotFoundError before even reaching %%build. Recreate it here
# from PKG-INFO's Requires-Dist (the base/required deps only -- not
# the optional sounds/speed extras) so setup.py's read succeeds with
# the correct, upstream-published dependency list.
cat > requirements.txt << 'EOF'
aiohttp>=3.6.0,<4
iso8601
typing-extensions
EOF

%build
%py3_build

%install
%py3_install

%files
%doc README.rst
%doc LICENSE
%python3_sitelibdir/twitchio
%python3_sitelibdir/twitchio-%{version}*.egg-info

%changelog
* Wed Oct 08 2025 Packager <packager@altlinux.org> 2.10.0-alt1
