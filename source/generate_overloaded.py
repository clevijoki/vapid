import tokenize
import token
import io


class FunctionDefinition(object):
    def __init__(self):
        self.return_type = None
        self.name = None
        self.args = {}
        self.generate_impl = True
        self.fail_value = None

defs = []

FAIL_VALUES = {
    'HANDLE':'INVALID_HANDLE_VALUE',
    'BOOL':'FALSE',
    'DWORD':'-1',
}

POINTER_DATA_TYPES = {
    'PLONG':'LONG',
    'LPDWORD':'DWORD',
    'PLARGE_INTEGER':'LARGE_INTEGER'
}

EXPECT_STATEMENT_START = 0
EXPECT_FUNCTION_NAME = 1
EXPECT_ARG_LIST_START = 2
EXPECT_ARG_START = 3
EXPECT_ARG_NAME = 4
EXPECT_ARG_LIST_END = 5
EXPECT_STATEMENT_END = 6
EXPECT_BUFFER_SIZE_START = 7
EXPECT_BUFFER_SIZE_NAME = 8
EXPECT_BUFFER_SIZE_END = 9
EXPECT_IGNORE_VALUE_START = 10
EXPECT_IGNORE_VALUE_NAME = 11
EXPECT_IGNORE_VALUE_END = 12
EXPECT_FAIL_VALUE_START = 13
EXPECT_FAIL_VALUE_NAME = 14
EXPECT_FAIL_VALUE_END = 15

state = EXPECT_STATEMENT_START

current_def = FunctionDefinition()
current_arg = {}

infilebytes = open('GenerationInput.h', 'rt').read().encode()

op_name = {
    token.OP:"OP",
    token.STRING:"STRING",
    token.NAME:"NAME",
    token.NUMBER:"NUMBER"
}

for token_type, name, _, _, _ in tokenize.tokenize(io.BytesIO(infilebytes).readline):

    def match(match_type, val, next_state):
        match_types = match_type if isinstance(match_type, list) else [match_type]

        if name != val or token_type not in match_types:
            raise Exception("Syntax error, expected '%s %s', found '%s %s'" % (' or '.join([op_name[x] for x in match_types]), val, op_name[token_type], name))
        global state
        state = next_state
        return name

    def match_test(match_type, val, next_state):
        if name != val or match_type != token_type:
            return False
        global state
        state = next_state
        return True

    # print(token_type, name, state)

    if token_type not in op_name:
        continue

    if name in ['', 'ENCODING', 'NEWLINE', 'ENDMARKER']:
        continue

    if state == EXPECT_STATEMENT_START:
        if match_test(token.NAME, '__NO_IMPL', EXPECT_STATEMENT_START):
            current_def.generate_impl = False
            continue

        if match_test(token.NAME, '__FAIL_VALUE', EXPECT_FAIL_VALUE_START):
            continue

        current_def.return_type = match(token.NAME, name, EXPECT_FUNCTION_NAME)

        if current_def.fail_value is None:

            if not current_def.return_type in FAIL_VALUES:
                raise Exception("didn't have a default fail value for type %s" % (current_def.return_type))
            current_def.fail_value = FAIL_VALUES[current_def.return_type]

    elif state == EXPECT_FAIL_VALUE_START:
        match(token.OP, '(', EXPECT_FAIL_VALUE_NAME)

    elif state == EXPECT_FAIL_VALUE_NAME:
        current_def.fail_value = match([token.NAME, token.NUMBER], name, EXPECT_FAIL_VALUE_END)

    elif state == EXPECT_FAIL_VALUE_END:
        match(token.OP, ')', EXPECT_STATEMENT_START)

    elif state == EXPECT_FUNCTION_NAME:
        current_def.name = match(token.NAME, name, EXPECT_ARG_LIST_START)

    elif state == EXPECT_ARG_LIST_START:
        match(token.OP, '(', EXPECT_ARG_START)

    elif state == EXPECT_ARG_START:
        if name == ')':
            match(token.OP, ')', EXPECT_STATEMENT_END)

        else:
            current_arg['type'] = match(token.NAME, name, EXPECT_ARG_NAME)

    elif state == EXPECT_ARG_NAME:
        current_arg['name'] = match(token.NAME, name, EXPECT_ARG_LIST_END)

    elif state == EXPECT_ARG_LIST_END:

        if match_test(token.NAME, '__BUFFER_SIZE', EXPECT_BUFFER_SIZE_START):
            continue

        if match_test(token.NAME, '__INOUT', EXPECT_ARG_LIST_END):
            current_arg['inout'] = True
            continue

        if match_test(token.NAME, '__IGNORE', EXPECT_IGNORE_VALUE_START):
            continue

        # done our argument add us 
        current_def.args[current_arg['name']] = current_arg
        current_arg = {}

        if name == ',':
            match(token.OP, ',', EXPECT_ARG_START)

        elif name == ')':
            match(token.OP, ')', EXPECT_STATEMENT_END)

    elif state == EXPECT_BUFFER_SIZE_START:
        match(token.OP, '(', EXPECT_BUFFER_SIZE_NAME)

    elif state == EXPECT_BUFFER_SIZE_NAME:
        current_arg['size_arg'] = match(token.NAME, name, EXPECT_BUFFER_SIZE_END)

    elif state == EXPECT_BUFFER_SIZE_END:
        match(token.OP, ')', EXPECT_ARG_LIST_END)

    elif state == EXPECT_IGNORE_VALUE_START:
        match(token.OP, '(', EXPECT_IGNORE_VALUE_NAME)

    elif state == EXPECT_IGNORE_VALUE_NAME:

        current_arg['ignore_value'] = match([token.NAME, token.NUMBER], name, EXPECT_IGNORE_VALUE_END)

    elif state == EXPECT_IGNORE_VALUE_END:
        match(token.OP, ')', EXPECT_ARG_LIST_END)

    elif state == EXPECT_STATEMENT_END:
        match(token.OP, ';', EXPECT_STATEMENT_START)
        defs.append(current_def)

        current_def = FunctionDefinition()

