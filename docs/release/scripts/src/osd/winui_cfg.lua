defines {
	"OSD_WINDOWS",
	"_WIN32_WINNT=0x0501",
	"USE_QTDEBUG=0",
}

configuration { "mingw*-gcc or vs*" }
	defines {
		"UNICODE",
		"_UNICODE",
		"main=utf8_main",
	}

configuration { "Debug" }
	defines {
		"MALLOC_DEBUG",
	}

configuration { "vs*" }
	flags {
		"Unicode",
	}

configuration { }

if not _OPTIONS["DONT_USE_NETWORK"] then
	defines {
		"USE_NETWORK",
		"OSD_NET_USE_PCAP",
	}
end

	defines {
		"USE_SDL=0",
	}

