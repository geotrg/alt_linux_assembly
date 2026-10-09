# Target: ALT Linux 10.4 (p10), Python 3.9
# twitchio 3.x needs Python >= 3.11, so the last 2.x release (2.10.0,
# Python 3.7-3.11) is packaged instead.
%define oname twitchio

Name: python3-module-twitchio
Version: 2.10.0
Release: alt1

Summary: Async Python wrapper for the Twitch API, IRC chat and EventSub
License: MIT
Group: Development/Python
Url: https://github.com/TwitchIO/TwitchIO

BuildArch: noarch

Source: %oname-%{version}.tar.gz

BuildRequires: rpm-build-python3
BuildRequires: python3-module-setuptools python3-module-wheel

Requires: python3-module-aiohttp >= 3.6.0
Requires: python3-module-iso8601
Requires: python3-module-typing-extensions

%description
TwitchIO is an asynchronous Python wrapper around the Twitch API and
chat (IRC). It provides a command framework, event handling, PubSub and
EventSub support.

%description -l ru_RU.UTF-8
TwitchIO - асинхронная обёртка на Python для Twitch API и чата (IRC).
Включает фреймворк команд, обработку событий, PubSub и EventSub.

%prep
%setup -n %oname-%{version}
# the 2.10.0 sdist does not ship requirements.txt, but setup.py reads it
cat > requirements.txt <<'REQ'
aiohttp>=3.6.0,<4
iso8601
typing-extensions
REQ

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
* Fri Oct 09 2026 Packager <packager@altlinux.org> 2.10.0-alt1
- Initial build for ALT Linux 10.4 (Python 3.9)
