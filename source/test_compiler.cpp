// test compiler
#include <cstdio>
#include <vector>
#include <cctype>

#define AT_SCOPE_EXIT(func) template<typename T> struct ScopeExiter { T exit_func = func;  ~ScopeExiter() { exit_func(); } } s_at_scope_exit<decltype(func)>;

std::vector<char> ReadBinaryFile(const char* filename)
{
	FILE *fp = std::fopen(filename, "rb");
	if (!fp)
	{
		std::fprintf(stderr, "Unable to open %s for reading\n", filename);
		exit(-1);
	}

	std::fseek(fp, 0, SEEK_END);
	std::vector<char> result(std::ftell(fp));
	std::fseek(fp, 0, SEEK_SET);

	std::fread(result.data(), 1, result.size(), fp);
	std::fclose(fp);

	return result;
}

void WriteBinaryFile(const std::vector<char>& data, const char* filename)
{
	FILE *fp = std::fopen(filename, "wb");
	if (!fp)
	{
		std::fprintf(stderr, "Unable to open %s for writing\n", filename);
		exit(-1);
	}

	std::fwrite(data.data(), 1, data.size(), fp);

	std::fclose(fp);
}

// arguments are input, output
int main(int argc, char** argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: tester.exe inputfile outputfile\n");
		return -1;
	}

	std::vector<char> data = ReadBinaryFile(argv[1]);

	for (char& c : data)
	{
		c = char(std::toupper(c));
	}

	WriteBinaryFile(data, argv[2]);

	fprintf(stdout, "OK!\n");

	return 0;
}