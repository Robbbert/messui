defines {
	"OSD_WINDOWS",
	"_WIN32_WINNT=0x0501",
	"USE_QTDEBUG=0",
	"WIN32_LEAN_AND_MEAN",
	"NOMINMAX",
}

configuration { "mingw*-gcc or vs*" }
	defines {
		"UNICODE",
		"_UNICODE",
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

if _OPTIONS["USE_TAPTUN"]=="1" or _OPTIONS["USE_PCAP"]==1 then
	defines {
		"USE_NETWORK",
	}
	if _OPTIONS["USE_TAPTUN"]=="1" then
		defines {
			"OSD_NET_USE_TAPTUN",
		}
	end
	if _OPTIONS["USE_PCAP"]=="1" then
		defines {
			"OSD_NET_USE_PCAP",
		}
	end
end

	defines {
		"USE_SDL=0",
	}

