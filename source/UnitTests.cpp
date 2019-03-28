#include "Assert.h"
#include "MutexProtectedResource.h"
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <cctype>
#include <stdint.h>
#include "DebugLog.h"
#include "AtScopeExit.h"
#include "Hash.h"
#include "CacheServer.h"
#include "WorkerPoolServer.h"
#include "ArgParser.h"

#include "Master.inl"
#include "Worker.inl"

BOOL MutexProtectedResourceCloseHandle(HANDLE handle)
{
	return CloseHandle(handle);
}

std::vector<char> ReadBinaryFile(const char* filename)
{
	HANDLE fh = worker::CreateFileA( filename,
		GENERIC_READ, 0, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	EXPECT_NE(fh, INVALID_HANDLE_VALUE);

	std::vector<char> result(worker::GetFileSize(fh, NULL));

	EXPECT_EQ(TRUE, worker::ReadFile(fh, result.data(), (DWORD)result.size(), NULL, NULL));

	EXPECT_EQ(TRUE, worker::CloseHandle(fh));

	return result;
}

void WriteBinaryFile(const std::vector<char>& data, const char* filename)
{
	HANDLE fh = worker::CreateFileA( filename,
		GENERIC_WRITE, 0, NULL,	CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	EXPECT_NE(fh, INVALID_HANDLE_VALUE);

	EXPECT_EQ(TRUE, worker::WriteFile( fh, data.data(), (DWORD)data.size(), NULL, NULL));

	EXPECT_EQ(TRUE, worker::CloseHandle(fh));
}

void TestSHAHash()
{
	const char test_data_buffer1[] = "This is a zither, what the heck is going on, blah blah big text block neato burrito";
	const char test_data_buffer2[] = "This is a zither, what the heck is going on, blah blah big text block neato burrito!";
	const char test_data_buffer3[] = "This is a zither, what the heck is going on, blah blah big text block neato burrito";

	Hash hash1 = HashBytes(test_data_buffer1, sizeof(test_data_buffer1));
	Hash hash2 = HashBytes(test_data_buffer2, sizeof(test_data_buffer2));
	Hash hash3 = HashBytes(test_data_buffer3, sizeof(test_data_buffer3));

	EXPECT_NE(hash1, hash2);
	EXPECT_EQ(hash1, hash3);
}

void TestHashServer()
{
	DebugLog("TestHashServer\n");

	SocketCreateListenerResults listener = SocketCreateListener();
	ASSERT(listener.socket != INVALID_SOCKET, "Coudn't create listener : %s\n", WSALastErrorToString().c_str());
	AT_SCOPE_EXIT([&](){ closesocket(listener.socket); });

	cache::Server* server = cache::ServerCreate();
	AT_SCOPE_EXIT([server]{ ServerDestroy(server); });

	std::thread server_thread(cache::ServerRun, server, listener.socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // give it time to start the other thread

	std::thread worker_thread([server, &listener](){

		SOCKET sock = SocketConnectToAddress(listener.address, listener.port);
		AT_SCOPE_EXIT([sock](){ closesocket(sock); });

		ASSERT(sock != INVALID_SOCKET);

		char neato_compiler_contents[] = "open(sys.argv[1], 'wb').write(' '.join([open(x, 'rb').read() for x in sys.argv[2:]]) + '!')";
		char neato_output[] = "Neato Burrito!";
		char command_line[] = "python concat.py ${TGT} ${SRC}";

		Hash neato_compiler_hash = HashString(neato_compiler_contents);
		Hash command_line_hash = HashString(command_line);

		std::vector<cache::FileHash> source = {
			cache::FileHash("input0.txt", HashString("Neato")),
			cache::FileHash("input1.txt", HashString("Burrito")),
		};

		std::vector<std::vector<cache::FileHash>> dependencies;

		const Hash explicit_hash = cache::CreateExplicitHash(command_line, source);

		EXPECT_EQ(cache::RequestTaskDependencies(
			sock, 
			explicit_hash,
			dependencies), cache::RequestResult::CacheMiss);

		// so not found, pretend we built it

		std::vector<cache::FileHash> implicit_dependencies = {
			cache::FileHash("concat.py", HashString(neato_compiler_contents))
		};

		const Hash implicit_hash = cache::CreateImplicitHash(implicit_dependencies);

		std::vector<cache::FileData> built_outputs = {
			cache::FileData("output.txt", HashString(neato_output), {(const uint8_t*)neato_output, (const uint8_t*)neato_output + sizeof(neato_output)}),
		};

		EXPECT_EQ(cache::StoreTaskOutputs(
			sock,
			explicit_hash,
			implicit_hash,
			implicit_dependencies,
			built_outputs
		), cache::StoreTaskOutputsResult::Success);

		// now try to retrieve it from the cache again
		EXPECT_EQ(cache::RequestTaskDependencies(
			sock, 
			explicit_hash,
			dependencies), cache::RequestResult::CacheHit);

		EXPECT_EQ(1, dependencies.size());
		EXPECT_EQ(1, dependencies[0].size());

		const Hash cache_implict_dep_hash = cache::CreateImplicitHash(dependencies[0]);

		EXPECT_EQ(implicit_hash, cache_implict_dep_hash);

		std::vector<cache::FileHash> outputs;
		EXPECT_EQ(cache::RequestTaskOutputs(
			sock,
			explicit_hash,
			implicit_hash,
			outputs), cache::RequestResult::CacheHit);

		// neato, now try to get the output files
		EXPECT_EQ(outputs.size(), 1);

		cache::FileData cache_result;
		EXPECT_EQ(cache::RequestFile(
			sock,
			outputs[0],
			cache_result
		), cache::RequestResult::CacheHit);

		EXPECT_EQ(cache_result.bytes.size(), built_outputs[0].bytes.size());
		EXPECT_EQ(0, memcmp(cache_result.bytes.data(), built_outputs[0].bytes.data(), cache_result.bytes.size()));

		// make sure cache misses work w/ different explicit hash
		EXPECT_EQ(cache::RequestTaskDependencies(
			sock, 
			HashString("A way different command line"),
			dependencies), cache::RequestResult::CacheMiss);

		EXPECT_EQ(dependencies.size(), 0);

		// make sure cache misses work w/ different implicit hash
		EXPECT_EQ(cache::RequestTaskOutputs(
			sock,
			explicit_hash,
			HashString("A way different implicit hash"),
			outputs), cache::RequestResult::CacheMiss);
		EXPECT_EQ(outputs.size(), 0);

		// register us with the cache server

	});

	worker_thread.join();

	closesocket(listener.socket);

	server_thread.join();
}

void TestWorkerPool()
{
	DebugLog("TestWorkerPool\n");

	SocketCreateListenerResults listener = SocketCreateListener();
	ASSERT(listener.socket != INVALID_SOCKET, "TestWorkerPool Coudn't create listener : %s\n", WSALastErrorToString().c_str());
	AT_SCOPE_EXIT([&](){ closesocket(listener.socket); });

	worker_pool::Server* server = worker_pool::ServerCreate();
	AT_SCOPE_EXIT([server]{ ServerDestroy(server); });

	std::thread server_thread(worker_pool::ServerRun, server, listener.socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // give it time to start the other thread

	std::thread worker_thread([server, &listener](){

		SOCKET sock = SocketConnectToAddress(listener.address, listener.port);
		AT_SCOPE_EXIT([sock](){ closesocket(sock); });

		SocketCreateListenerResults worker_listener = SocketCreateListener();
		ASSERT(worker_listener.socket != INVALID_SOCKET, "TestWorkerPool Worker Coudn't create listener : %s\n", WSALastErrorToString().c_str());
		AT_SCOPE_EXIT([&](){ closesocket(worker_listener.socket); });

		EXPECT_EQ(worker_pool::RegisterWorker(sock, worker_listener.address, worker_listener.port), worker_pool::Result::Success);

		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // give it time to actually register

		EXPECT_EQ(1, ServerGetWorkerCount(server));

	});

	worker_thread.join();

	closesocket(listener.socket);

	server_thread.join();

	// should have auto-removed us
	EXPECT_EQ(0, ServerGetWorkerCount(server));
}

void TestApiRedirection(const char *filename)
{
	DebugLog("TestApiRedirection\n");

	SocketCreateListenerResults listener = SocketCreateListener(0, AF_INET);
	ASSERT(listener.socket != INVALID_SOCKET, "Coudn't create listener : %s\n", WSALastErrorToString().c_str());

	// start the listener thread
	std::thread server_thread(master::RunServer, listener.socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // give it time to start the other thread

	// these should be separate processes outside of unit tests
	std::thread worker_thread([listener, filename](){

		// setup slave socket
		worker::g_handle_table.Initialize({});
		worker::g_socket.Initialize(SocketConnectToAddress(listener.address, listener.port));

		EXPECT_NE(*worker::g_socket.lock(), INVALID_SOCKET);

		char lowercase_text[] = "this is a bunch of stuff all lowercase";
		std::vector<char> lowercase_data(sizeof(lowercase_text) - 1);
		memcpy(lowercase_data.data(), lowercase_text, sizeof(lowercase_text) - 1);

		WriteBinaryFile(lowercase_data, filename);

		std::vector<char> data = ReadBinaryFile(filename);

		for (char& c : data)
		{
			c = char(std::toupper(c));
		}

		WriteBinaryFile(data, filename);
	});

	worker_thread.join();

	closesocket(listener.socket);

	server_thread.join();

}

void TestPackBuffer()
{

	uint8_t a = 1;
	uint16_t b = 2;
	uint32_t c = 3;

	struct big_struct { char data[128] = {4}; } d;

	SendBuffer sb(INVALID_SOCKET);
	sb.pack(a, b, c, d);

	EXPECT_EQ(sb.offset, sizeof(a) + sizeof(b) + sizeof(c) + sizeof(d));

}

void TestArgParser()
{
	{
		const char* int_args[] = {"UnitTests.exe", "-i", "314"};
		int32_t ival = -1;
		EXPECT_EQ(true, ParseArgs(_countof(int_args), int_args, {
			{ &ival, Arg::Required, 'i', "input", "help str!"}
		}));
		EXPECT_EQ(314, ival);
		

		const char* uint_args[] = {"UnitTests.exe", "-i", "314"};
		uint32_t uval = -1;
		EXPECT_EQ(true, ParseArgs(_countof(uint_args), uint_args, {
			{ &uval, Arg::Required, 'i', "input", "help str!"}
		}));
		EXPECT_EQ(314, uval);


		const char* float_args[] = {"UnitTests.exe", "-i", "3.14"};
		float fval = 1;
		EXPECT_EQ(true, ParseArgs(_countof(float_args), float_args, {
			{ &fval, Arg::Required, 'i', "input", "help str!"}
		}));
		EXPECT_EQ(3.14f, fval);


		const char* flag_args[] = {"UnitTests.exe", "-i"};
		bool bval = false;
		EXPECT_EQ(true, ParseArgs(_countof(flag_args), flag_args, {
			{ &bval, Arg::Required, 'i', "input", "help str!"}
		}));
		EXPECT_EQ(true, bval);		
	}

	{
		const char* int_args[] = {"UnitTests.exe", "--input", "314"};
		int32_t ival = -1;
		EXPECT_EQ(true, ParseArgs(_countof(int_args), int_args, {
			{ &ival, Arg::Required, 'i', "input", "help str!"}
		}));
		EXPECT_EQ(314, ival);
		

		const char* uint_args[] = {"UnitTests.exe", "--input", "314"};
		uint32_t uval = -1;
		EXPECT_EQ(true, ParseArgs(_countof(uint_args), uint_args, {
			{ &uval, Arg::Required, 'i', "input", "help str!"}
		}));
		EXPECT_EQ(314, uval);


		const char* float_args[] = {"UnitTests.exe", "--input", "3.14"};
		float fval = 1;
		EXPECT_EQ(true, ParseArgs(_countof(float_args), float_args, {
			{ &fval, Arg::Required, 'i', "input", "help str!"}
		}));
		EXPECT_EQ(3.14f, fval);


		const char* flag_args[] = {"UnitTests.exe", "--input"};
		bool bval = false;
		EXPECT_EQ(true, ParseArgs(_countof(flag_args), flag_args, {
			{ &bval, Arg::Required, 'i', "input", "help str!"}
		}));
		EXPECT_EQ(true, bval);			
	}

}

// arguments are input, output
int main(int argc, const char** argv)
{
	TestArgParser();

	bool test_sha_hash = false;
	bool test_pack_buffer = false;
	bool test_hash_server = false;
	bool test_worker_pool = false;
	bool test_api_redirection = false;

	const char* debug_filename = "test.txt";

	if (!ParseArgs(argc, argv, {
			{ &debug_filename, Arg::Optional, 0, nullptr, "Filename used for testing api redirection." },
			{ &test_sha_hash, Arg::Optional, 0, "sha-hash-test", "Only run TestSHAHash." },
			{ &test_pack_buffer, Arg::Optional, 0, "pack-buffer-test", "Only run TestPackBuffer." },
			{ &test_hash_server, Arg::Optional, 0, "hash-server-test", "Only run TestHashServer." },
			{ &test_worker_pool, Arg::Optional, 0, "worker-pool-test", "Only run TestWorkerPool." },
			{ &test_api_redirection, Arg::Optional, 0, "api-redirection-test", "Only run TestApiRedirection." },
		}))
	{
		return 1;
	}

	if ((false
		| test_sha_hash
		| test_pack_buffer
		| test_hash_server
		| test_worker_pool
		| test_api_redirection) == false)
	{
		test_sha_hash = true;
		test_pack_buffer = true;
		test_hash_server = true;
		test_worker_pool = true;
		test_api_redirection = true;
	}

	if (test_sha_hash)
		TestSHAHash();

	if (test_pack_buffer)
		TestPackBuffer();

	WSADATA wsa_data;
	EXPECT_EQ(NO_ERROR, WSAStartup(MAKEWORD(2,2), &wsa_data));
	AT_SCOPE_EXIT([](){ WSACleanup(); });

	if (test_hash_server)
		TestHashServer();

	if (test_worker_pool)
		TestWorkerPool();

	if (test_api_redirection)
		TestApiRedirection(debug_filename);

	DebugLog("All Tests Succeeded!\n");

	return 0;
}