# now that we've parsed all of our files, generate the parsing code

STATIC_TYPES = ['DWORD', 'LARGE_INTEGER', 'LONG']
STRING_TYPES = ['LPCSTR', 'LPCWSTR']

shared_enum_str = ""
worker_bind_str = ""
worker_os_ptrs = ""
detours_attach_str = ""
detours_detach_str = ""
worker_func_decls = ""

def get_worker_arg_str(arg):

    return "%s %s" % (arg['type'], arg['name'])

for func in defs:
    shared_enum_str += '\t\t%s,\n' % (func.name)

    arg_type_str = ', '.join([arg['type'] for arg in func.args.values()])
    worker_os_ptrs += '\t\t%s (*%s)(%s);\n' % (func.return_type, func.name, arg_type_str)

    worker_bind_str += '\t\tworker::os::%s = &::%s;\n' % (func.name, func.name)

    detours_attach_str += '\t\tDetourAttach(&(PVOID&)worker::os::%s, worker::%s);\n' % (func.name, func.name)
    detours_detach_str += '\t\tDetourDetach(&(PVOID&)worker::os::%s, worker::%s);\n' % (func.name, func.name)

    named_arg_type_str = ', '.join([get_worker_arg_str(x) for x in func.args.values()])
    worker_func_decls += '\t%s %s(%s);\n' % (func.return_type, func.name, named_arg_type_str)


master_switch = ""
worker_func_defs = ""

