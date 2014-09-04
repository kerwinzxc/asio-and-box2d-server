// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <set>
#include <atomic>

#include <tbb\concurrent_queue.h>
#include <boost\asio.hpp>
#include <boost\thread\thread.hpp>
#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\make_shared.hpp>
#include <boost\enable_shared_from_this.hpp>
#include <boost\function.hpp>
#include <boost\date_time\posix_time\posix_time.hpp>
#include <boost\coroutine\all.hpp>
#include <boost\mpl\list.hpp>


#include <boost\statechart\state_machine.hpp>
#include <boost\statechart\simple_state.hpp>
#include <boost\statechart\transition.hpp>
#include <boost\statechart\custom_reaction.hpp>
#include <boost\statechart\event.hpp>
#include <boost\statechart\deferral.hpp>
#include <boost\uuid\uuid.hpp>
#include <boost\uuid\uuid_generators.hpp>
#include <boost\uuid\uuid_io.hpp>


namespace sc = boost::statechart;
namespace mpl = boost::mpl;

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


#include <Box2D/Box2D.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
