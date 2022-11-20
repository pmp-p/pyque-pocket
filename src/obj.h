#pragma once

#include "safestl.h"

typedef int64_t _Int;
typedef double _Float;

const _Int _Int_MAX_POS = 9223372036854775807LL;
const _Int _Int_MAX_NEG = -9223372036854775807LL;
const _Float _FLOAT_INF_POS = INFINITY;
const _Float _FLOAT_INF_NEG = -INFINITY;

#define PK_VERSION "0.2.9"

class CodeObject;
class BasePointer;
class Pointer;
class VM;
class Frame;
class PkExportedResource {};

typedef std::shared_ptr<const BasePointer> _Pointer;
typedef PyVar (*_CppFunc)(VM*, const pkpy::ArgList&);
typedef std::shared_ptr<CodeObject> _Code;

struct Pointer {
    Frame* frame;               // the frame that created this pointer
    _Pointer ptr;               // the internal pointer
    Pointer(Frame* frame, _Pointer ptr) : frame(frame), ptr(ptr) {}
};

struct Function {
    _Str name;
    _Code code;
    std::vector<_Str> args;
    _Str starredArg;        // empty if no *arg
    PyVarDict kwArgs;       // empty if no k=v
    std::vector<_Str> kwArgsOrder;

    bool hasName(const _Str& val) const {
        bool _0 = std::find(args.begin(), args.end(), val) != args.end();
        bool _1 = starredArg == val;
        bool _2 = kwArgs.find(val) != kwArgs.end();
        return _0 || _1 || _2;
    }
};

struct _BoundedMethod {
    PyVar obj;
    PyVar method;
};

struct _Range {
    _Int start = 0;
    _Int stop = -1;
    _Int step = 1;
};

struct _Slice {
    int start = 0;
    int stop = 2147483647;  // container types always use int32 as index, no support for int64

    void normalize(int len){
        if(start < 0) start += len;
        if(stop < 0) stop += len;
        if(start < 0) start = 0;
        if(stop > len) stop = len;
    }
};

class _Iterator {
protected:
    PyVar _ref;     // keep a reference to the object so it will not be deleted while iterating
    VM* vm;
public:
    virtual PyVar next() = 0;
    virtual bool hasNext() = 0;
    _Pointer var;
    _Iterator(VM* vm, PyVar _ref) : vm(vm), _ref(_ref) {}
};

typedef std::shared_ptr<Function> _Func;
typedef std::variant<_Int,_Float,bool,_Str,PyVarList,_CppFunc,_Func,std::shared_ptr<_Iterator>,_BoundedMethod,_Range,_Slice,_Pointer,Pointer> _Value;

const int _SIZEOF_VALUE = sizeof(_Value);

struct PyObject {
    PyVarDict attribs;
    _Value _native;
    PyVar _type;

    inline bool isType(const PyVar& type){
        return this->_type == type;
    }

    inline void setType(const PyVar& type){
        this->_type = type;
        this->attribs[__class__] = type;
    }

    // currently __name__ is only used for 'type'
    _Str getName(){
        _Value val = attribs[__name__]->_native;
        return std::get<_Str>(val);
    }

    _Str getTypeName(){
        return _type->getName();
    }

    PyObject(const _Value& val): _native(val) {}
    PyObject(_Value&& val): _native(std::move(val)) {}
};