for func in defs:

    if not func.generate_impl:
        continue

    master_switch += "\t\tcase common::ApiTag::%s:\n" % (func.name)
    master_switch += "\t\t{\n"

    named_arg_type_str = ', '.join([get_worker_arg_str(arg) for arg in func.args.values()])
    worker_func_defs += '\t%s %s(%s)\n\t{\n' % (func.return_type, func.name, named_arg_type_str)

    fail_value = func.fail_value

    # look for a handle to translate
    needs_sep = False
    for name, arg in func.args.items():
        if 'ignore_value' in arg:
            continue

        if arg['type'] == 'HANDLE':
            worker_func_defs += '\t\tHANDLE_DATA_TYPE %s_data = LookupRealHandle(%s);\n' % (name, name)

            worker_func_defs += '\t\tif (%s_data == INVALID_HANDLE_DATA_TYPE)\n\t\t\treturn os::%s(%s);\n' % (name, func.name, ', '.join(func.args.keys()))

            needs_sep = True

    if needs_sep:
        worker_func_defs += '\n'


    # generate the recieve buf size

    param_sizes = []

    for _, arg in func.args.items():

        arg_type = arg['type']

        if 'ignore_value' in arg:
            continue     

        elif arg_type in STATIC_TYPES:
            param_sizes.append('sizeof(%s)' % arg_type)
        elif arg_type in STRING_TYPES:
            param_sizes.append('sizeof(uint32_t)')
        elif arg_type == 'HANDLE':
            param_sizes.append('sizeof(HANDLE_DATA_TYPE)')

        elif arg_type in POINTER_DATA_TYPES:
            param_sizes.append('sizeof(uint8_t)')

            if 'inout' in arg:
                param_sizes.append('sizeof(%s)' % (POINTER_DATA_TYPES[arg_type]))


    # figure out response size so we can generate an accurate data buf size
    return_sizes = []
    fixed_optional_sizes = []
    dynamic_optional_sizes = []

    if func.return_type == 'HANDLE':
        return_sizes.append('sizeof(HANDLE_DATA_TYPE)')
    elif func.return_type != 'VOID':
        return_sizes.append('sizeof(%s)' % func.return_type)

    for arg_name, arg in func.args.items():
        arg_type = arg['type']

        if 'ignore_value' in arg:
            continue     

        if arg_type in POINTER_DATA_TYPES:
            use_arg_type = POINTER_DATA_TYPES[arg_type]
            fixed_optional_sizes.append('sizeof(%s)' % (use_arg_type))
            dynamic_optional_sizes.append('(%s ? sizeof(%s) : 0)' % (arg_name, use_arg_type))

    return_sizes.append('sizeof(DWORD)')
    
    worker_func_defs += '\t\tconst size_t param_size = sizeof(uint8_t) + %s;\n' % (' + '.join(param_sizes))
    master_switch += "\t\t\tconst size_t param_size = %s;\n" % (" + ".join(param_sizes) if param_sizes else '0')

    master_switch += '\t\t\tconst size_t fixed_response_size = %s;\n' % (' + '.join(return_sizes + fixed_optional_sizes))
    worker_func_defs += '\t\tconst size_t fixed_response_size = %s;\n' % (' + '.join(return_sizes + fixed_optional_sizes))

    if param_sizes:

        master_switch += "\t\t\tchar data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];\n"
        master_switch += "\t\t\tif (!ReceiveData(socket, data_buf, param_size))\n\t\t\t\treturn false;\n\n"
        master_switch += "\t\t\tchar *buf = data_buf;\n"

        worker_func_defs += '\t\tchar data_buf[param_size > fixed_response_size ? param_size : fixed_response_size];\n\n'
        worker_func_defs += "\t\tchar *buf = data_buf;\n"
        worker_func_defs += '\t\tbufops::Write<uint8_t>(buf, static_cast<uint8_t>(common::ApiTag::%s));\n' % (func.name)

        # now serialize them the same way

        for arg_name, arg in func.args.items():

            if 'ignore_value' in arg:
                continue

            arg_type = arg['type']
            
            if arg_type in STATIC_TYPES:
                worker_func_defs += '\t\tbufops::Write<%s>(buf, %s);\n' % (arg_type, arg_name)
                master_switch += '\t\t\tconst %s %s = bufops::Read<%s>(buf);\n' % (arg_type, arg_name, arg_type)

            elif arg_type == 'LPCSTR':
                worker_func_defs += '\t\tconst uint32_t %s_len = static_cast<uint32_t>(strlen(%s));\n' % (arg_name, arg_name)
                worker_func_defs += '\t\tbufops::Write<uint32_t>(buf, %s_len);\n' % (arg_name)
                master_switch += '\t\t\tconst uint32_t %s_len = bufops::Read<uint32_t>(buf);\n' % (arg_name)

            elif arg_type == 'LPCWSTR':
                worker_func_defs += '\t\tconst uint32_t %s_len = static_cast<uint32_t>(wcslen(%s));\n' % (arg_name, arg_name)
                worker_func_defs += '\t\tbufops::Write<uint32_t>(buf, %s_len);\n' % (arg_name)
                master_switch += '\t\t\tconst uint32_t %s_len = bufops::Read<uint32_t>(buf);\n' % (arg_name)

            elif arg_type == 'HANDLE':
                worker_func_defs += '\t\tbufops::Write<HANDLE_DATA_TYPE>(buf, %s_data);\n' % (arg_name)
                master_switch += '\t\t\tconst HANDLE %s = ToHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));\n' % (arg_name)

            elif arg_type in POINTER_DATA_TYPES:
                worker_func_defs += '\t\tbufops::Write<uint8_t>(buf, %s ? 1 : 0);\n' % (arg_name)
                master_switch += '\t\t\tconst bool %s_is_null = bufops::Read<uint8_t>(buf) == 0;\n' % (arg_name)

        master_switch += "\t\t\tEXPECT_EQ(buf, data_buf + param_size);\n\n"

        worker_func_defs += "\t\tEXPECT_EQ(buf, data_buf + param_size);\n\n"
        worker_func_defs += "\t\tauto socket_ptr = worker::g_socket.lock();\n"
        worker_func_defs += "\t\tEnsureNetworkIsSetup(*socket_ptr);\n\n"
        worker_func_defs += "\t\tif (!SendData(*socket_ptr, data_buf, param_size))\n\t\t\treturn %s;\n" % fail_value

        # now serialize our strings
        for arg_name, arg in func.args.items():
            arg_type = arg['type']

            if arg_type == 'LPCSTR':
                worker_func_defs += "\t\tif (!SendData(*socket_ptr, %s, %s_len))\n\t\t\treturn %s;\n" % (arg_name, arg_name, fail_value)

                master_switch += '\t\t\tstd::unique_ptr<char[]> %s(new char[%s_len+1]);\n' % (arg_name, arg_name)
                master_switch += '\t\t\tif (!ReceiveData(socket, %s.get(), %s_len))\n\t\t\t\treturn false;\n' % (arg_name, arg_name)
                master_switch += '\t\t\t%s[%s_len] = 0;\n\n' % (arg_name, arg_name)


            elif arg_type == 'LPCWSTR':
                worker_func_defs += "\t\tif (!SendData(*socket_ptr, %s, %s_len * sizeof(wchar_t)))\n\t\t\treturn %s;\n" % (arg_name, arg_name, fail_value)

                master_switch += '\t\t\tstd::unique_ptr<wchar_t[]> %s(new wchar_t[%s_len+1]);\n' % (arg_name, arg_name)
                master_switch += '\t\t\tif (!ReceiveData(socket, %s.get(), %s_len * sizeof(wchar_t)))\n\t\t\t\treturn false;\n' % (arg_name, arg_name)
                master_switch += '\t\t\t%s[%s_len] = 0;\n\n' % (arg_name, arg_name)

                
            elif arg_type == "LPCVOID":
                size_arg = arg['size_arg']
                master_switch += '\t\t\tstd::unique_ptr<uint8_t[]> %s(new uint8_t[%s]);\n' % (arg_name, size_arg)
                master_switch += '\t\t\tif (!ReceiveData(socket, %s.get(), %s))\n\t\t\t\treturn false;\n' % (arg_name, size_arg)

                worker_func_defs += "\t\tif (!SendData(*socket_ptr, %s, %s))\n\t\t\treturn %s;\n" % (arg_name, size_arg, fail_value)

        worker_func_defs += '\n'
        worker_func_defs += '\t\tconst size_t dynamic_response_size = %s;\n' % (' + '.join(return_sizes + dynamic_optional_sizes))
        worker_func_defs += "\t\tif (!ReceiveData(*socket_ptr, data_buf, dynamic_response_size))\n\t\t\treturn %s;\n" % (fail_value)

    worker_func_defs += '\t\tbuf = data_buf;\n'

    if func.return_type == 'HANDLE':
        worker_func_defs += '\t\tconst HANDLE result = CreateFakeHandle(bufops::Read<HANDLE_DATA_TYPE>(buf));\n'

    elif func.return_type != 'VOID':
        worker_func_defs += '\t\tconst %s result = bufops::Read<%s>(buf);\n' % (func.return_type, func.return_type)


    if func.return_type != 'VOID':
        worker_func_defs += '\t\tif (result == %s)\n\t\t{\n' % (fail_value)
        worker_func_defs += '\t\t\tSetLastError(bufops::Read<DWORD>(buf));\n'
        worker_func_defs += '\t\t\treturn result;\n'
        worker_func_defs += '\t\t}\n'

    # now setup result types from the API func
    for arg_name, arg in func.args.items():
        arg_type = arg['type']

        if 'ignore_value' in arg:
            continue

        elif arg_type in POINTER_DATA_TYPES:
            use_data_type = POINTER_DATA_TYPES[arg_type]
            master_switch += '\t\t\t%s %s{};\n' % (use_data_type, arg_name)
            worker_func_defs += '\t\tif (%s) *%s = bufops::Read<%s>(buf);\n' % (arg_name, arg_name, use_data_type)

        elif arg_type == 'LPVOID':
            master_switch += '\t\t\tstd::unique_ptr<uint8_t[]> %s(new uint8_t[%s]);\n' % (arg_name, arg['size_arg'])

            worker_func_defs += '\t\tif (%s) ReceiveData(*socket_ptr, %s, %s);\n' % (arg_name, arg_name, arg['size_arg'])

    worker_func_defs += '\t\tEXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));\n\n'

    if func.name == 'CloseHandle':
        worker_func_defs += '\t\tReleaseFakeHandle(hObject);\n'
    
    worker_func_defs += '\t\treturn result;\n'

    # now we have to do the api call
    first_arg = True
    master_switch += '\t\t\t'

    if func.return_type != 'VOID':
        master_switch += 'const %s result = ' % (func.return_type)

    master_switch += '%s(' % (func.name)

    master_dynamic_response_sizes = []
    # setup arguments
    for arg_name, arg in func.args.items():
        arg_type = arg['type']

        if not first_arg:
            master_switch += ','
        first_arg = False

        ignore_value = arg.get('ignore_value', None)

        if ignore_value is not None:
            master_switch += '\n\t\t\t\t%s' % (ignore_value)

        elif arg_type in STATIC_TYPES + ['HANDLE']:
            master_switch += '\n\t\t\t\t%s' % (arg_name)
        
        elif arg_type in STRING_TYPES:
            master_switch += '\n\t\t\t\t%s.get()' % (arg_name)
            
        elif arg_type in POINTER_DATA_TYPES:
            master_switch += '\n\t\t\t\t%s_is_null ? nullptr : &%s' % (arg_name, arg_name)
            master_dynamic_response_sizes.append('(%s_is_null ? 0 : sizeof(%s))' % (arg_name, POINTER_DATA_TYPES[arg_type]))

        elif arg_type == 'LPVOID':
            master_switch += '\n\t\t\t\t%s.get()' % (arg_name)

        elif arg_type == 'LPCVOID':
            master_switch += '\n\t\t\t\t%s.get()' % (arg_name)

        else:
            raise Exception("Unhandled param '%s %s' in function '%s'" % (arg_type, arg_name, func.name))

            
    master_switch += '\n\t\t\t);\n\n'

    # write response
    master_switch += '\t\t\tbuf = data_buf;\n'

    master_switch += '\t\t\tconst size_t dynamic_response_size = %s;\n' % (' + '.join(return_sizes + master_dynamic_response_sizes))

    if func.return_type == 'HANDLE':
        master_switch += '\t\t\tbufops::Write<HANDLE_DATA_TYPE>(buf, ToHandleDataType(result));\n';

    elif func.return_type != 'VOID':
        master_switch += '\t\t\tbufops::Write<%s>(buf, result);\n' % (func.return_type);

    master_switch += '\t\t\tif (result == %s)\n\t\t\t{\n' % (fail_value)
    master_switch += '\t\t\t\tbufops::Write<DWORD>(buf, GetLastError());\n'
    master_switch += '\t\t\t}\n'
    master_switch += '\t\t\telse\n'
    master_switch += '\t\t\t{\n'

    for arg_name, arg in func.args.items():
        arg_type = arg['type']

        if arg_type in POINTER_DATA_TYPES:
            master_switch += '\t\t\t\tif (!%s_is_null) bufops::Write<%s>(buf, %s);\n' % (arg_name, POINTER_DATA_TYPES[arg_type], arg_name)

    master_switch += "\t\t\t\tEXPECT_EQ(buf, data_buf + dynamic_response_size - sizeof(DWORD));\n"
    # write last error
    master_switch += '\t\t\t}\n\n'

    master_switch += "\t\t\tif (!SendData(socket, data_buf, dynamic_response_size))\n\t\t\t\treturn false;\n"

    for arg_name, arg in func.args.items():
        arg_type = arg['type']

        if arg_type == 'LPVOID':
            master_switch += "\t\t\tif (!SendData(socket, %s.get(), %s))\n\t\t\t\treturn false;\n" % (arg_name, arg['size_arg'])

    master_switch += "\t\t}\n"
    master_switch += "\t\treturn true;\n\n"

    worker_func_defs += "\t}\n\n"


