/*
    Copyright (c) 2011 Sencha, Inc.
    Copyright (c) 2010 Sencha, Inc.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <v8.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>

#include <JSGlobalData.h>
#include <SourceCode.h>
#include <UString.h>

using namespace v8;

static Handle<Value> fs_exists(const Arguments& args);
static Handle<Value> fs_isDirectory(const Arguments& args);
static Handle<Value> fs_isFile(const Arguments& args);
static Handle<Value> fs_makeDirectory(const Arguments& args);
static Handle<Value> fs_list(const Arguments& args);
static Handle<Value> fs_open(const Arguments& args);
static Handle<Value> fs_workingDirectory(const Arguments& args);

static Handle<Value> reflect_parse(const Arguments& args);

static Handle<Value> stream_constructor(const Arguments& args);
static Handle<Value> stream_close(const Arguments& args);
static Handle<Value> stream_flush(const Arguments& args);
static Handle<Value> stream_next(const Arguments& args);
static Handle<Value> stream_readLine(const Arguments& args);
static Handle<Value> stream_writeLine(const Arguments& args);

static Handle<Value> system_execute(const Arguments& args);
static Handle<Value> system_exit(const Arguments& args);
static Handle<Value> system_print(const Arguments& args);

static void CleanupStream(Persistent<Value>, void *data)
{
    delete reinterpret_cast<std::fstream*>(data);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: hammerjs inputfile.js" << std::endl;
        return 0;
    }

    FILE* f = fopen(argv[1], "r");
    if (!f) {
        std::cerr << "Error: unable to open file " << argv[1] << std::endl;
        return 0;
    }
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    rewind(f);
    char* buf = new char[len + 1];
    fread(buf, 1, len, f);
    buf[len - 1] = '\0';
    fclose(f);

    HandleScope handle_scope;
    Handle<ObjectTemplate> global = ObjectTemplate::New();
    Handle<Context> context = Context::New(NULL, global);

    Context::Scope context_scope(context);

    // 'fs' object
    Handle<FunctionTemplate> fsObject = FunctionTemplate::New();
    fsObject->Set(String::New("exists"), FunctionTemplate::New(fs_exists)->GetFunction());
    fsObject->Set(String::New("makeDirectory"), FunctionTemplate::New(fs_makeDirectory)->GetFunction());
    fsObject->Set(String::New("isDirectory"), FunctionTemplate::New(fs_isDirectory)->GetFunction());
    fsObject->Set(String::New("isFile"), FunctionTemplate::New(fs_isFile)->GetFunction());
    fsObject->Set(String::New("list"), FunctionTemplate::New(fs_list)->GetFunction());
    fsObject->Set(String::New("open"), FunctionTemplate::New(fs_open)->GetFunction());
    fsObject->Set(String::New("workingDirectory"), FunctionTemplate::New(fs_workingDirectory)->GetFunction());
    context->Global()->Set(String::New("fs"), fsObject->GetFunction());

    // 'Reflect' object
    Handle<FunctionTemplate> reflectObject = FunctionTemplate::New();
    reflectObject->Set(String::New("parse"), FunctionTemplate::New(reflect_parse)->GetFunction());
    reflectObject->Set(String::New("exit"), FunctionTemplate::New(system_exit)->GetFunction());
    context->Global()->Set(String::New("Reflect"), reflectObject->GetFunction());

    // 'system' object
    Handle<FunctionTemplate> systemObject = FunctionTemplate::New();
    Handle<Array> systemArgs = Array::New(argc);
    for (int i = 1; i < argc; ++i)
        systemArgs->Set(i - 1, String::New(argv[i]));
    systemObject->Set(String::New("args"), systemArgs);
    systemObject->Set(String::New("execute"), FunctionTemplate::New(system_execute)->GetFunction());
    systemObject->Set(String::New("exit"), FunctionTemplate::New(system_exit)->GetFunction());
    systemObject->Set(String::New("print"), FunctionTemplate::New(system_print)->GetFunction());
    context->Global()->Set(String::New("system"), systemObject->GetFunction());

    // 'Stream' class
    Handle<FunctionTemplate> streamClass = FunctionTemplate::New(stream_constructor);
    streamClass->SetClassName(String::New("Stream"));
    streamClass->InstanceTemplate()->SetInternalFieldCount(1);
    streamClass->InstanceTemplate()->Set(String::New("close"), FunctionTemplate::New(stream_close)->GetFunction());
    streamClass->InstanceTemplate()->Set(String::New("flush"), FunctionTemplate::New(stream_flush)->GetFunction());
    streamClass->InstanceTemplate()->Set(String::New("next"), FunctionTemplate::New(stream_next)->GetFunction());
    streamClass->InstanceTemplate()->Set(String::New("readLine"), FunctionTemplate::New(stream_readLine)->GetFunction());
    streamClass->InstanceTemplate()->Set(String::New("writeLine"), FunctionTemplate::New(stream_writeLine)->GetFunction());
    context->Global()->Set(String::New("Stream"), streamClass->GetFunction(), PropertyAttribute(ReadOnly | DontDelete));

    Handle<Script> script = Script::Compile(String::New(buf));
    script->Run();

    delete [] buf;
    return 0;
}

static Handle<Value> fs_exists(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: function fs.exists() accepts 1 argument"));

    String::Utf8Value fileName(args[0]);

    struct stat statbuf;
    bool canStat = ::stat(*fileName, &statbuf) == 0;
    return Boolean::New(canStat);
}

static Handle<Value> fs_isDirectory(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: fs.isDirectory() accepts 1 argument"));

    String::Utf8Value name(args[0]);

    struct stat statbuf;
    if (::stat(*name, &statbuf))
        return ThrowException(String::New("Exception: fs.isDirectory() can't access the directory"));

    return Boolean::New(S_ISDIR(statbuf.st_mode));
}

static Handle<Value> fs_isFile(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: fs.isFile() accepts 1 argument"));

    String::Utf8Value name(args[0]);

    struct stat statbuf;
    if (::stat(*name, &statbuf))
        return ThrowException(String::New("Exception: fs.isFile() can't access the file"));

    return Boolean::New(S_ISREG(statbuf.st_mode));
}

static Handle<Value> fs_makeDirectory(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: function fs.makeDirectory() accepts 1 argument"));

    String::Utf8Value directoryName(args[0]);

    if (::mkdir(*directoryName, 0777) == 0)
        return Undefined();

    return ThrowException(String::New("Exception: fs.makeDirectory() can't create the directory"));
}

static Handle<Value> fs_list(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: fs.list() accepts 1 argument"));

    String::Utf8Value dirname(args[0]);

    DIR *dir = opendir(*dirname);
    if (!dir)
        return ThrowException(String::New("Exception: fs.list() can't access the directory"));

    Handle<Array> entries = Array::New();
    int count = 0;
    struct dirent entry;
    struct dirent *ptr = NULL;
    ::readdir_r(dir, &entry, &ptr);
    while (ptr) {
        if (strcmp(entry.d_name, ".") && strcmp(entry.d_name, ".."))
            entries->Set(count++, String::New(entry.d_name));
        ::readdir_r(dir, &entry, &ptr);
    }
    ::closedir(dir);

    return entries;
}

static Handle<Value> fs_open(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1 && args.Length() != 2)
        return ThrowException(String::New("Exception: function fs.open() accepts 1 or 2 arguments"));

    Handle<Context> context = Context::GetCurrent();
    Handle<Value> streamClass = context->Global()->Get(String::New("Stream"));
    Function *streamFunction = Function::Cast(*streamClass);

    Handle<Value> argv[2];
    argv[0] = args[0];
    if (args.Length() == 2)
        argv[1] = args[1];

    Handle<Object> result = streamFunction->NewInstance(args.Length(), argv);
    return result;
}

static Handle<Value> reflect_parse(const Arguments& args)
{
    if (args.Length() != 1)
        return ThrowException(String::New("Exception: Reflect.parse() accepts 1 argument"));

    String::Utf8Value code(args[0]);
    UChar *content = new UChar[code.length()];
    for (int i = 0; i < code.length(); ++i)
        content[i] = (*code)[i];
    JSC::UString scriptCode = JSC::UString(content, code.length());
    delete [] content;

    JSC::JSGlobalData* globalData = new JSC::JSGlobalData;
    JSC::UString tree = globalData->parser->createSyntaxTree(globalData, JSC::makeSource(scriptCode));
    delete globalData;

    if (tree.length() == 0)
        return Undefined();

    char *buffer = new char[tree.length() + 1];
    buffer[tree.length()] = '\0';
    const UChar *uchars = tree.characters();
    for (size_t i = 0; i < tree.length(); ++i)
        buffer[i] = uchars[i];

    Handle<ObjectTemplate> global = ObjectTemplate::New();
    global->Set("tree", String::New(buffer));
    Handle<Context> context = Context::New(NULL, global);
    Context::Scope context_scope(context);

    Handle<Script> script = Script::Compile(String::New("JSON.parse(tree)"));
    delete [] buffer;
    return script->Run();
}

static Handle<Value> fs_workingDirectory(const Arguments& args)
{
    if (args.Length() != 0)
        return ThrowException(String::New("Exception: function fs.workingDirectory() accepts no argument"));

    char currentName[PATH_MAX + 1];
    if (::getcwd(currentName, PATH_MAX))
        return String::New(currentName);

    return ThrowException(String::New("Exception: fs.workingDirectory() can't get current working directory"));
}

static Handle<Value> stream_constructor(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1 && args.Length() != 2)
        return ThrowException(String::New("Exception: Stream constructor accepts 1 or 2 arguments"));

    String::Utf8Value name(args[0]);
    String::Utf8Value modes(args[1]);

    std::fstream::openmode mode = std::fstream::in;
    if (args.Length() == 2) {
        String::Utf8Value m(args[0]);
        const char* options = *modes;
        bool read = strchr(options, 'r');
        bool write = strchr(options, 'w');
        if (!read && !write)
            return ThrowException(String::New("Exception: Invalid open mode for Stream"));
        if (!read)
            mode = std::fstream::out;
        if (write)
            mode |= std::fstream::out;
    }

    std::fstream *data = new std::fstream;
    data->open(*name, mode);
    if (data->bad()) {
        delete data;
        return ThrowException(String::New("Exception: Can't open the file"));
    }

    args.This()->SetPointerInInternalField(0, data);

    Persistent<Object> persistent = Persistent<Object>::New(args.Holder());
    persistent.MakeWeak(data, CleanupStream);

    persistent->Set(String::New("name"), args[0]);

    return handle_scope.Close(persistent);
}

static Handle<Value> stream_close(const Arguments& args)
{
    if (args.Length() != 0)
        return ThrowException(String::New("Exception: Stream.close() accepts no argument"));

    void *data = args.This()->GetPointerFromInternalField(0);
    std::fstream *fs = reinterpret_cast<std::fstream*>(data);
    fs->close();

    return Undefined();
}

static Handle<Value> stream_flush(const Arguments& args)
{
    if (args.Length() != 0)
        return ThrowException(String::New("Exception: Stream.flush() accepts no argument"));

    void *data = args.This()->GetPointerFromInternalField(0);
    std::fstream *fs = reinterpret_cast<std::fstream*>(data);
    fs->flush();

    return args.This();
}

static Handle<Value> stream_next(const Arguments& args)
{
    if (args.Length() != 0)
        return ThrowException(String::New("Exception: Stream.next() accepts no argument"));

    void *data = args.This()->GetPointerFromInternalField(0);
    std::fstream *fs = reinterpret_cast<std::fstream*>(data);

    std::string buffer;
    std::getline(*fs, buffer);
    if (fs->eof())
        return ThrowException(String::New("Exception: Stream.readLine() reaches end of file"));

    return String::New(buffer.c_str());
}

static Handle<Value> stream_readLine(const Arguments& args)
{
    if (args.Length() != 0)
        return ThrowException(String::New("Exception: Stream.readLine() accepts no argument"));

    void *data = args.This()->GetPointerFromInternalField(0);
    std::fstream *fs = reinterpret_cast<std::fstream*>(data);

    if (fs->eof())
        return String::NewSymbol("");

    std::string buffer;
    std::getline(*fs, buffer);
    buffer.append("\n");

    return String::New(buffer.c_str());
}

static Handle<Value> stream_writeLine(const Arguments& args)
{
    if (args.Length() != 1)
        return ThrowException(String::New("Exception: Stream.writeLine() accepts 1 argument"));

    void *data = args.This()->GetPointerFromInternalField(0);
    std::fstream *fs = reinterpret_cast<std::fstream*>(data);

    String::Utf8Value line(args[0]);
    fs->write(*line, line.length());
    fs->put('\n');

    return args.This();
}

static Handle<Value> system_execute(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: function system.execute() accepts 1 argument"));

    String::Utf8Value cmd(args[0]);
    ::system(*cmd);


    return Undefined();
}

static Handle<Value> system_exit(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 0 && args.Length() != 1)
        return ThrowException(String::New("Exception: function system.exit() accepts 1 argument"));

    int status = (args.Length() == 1) ? args[0]->Int32Value() : 0;
    exit(status);

    return Undefined();
}

static Handle<Value> system_print(const Arguments& args)
{
    HandleScope handle_scope;

    for (int i = 0; i < args.Length(); i++) {
        String::Utf8Value value(args[i]);
        std::cout << *value;
        if (i < args.Length() - 1)
            std::cout << ' ';
    }
    std::cout << std::endl;

    return Undefined();
}

