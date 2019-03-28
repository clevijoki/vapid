#pragma once

/*

Header only arg parser, collects all args, has required/optional args, and prints help if args didn't match. 

Usage: Insert code like this in the main function.

std::vector<const char*> source;
const char* output = nullptr;
bool strict_mode = false;
int32_t job_count = 8;
// also supports float and uint32_t values!

if (!ParseArgs(argc, argv, "MyExeName", {
	{&source, Arg::Required, 0, nullptr, "List of source files to compile."},
	{&output, Arg::Required, 'o', "output", "Output filename to compile to."},
	{&strict_mode, Arg::Optional, 0, "strict", "Use strict compile mode."},
	{&job_count, Arg::Optional, 'j', "jobs", "How many jobs to create."},
})) return 1;




*/


#include <initializer_list>
#include <vector>
#include <string>

struct Arg
{
	union
	{
		uint32_t* u32;
		int32_t* i32;
		float* f32;
		const char** string;
		bool* flag;
		std::vector<const char*>* list;
	};

	enum class Type
	{
		U32,
		I32,
		F32,
		String,
		Flag,
		List,
	};

	enum RequiredType
	{
		Optional,
		Required,
	};

	const char short_name;
	const char* name;
	const char* help;
	Type type;
	RequiredType required_type;
	mutable bool was_parsed = false;

	Arg(uint32_t *value, RequiredType required_type, char short_name, const char* name, const char* help)
	: u32(value)
	, type(Type::U32)
	, required_type(required_type)
	, short_name(short_name)
	, name(name)
	, help(help)
	{
	}

	Arg(int32_t *value, RequiredType required_type, char short_name, const char* name, const char* help)
	: i32(value)
	, type(Type::I32)
	, required_type(required_type)
	, short_name(short_name)
	, name(name)
	, help(help)
	{

	}

	Arg(float *value, RequiredType required_type, char short_name, const char* name, const char* help)
	: f32(value)
	, type(Type::F32)
	, required_type(required_type)
	, short_name(short_name)
	, name(name)
	, help(help)
	{

	}

	Arg(const char **value, RequiredType required_type, char short_name, const char* name, const char* help)
	: string(value)
	, type(Type::String)
	, required_type(required_type)
	, short_name(short_name)
	, name(name)
	, help(help)
	{
	}

	// this will just invert the flag if found, so pass what you don't want
	Arg(bool *value, RequiredType required_type, char short_name, const char* name, const char* help)
	: flag(value)
	, type(Type::Flag)
	, required_type(required_type)
	, short_name(short_name)
	, name(name)
	, help(help)
	{
	}

	Arg(std::vector<const char*> *value, RequiredType required_type, char short_name, const char* name, const char* help)
	: list(value)
	, type(Type::List)
	, required_type(required_type)
	, short_name(short_name)
	, name(name)
	, help(help)
	{
	}

	bool parse(const char* s) const
	{
		was_parsed = true;
		switch (type)
		{
		case Type::U32:
			*u32 = (uint32_t)atoi(s);
			break;

		case Type::I32:
			*i32 = (int32_t)atoi(s);
			break;

		case Type::F32:
			*f32 = (float)atof(s);
			break;

		case Type::String:
			*string = s;
			break;

		case Type::List:
			list->push_back(s);
			break;

		case Type::Flag:
			*flag = !*flag;
			break;
		}

		return true;
	}

	bool is_valid() const
	{
		if (required_type == Required && !was_parsed)
		{
			if (name)
			{
				printf("'--%s' is required\n", name);
			}
			else if (short_name)
			{
				printf("'-%c' is required\n", short_name);
			}
			else
			{
				printf("[source] list is required.");
			}
			return false;
		}
	}

	bool is_source() const
	{
		return name == nullptr && short_name == 0;
	}

	void print_default_value() const
	{
		switch (type)
		{
		case Type::U32:
			printf(" Default: %u", *u32);
			break;

		case Type::I32:
			printf(" Default: %d", *i32);
			break;

		case Type::F32:
			printf(" Default: %f", *f32);
			break;

		case Type::String:
			if (*string)
			{
				printf(" Default: %s", *string);
			}
			break;

		case Type::List:
			break;

		case Type::Flag:
			break;
		}
	}
};