master_result = """
/* This function is generated from geneated_overloaded.py, do not edit directly */

namespace master {

bool Update(SOCKET socket)
{
    using namespace bufops;

    common::ApiTag tag;
    if (!ReceiveData(socket, &tag, sizeof(uint8_t)))
        return false;

    switch (tag)
    {
%s
        default:
            ASSERT(false, "Unknown tag code %%d", tag);
    }

    return true;
}

} // namespace master
""" % master_switch

with open('GeneratedMaster.inl', 'wt') as f:
    f.write(master_result)

worker_result = """
/* This file is generated from generated_overloaded.py, do not edit directly */

namespace worker
{
    void EnsureNetworkIsSetup(SOCKET& s);
    HANDLE_DATA_TYPE LookupRealHandle(HANDLE fake_handle);

    void ReleaseFakeHandle(HANDLE);
    HANDLE CreateFakeHandle(HANDLE_DATA_TYPE data_type);

    namespace os
    {
%s
    }

%s

%s
}

""" % (worker_os_ptrs, worker_func_decls, worker_func_defs)

with open('GeneratedWorker.inl', 'wt') as f:
    f.write(worker_result)

shared_result = """#pragma once
/* This file is generated from generated_overloaded.py, do not edit directly */

namespace common
{
    enum class ApiTag : uint8_t
    {
%s
    };
}

typedef int64_t HANDLE_DATA_TYPE;

inline HANDLE_DATA_TYPE ToHandleDataType(HANDLE handle)
{
    return (HANDLE_DATA_TYPE)handle;
}

inline HANDLE ToHandle(HANDLE_DATA_TYPE handle)
{
    return (HANDLE)handle;
}

const HANDLE_DATA_TYPE INVALID_HANDLE_DATA_TYPE = ToHandleDataType(INVALID_HANDLE_VALUE);


""" % (shared_enum_str)

with open('GeneratedCommon.h', 'wt') as f:
    f.write(shared_result)


detours_result = """#pragma once
/* This file is generated from generated_overloaded.py, do not edit directly */
#include "detours/detours.h"

namespace detours
{
    // save all original pointers
    void Inject()
    {
%s
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
%s
        DetourTransactionCommit();
    }

    // undo detours
    void UndoInject()
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
%s
        DetourTransactionCommit();

    }
}

""" % (worker_bind_str, detours_attach_str, detours_detach_str)

with open('GeneratedDetours.inl', 'wt') as f:
    f.write(detours_result)