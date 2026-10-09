Name: python3-module-twitchio
Version: 3.3.2
Release: alt1

Summary: A powerful, asynchronous Python library for twitch.tv
Summary(ru_RU.UTF-8): Мощная асинхронная библиотека Python для twitch.tv

License: MIT
Group: Development/Python
Url: https://github.com/TwitchIO/TwitchIO
# Upstream sdist from PyPI is named twitchio-VERSION.tar.gz (no
# "python3-module-" prefix). Source: intentionally does NOT use
# %%name here -- that would resolve to
# python3-module-twitchio-VERSION.tar.gz, which does not match the
# file actually published on PyPI.
Source: twitchio-%{version}.tar.gz

BuildArch: noarch

BuildRequires: rpm-build-python3
BuildRequires: python3-module-setuptools
BuildRequires: python3-module-wheel

# Runtime dependency declared in upstream requirements.txt
# (aiohttp>=3.9.1,<4). Assumes python3-module-aiohttp is already
# packaged in the ALT repository, which it is.
Requires: python3-module-aiohttp >= 3.9.1

%description
TwitchIO is an asynchronous Python wrapper around the Twitch API and
EventSub/IRC systems, with a command-extension module and websocket/IRC
support, built on asyncio.

This package provides the twitchio module for python3, as packaged for
ALT Linux.

%description -l ru_RU.UTF-8
TwitchIO - это асинхронная библиотека Python для работы с Twitch API,
EventSub и IRC, включающая модуль команд и поддержку websocket/IRC на
основе asyncio.

Этот пакет содержит модуль twitchio для python3, собранный для ALT
Linux.

%prep
# Upstream tarball extracts into twitchio-VERSION/, not
# python3-module-twitchio-VERSION/, since Source: above is the
# unmodified upstream sdist.
%setup -n twitchio-%{version}

%build
%py3_build

%install
%py3_install

# Upstream ships a py.typed marker (PEP 561) alongside the package;
# %%py3_install above already installs it as part of package data, so
# nothing extra is needed here.

%files
%doc README.md
%doc LICENSE
%python3_sitelibdir/twitchio
%python3_sitelibdir/twitchio-%{version}*.egg-info

%changelog
* Tue Oct 07 2025 Packager <packager@altlinux.org> 3.3.2-alt1
- Initial build for ALT Linux Sisyphus.
- Packaged from PyPI sdist twitchio-3.3.2.tar.gz.