// returns false if failed, and will print usage
inline bool ParseArgs(int argc, const char** argv, std::initializer_list<Arg> args)
{
	auto print_usage = [&](const char* help)
	{
		bool has_source = false;
		size_t max_long_name = 6; // help str

		const char *source_arg = nullptr;

		for (const Arg& a : args)
		{
			if (a.is_source())
			{
				has_source = true;

				source_arg = a.type == Arg::Type::List ? "[source1 source2]" : "[source]";

				max_long_name = std::max<size_t>(max_long_name, strlen(source_arg));
			}

			if (!a.name)
				continue;

			max_long_name = std::max<size_t>(max_long_name, strlen(a.name)+2);
		}

		if (help)
		{
			puts(help);
		}

		printf("Usage: %s [args]", argv[0]);
		if (has_source)
			printf(" %s...", source_arg);

		putchar('\n');

		max_long_name += 2; // add some spacing

		printf("  -?  --help");
		for (size_t n = 5; n < max_long_name; ++n)
			putchar(' ');

		puts("Print this message");

		for (const Arg& a : args)
		{
			if (a.short_name)
				printf("  -%c  ", a.short_name);
			else
				printf("       ");

			size_t remaining = max_long_name;
			if (a.name)
			{
				printf("--%s", a.name);

				remaining = max_long_name - (strlen(a.name)+2);
			}
			else
			{
				if (a.is_source())
				{
					printf(source_arg);

					remaining = max_long_name - strlen(source_arg);
				}
			}

			// space help to start on the same column
			for (size_t n = 0; n < remaining; ++n)
				putchar(' ');

			if (a.help)
			{
				printf(a.help);
			}

			a.print_default_value();

			putchar('\n');
		}

		return false;
	};

	for (int n = 1; n < argc;)
	{
		const char* current = argv[n];

		if (current[0] != '-')
		{
			bool valid = false;
			for (const Arg& a : args)
			{
				if (a.is_source())
				{
					a.parse(current);
					valid = true;
					break;
				}
			}

			if (!valid)
			{
				char tmp[512];
				snprintf(tmp, sizeof(tmp), "Invalid argument '%s',  ", current);
				return print_usage(tmp);				
			}
			else
			{
				++n;
				continue;
			}
		}

		const Arg* match = nullptr;
		if (current[1] == '-')
		{
			if (strcmp(current+2, "help")==0)
				return print_usage(nullptr);

			// parsing long name
			for (const Arg& a : args)
			{
				if (!a.name)
					continue;

				if (strcmp(current+2, a.name)==0)
				{
					match = &a;
					break;
				}
			}
		}
		else
		{
			if (!current[1] || current[2])
			{
				char tmp[512];
				snprintf(tmp, sizeof(tmp), "Invalid argument '%s', expected single letter after single dash", current);

				return print_usage(tmp);
			}

			if (current[1] == '?')
				return print_usage(nullptr);			

			// parsing short name
			for (const Arg& a : args)
			{
				if (a.short_name == current[1])
				{
					match = &a;
					break;
				}
			}
		}

		++n;

		if (!match)
		{
			char tmp[512];
			snprintf(tmp, sizeof(tmp), "Unknown argument %s", current);
			return print_usage(tmp);
		}

		if (match->type == Arg::Type::Flag)
		{
			*match->flag = !*match->flag; 
			continue;
		}

		// expected more elements
		if (n == argc)
		{
			char tmp[512];
			snprintf(tmp, sizeof(tmp), "Expected value after %s", argv[n-1]);
			return print_usage(tmp);
		}

		// now parse the value
		if (!match->parse(argv[n]))
		{
			char tmp[512];
			snprintf(tmp, sizeof(tmp), "Couldn't parse value for %s", argv[n-1]);
			return print_usage(tmp);
		}

		++n;
	}

	return true;
}