

#pragma once
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS 1
#endif
#ifdef _MSC_VER
#  include <stdint.h>
#  if _MSC_VER >= 1700
#    include <inttypes.h>
#  else
#    define PRId64 "lld"
#  endif
#else
#  include <inttypes.h>
#endif


enum mpd_Type
{
	mpdt_None,
	mpdt_Struct,
	mpdt_Pointer,
	mpdt_Enum,
	mpdt_ConstString,
	mpdt_Bool,
	mpdt_Int8,
	mpdt_Int16,
	mpdt_Int32,
	mpdt_Int64,
	mpdt_UInt8,
	mpdt_UInt16,
	mpdt_UInt32,
	mpdt_UInt64,
	mpdt_Float32,
	mpdt_Float64,
};

inline bool mpd_TypeIsInteger( mpd_Type t )
{
	return t == mpdt_Int8
		|| t == mpdt_Int16
		|| t == mpdt_Int32
		|| t == mpdt_Int64
		|| t == mpdt_UInt8
		|| t == mpdt_UInt16
		|| t == mpdt_UInt32
		|| t == mpdt_UInt64;
}

inline bool mpd_TypeIsFloat( mpd_Type t )
{
	return t == mpdt_Float32 || t == mpdt_Float64;
}

inline const char* mpd_TypeToName( mpd_Type t )
{
	switch( t )
	{
	case mpdt_None: return "<none>";
	case mpdt_Struct: return "<struct>";
	case mpdt_Pointer: return "<pointer>";
	case mpdt_Enum: return "<enum>";
	case mpdt_ConstString: return "<const_string>";
	case mpdt_Bool: return "bool";
	case mpdt_Int8: return "int8";
	case mpdt_Int16: return "int16";
	case mpdt_Int32: return "int32";
	case mpdt_Int64: return "int64";
	case mpdt_UInt8: return "uint8";
	case mpdt_UInt16: return "uint16";
	case mpdt_UInt32: return "uint32";
	case mpdt_UInt64: return "uint64";
	case mpdt_Float32: return "float32";
	case mpdt_Float64: return "float64";
	default: return "<unknown>";
	}
}

struct mpd_StringView
{
	const char* str;
	size_t size;
	
	static mpd_StringView create( const char* str ){ mpd_StringView out = { str, strlen( str ) }; return out; }
	static mpd_StringView create( const char* str, size_t size ){ mpd_StringView out = { str, size }; return out; }
};

//
// --- DUMP INFO ---
//
inline void __mpd_reprint( const char* text, int count )
{
	size_t sz = strlen( text );
	while( count --> 0 )
		fwrite( text, 1, sz, stdout );
}

#define MPD_STATICDUMP_ARGS(ty) ty const* pdata, int limit, int level
#define MPD_DUMPLEV( add ) __mpd_reprint( "\t", level + add )
#define MPD_DUMP_PROP( ty, name, getter ) __mpd_reprint( "\t", level + 1 ); printf( "%s = ", #name ); mpd_DumpData<ty>( (ty const&) getter, limit, level + 1 ); printf( "\n" );
#define MPD_DUMPDATA_ARGS(ty) ty const& data, int limit, int level
#define MPD_DUMPDATA_WRAPPER(ty,nty) template<> inline void mpd_DumpData<nty>( MPD_DUMPDATA_ARGS(nty) ){ ty##_MPD::dump( &data, limit, level ); }
#define MPD_DUMPDATA_USEARGS (void) data; (void) limit; (void) level;
#define MPD_DUMPDATA_USESTATICARGS (void) pdata; (void) limit; (void) level;
template< class T > void mpd_DumpData( T const& data, int limit = 5, int level = 0 ){ printf( "<unknown>" ); }
template<> inline void mpd_DumpData<bool>( MPD_DUMPDATA_ARGS(bool) ){ MPD_DUMPDATA_USEARGS; printf( "bool (%s)", data ? "true" : "false" ); }
template<> inline void mpd_DumpData<int8_t>( MPD_DUMPDATA_ARGS(int8_t) ){ MPD_DUMPDATA_USEARGS; printf( "int8 (%d)", (int) data ); }
template<> inline void mpd_DumpData<int16_t>( MPD_DUMPDATA_ARGS(int16_t) ){ MPD_DUMPDATA_USEARGS; printf( "int16 (%d)", (int) data ); }
template<> inline void mpd_DumpData<int32_t>( MPD_DUMPDATA_ARGS(int32_t) ){ MPD_DUMPDATA_USEARGS; printf( "int32 (%d)", (int) data ); }
template<> inline void mpd_DumpData<int64_t>( MPD_DUMPDATA_ARGS(int64_t) ){ MPD_DUMPDATA_USEARGS; printf( "int64 (%d)", (int) data ); }
template<> inline void mpd_DumpData<uint8_t>( MPD_DUMPDATA_ARGS(uint8_t) ){ MPD_DUMPDATA_USEARGS; printf( "uint8 (%d)", (int) data ); }
template<> inline void mpd_DumpData<uint16_t>( MPD_DUMPDATA_ARGS(uint16_t) ){ MPD_DUMPDATA_USEARGS; printf( "uint16 (%d)", (int) data ); }
template<> inline void mpd_DumpData<uint32_t>( MPD_DUMPDATA_ARGS(uint32_t) ){ MPD_DUMPDATA_USEARGS; printf( "uint32 (%d)", (int) data ); }
template<> inline void mpd_DumpData<uint64_t>( MPD_DUMPDATA_ARGS(uint64_t) ){ MPD_DUMPDATA_USEARGS; printf( "uint64 (%d)", (int) data ); }
template<> inline void mpd_DumpData<float>( MPD_DUMPDATA_ARGS(float) ){ MPD_DUMPDATA_USEARGS; printf( "float32 (%f)", data ); }
template<> inline void mpd_DumpData<double>( MPD_DUMPDATA_ARGS(double) ){ MPD_DUMPDATA_USEARGS; printf( "float64 (%f)", data ); }
template<> inline void mpd_DumpData<mpd_StringView>( MPD_DUMPDATA_ARGS(mpd_StringView) )
{
	MPD_DUMPDATA_USEARGS;
	printf( "[%d]\"", (int) data.size );
	fwrite( data.str, data.size, 1, stdout );
	printf( "\"" );
}

//
// --- TYPE CONVERSION / DATA TRANSPORT ---
//
#define mpd_KeyValue_NONE_INIT { 0, 0, 0, 0, 0, 0 }
struct mpd_KeyValue
{
	const char* key;
	size_t keysz;
	const char* value;
	size_t valuesz;
	int32_t value_i32;
	float value_float;
	
	const mpd_KeyValue* find_ext( const char* name, size_t size ) const
	{
		const mpd_KeyValue* kv = this;
		while( kv->key )
		{
			if( kv->keysz == size && memcmp( kv->key, name, size ) == 0 )
				return kv;
			++kv;
		}
		return NULL;
	}
	const mpd_KeyValue* find( const char* name ) const { return find_ext( name, strlen( name ) ); }
	static const mpd_KeyValue* none()
	{
		static const mpd_KeyValue data = mpd_KeyValue_NONE_INIT;
		return &data;
	}
};

#define mpd_TypeInfo_NONE_INIT { 0, mpdt_None, 0 }
struct mpd_TypeInfo
{
	const char* name;
	mpd_Type cls;
	const struct virtual_MPD* virt;
};

#define MPD_PROP_HAS_GETTER   0x0001
#define MPD_PROP_HAS_SETTER   0x0002
#define MPD_PROP_BOXED_GETTER 0x0004

#define mpd_PropInfo_NONE_INIT { 0, 0, mpd_TypeInfo_NONE_INIT, 0, 0 }
struct mpd_PropInfo
{
	const char* name;
	size_t namesz;
	mpd_TypeInfo type;
	unsigned flags;
	const mpd_KeyValue* metadata;
	
	static const mpd_PropInfo* none()
	{
		static const mpd_PropInfo data = mpd_PropInfo_NONE_INIT;
		return &data;
	}
};

#define mpd_MethodInfo_NONE_INIT { 0, 0, 0, 0, 0 }
struct mpd_MethodInfo
{
	const char* name;
	size_t namesz;
	const mpd_TypeInfo* params;
	int paramcount;
	const mpd_KeyValue* metadata;
	
	static const mpd_MethodInfo* none()
	{
		static const mpd_MethodInfo data = mpd_MethodInfo_NONE_INIT;
		return &data;
	}
};

#define mpd_EnumValue_NONE_INIT { 0, 0, 0, 0 }
struct mpd_EnumValue
{
	const char* name;
	size_t namesz;
	int64_t value;
	const mpd_KeyValue* metadata;
	
	static const mpd_EnumValue* none()
	{
		static const mpd_EnumValue data = mpd_EnumValue_NONE_INIT;
		return &data;
	}
};

struct virtual_MPD
{
	virtual const char* vname() const { return 0; }
	virtual const mpd_KeyValue* vmetadata() const { return 0; }
	virtual int vpropcount() const { return 0; }
	virtual const mpd_PropInfo* vprops() const { return 0; }
	virtual const mpd_TypeInfo* vindextypes() const { return 0; }
	virtual int vvaluecount() const { return 0; }
	virtual const mpd_EnumValue* vvalues() const { return 0; }
	virtual int vmethodcount() const { return 0; }
	virtual const mpd_MethodInfo* vmethods() const { return 0; }
	virtual struct mpd_Variant vgetprop( const void*, int ) const;
	virtual bool vsetprop( void*, int, const mpd_Variant& ) const { return false; }
	virtual struct mpd_Variant vgetindex( const void*, const mpd_Variant& ) const;
	virtual bool vsetindex( void*, const mpd_Variant&, const mpd_Variant& ) const { return false; }
	virtual void vmethodcall( void*, int, const mpd_Variant*, int ) const {}
	virtual void vdump( const void*, int, int ) const {}
	virtual void vdump_enumval( int64_t, int, int ) const {}
	
	virtual void vdestruct( void* ) const {}
	virtual const mpd_PropInfo* vprop( int ) const { return 0; }
	virtual const mpd_PropInfo* vfindprop_ext( const char*, size_t ) const { return 0; }
	virtual const mpd_PropInfo* vfindprop( const char* ) const { return 0; }
	virtual int vfindpropid_ext( const char*, size_t ) const { return -1; }
	virtual int vfindpropid( const char* ) const { return -1; }
	virtual int vprop2id( const mpd_PropInfo* ) const { return -1; }
	virtual const mpd_EnumValue* vvalue( int ) const { return 0; }
	virtual const char* vvalue2name( int64_t, const char* def = "<unknown>" ) const { return def; }
	virtual int64_t vname2value( mpd_StringView, int64_t def = 0 ) const { return def; }
	virtual int vvalue2num( int64_t, int def = -1 ) const { return def; }
	virtual const mpd_MethodInfo* vmethod( int ) const { return 0; }
	virtual const mpd_MethodInfo* vfindmethod_ext( const char*, size_t ) const { return 0; }
	virtual const mpd_MethodInfo* vfindmethod( const char* ) const { return 0; }
	virtual int vfindmethodid_ext( const char*, size_t ) const { return -1; }
	virtual int vfindmethodid( const char* ) const { return -1; }
	virtual int vmethod2id( const mpd_MethodInfo* ) const { return -1; }
};

struct none_MPD : virtual_MPD
{
	static none_MPD* inst(){ static none_MPD none; return &none; }
	const char* vname() const { return "none"; }
	const mpd_KeyValue* vmetadata() const { return mpd_KeyValue::none(); }
	const mpd_PropInfo* vprops() const { return mpd_PropInfo::none(); }
	const mpd_EnumValue* vvalues() const { return mpd_EnumValue::none(); }
	const mpd_MethodInfo* vmethods() const { return mpd_MethodInfo::none(); }
};

template< class T > struct mpd_MetaType : none_MPD
{
};

struct mpd_BoxData
{
	template< class T > static mpd_BoxData* create( const T& v )
	{
		struct Data
		{
			mpd_BoxData header;
			T data;
		};
		assert( offsetof( Data, header ) == 0 );
		
		mpd_BoxData* data = (mpd_BoxData*) malloc( sizeof(Data) );
		data->refcount = 0;
		data->offset = offsetof( Data, data );
		new (data->data()) T(v);
		return data;
	}
	
	int32_t refcount;
	unsigned offset; // don't need 64 bits here
	
	void* data(){ return ((char*)this) + offset; }
	void acquire(){ refcount++; }
	void release( const virtual_MPD* info )
	{
		refcount--;
		assert( refcount >= 0 );
		if( refcount == 0 )
		{
			info->vdestruct( data() );
			free( this );
		}
	}
};

struct mpd_Variant
{
	enum TagEnum { Enum };
	enum TagBox { Box };
	
	mpd_Variant() : type( mpdt_None ), mpdata( none_MPD::inst() ), box( NULL ){}
	template< class T > mpd_Variant( T* v ) : type( mpdt_Pointer ), mpdata( mpd_MetaType<T>::inst() ), box( NULL ){ data.p = const_cast<void*>((const void*) v); }
	template< class T > mpd_Variant( T& v ) : type( mpdt_Struct ), mpdata( mpd_MetaType<T>::inst() ), box( NULL ){ data.p = const_cast<void*>((const void*) &v); }
	template< class T > mpd_Variant( T v, TagEnum ) : type( mpdt_Enum ), mpdata( mpd_MetaType<T>::inst() ), box( NULL ){ data.i = v; }
	mpd_Variant( mpd_Variant& p ) : type( p.type ), mpdata( p.mpdata ), data( p.data ), box( NULL ){}
	mpd_Variant( bool v ) : type( mpdt_Bool ), mpdata( none_MPD::inst() ), box( NULL ){ data.u = v ? 1 : 0; }
	mpd_Variant( int8_t v ) : type( mpdt_Int8 ), mpdata( none_MPD::inst() ), box( NULL ){ data.i = v; }
	mpd_Variant( int16_t v ) : type( mpdt_Int16 ), mpdata( none_MPD::inst() ), box( NULL ){ data.i = v; }
	mpd_Variant( int32_t v ) : type( mpdt_Int32 ), mpdata( none_MPD::inst() ), box( NULL ){ data.i = v; }
	mpd_Variant( int64_t v ) : type( mpdt_Int64 ), mpdata( none_MPD::inst() ), box( NULL ){ data.i = v; }
	mpd_Variant( uint8_t v ) : type( mpdt_UInt8 ), mpdata( none_MPD::inst() ), box( NULL ){ data.u = v; }
	mpd_Variant( uint16_t v ) : type( mpdt_UInt16 ), mpdata( none_MPD::inst() ), box( NULL ){ data.u = v; }
	mpd_Variant( uint32_t v ) : type( mpdt_UInt32 ), mpdata( none_MPD::inst() ), box( NULL ){ data.u = v; }
	mpd_Variant( uint64_t v ) : type( mpdt_UInt64 ), mpdata( none_MPD::inst() ), box( NULL ){ data.u = v; }
	mpd_Variant( float v ) : type( mpdt_Float32 ), mpdata( none_MPD::inst() ), box( NULL ){ data.f = v; }
	mpd_Variant( double v ) : type( mpdt_Float64 ), mpdata( none_MPD::inst() ), box( NULL ){ data.f = v; }
	mpd_Variant( const char* str ) : type( mpdt_ConstString ), mpdata( none_MPD::inst() ), box( NULL ){ data.s.str = str; data.s.size = strlen( str ); }
	mpd_Variant( const char* str, size_t size ) : type( mpdt_ConstString ), mpdata( none_MPD::inst() ), box( NULL ){ data.s.str = str; data.s.size = size; }
	mpd_Variant( mpd_StringView sv ) : type( mpdt_ConstString ), mpdata( none_MPD::inst() ), box( NULL ){ data.s = sv; }
	template< class T > mpd_Variant( const T& v, TagBox ) : type( mpdt_Struct ), mpdata( mpd_MetaType<T>::inst() ), box( mpd_BoxData::create( v ) ){ box->acquire(); data.p = box->data(); }
	~mpd_Variant()
	{
		if( box )
			box->release( mpdata );
	}
	mpd_Variant( const mpd_Variant& v ) : type( v.type ), mpdata( v.mpdata ), data( v.data ), box( v.box )
	{
		if( box )
			box->acquire();
	}
	mpd_Variant& operator = ( const mpd_Variant& v )
	{
		if( box )
			box->release( mpdata );
		type = v.type;
		mpdata = v.mpdata;
		data = v.data;
		box = v.box;
		if( box )
			box->acquire();
		return *this;
	}
	
	const char* get_name() const
	{
		if( mpdata )
			return mpdata->vname();
		else
			return mpd_TypeToName( type );
	}
	mpd_Type get_type() const { return type; }
	const virtual_MPD* get_typeinfo() const { return mpdata; }
	
	mpd_Variant get_target() const
	{
		if( type == mpdt_Pointer )
		{
			mpd_Variant p = *this;
			p.type = data.p ? mpdt_Struct : mpdt_None;
			return p;
		}
		return *this;
	}
	mpd_StringView get_stringview() const
	{
		if( type == mpdt_ConstString )
			return data.s;
		mpd_StringView sv = { NULL, 0 };
		return sv;
	}
	bool get_bool() const
	{
		mpd_Variant p = get_target();
		switch( p.type )
		{
		case mpdt_Enum:
		case mpdt_Int8:
		case mpdt_Int16:
		case mpdt_Int32:
		case mpdt_Int64:
			return 0 != p.data.i;
		case mpdt_Bool:
		case mpdt_UInt8:
		case mpdt_UInt16:
		case mpdt_UInt32:
		case mpdt_UInt64:
			return 0 != p.data.u;
		case mpdt_Float32:
		case mpdt_Float64:
			return 0 != p.data.f;
		default:
			return false;
		}
	}
	template< class T > T _get_numeric() const
	{
		mpd_Variant p = get_target();
		switch( p.type )
		{
		case mpdt_Enum:
		case mpdt_Int8:
		case mpdt_Int16:
		case mpdt_Int32:
		case mpdt_Int64:
			return (T) p.data.i;
		case mpdt_Bool:
		case mpdt_UInt8:
		case mpdt_UInt16:
		case mpdt_UInt32:
		case mpdt_UInt64:
			return (T) p.data.u;
		case mpdt_Float32:
		case mpdt_Float64:
			return (T) p.data.f;
		default:
			return (T) 0;
		}
	}
	int64_t get_enum() const { return _get_numeric<int64_t>(); }
	int8_t get_int8() const { return _get_numeric<int8_t>(); }
	int16_t get_int16() const { return _get_numeric<int16_t>(); }
	int32_t get_int32() const { return _get_numeric<int32_t>(); }
	int64_t get_int64() const { return _get_numeric<int64_t>(); }
	uint8_t get_uint8() const { return _get_numeric<uint8_t>(); }
	uint16_t get_uint16() const { return _get_numeric<uint16_t>(); }
	uint32_t get_uint32() const { return _get_numeric<uint32_t>(); }
	uint64_t get_uint64() const { return _get_numeric<uint64_t>(); }
	float get_float32() const { return _get_numeric<float>(); }
	double get_float64() const { return _get_numeric<double>(); }
	template< class T > T get_obj() const
	{
		typedef mpd_MetaType<T> MT;
		if( ( type == mpdt_Struct || type == mpdt_Pointer ) && mpdata == MT::inst() )
			return *(T*) data.p;
		else
			return T();
	}
	template< class T > bool set_obj( const T& val ) const
	{
		typedef mpd_MetaType<T> MT;
		if( ( type == mpdt_Struct || type == mpdt_Pointer ) && mpdata == MT::inst() )
		{
			*(T*) data.p = val;
			return true;
		}
		return false;
	}
	template< class T > T* get_ptr() const
	{
		typedef mpd_MetaType<T> MT;
		if( ( type == mpdt_Struct || type == mpdt_Pointer ) && mpdata == MT::inst() )
			return (T*) data.p;
		else
			return NULL;
	}
	const void* get_voidptr() const
	{
		return type == mpdt_Struct || type == mpdt_Pointer ? data.p : NULL;
	}
	
	bool operator == ( const mpd_Variant& o ) const
	{
		if( type != o.type )
			return false;
		switch( type )
		{
		case mpdt_Struct:
		case mpdt_Pointer:
			return data.p == o.data.p;
		case mpdt_ConstString:
			return data.s.str == o.data.s.str && data.s.size == o.data.s.size;
		case mpdt_Enum:
		case mpdt_Int8:
		case mpdt_Int16:
		case mpdt_Int32:
		case mpdt_Int64:
			return data.i == o.data.i;
		case mpdt_Bool:
		case mpdt_UInt8:
		case mpdt_UInt16:
		case mpdt_UInt32:
		case mpdt_UInt64:
			return data.u == o.data.u;
		case mpdt_Float32:
		case mpdt_Float64:
			return data.f == o.data.f;
		default:
			return false;
		}
	}
	bool operator != ( const mpd_Variant& o ) const { return !( *this == o ); }
	
	mpd_Variant getpropbyid( int id ) const
	{
		if( ( type == mpdt_Struct || type == mpdt_Pointer ) && data.p )
			return mpdata->vgetprop( data.p, id );
		return mpd_Variant();
	}
	mpd_Variant getprop( const char* name ) const { return getpropbyid( mpdata->vfindpropid( name ) ); }
	mpd_Variant getprop_ext( const char* name, size_t sz ) const { return getpropbyid( mpdata->vfindpropid_ext( name, sz ) ); }
	bool setpropbyid( int id, const mpd_Variant& val ) const
	{
		if( ( type == mpdt_Struct || type == mpdt_Pointer ) && data.p )
			return mpdata->vsetprop( data.p, id, val );
		return false;
	}
	bool setprop( const char* name, const mpd_Variant& val ) const { return setpropbyid( mpdata->vfindpropid( name ), val ); }
	bool setprop_ext( const char* name, size_t sz, const mpd_Variant& val ) const { return setpropbyid( mpdata->vfindpropid_ext( name, sz ), val ); }
	mpd_Variant getindex( const mpd_Variant& key ) const { return mpdata->vgetindex( data.p, key ); }
	bool setindex( const mpd_Variant& key, const mpd_Variant& val ) const { return mpdata->vsetindex( data.p, key, val ); }
	bool methodcallbyid( int id, const mpd_Variant* args, int argcount ) const
	{
		if( ( type == mpdt_Struct || type == mpdt_Pointer ) && data.p && id >= 0 && id < mpdata->vmethodcount() )
		{
			mpdata->vmethodcall( data.p, id, args, argcount );
			return true;
		}
		return false;
	}
	bool methodcall( const char* name, const mpd_Variant* args, int argcount ) const
	{
		if( ( type == mpdt_Struct || type == mpdt_Pointer ) && data.p )
		{
			int id = mpdata->vfindmethodid( name );
			if( id < 0 )
				return false;
			mpdata->vmethodcall( data.p, id, args, argcount );
			return true;
		}
		return false;
	}
	const mpd_KeyValue* metadata() const { return mpdata->vmetadata(); }
	void dump( int limit = 5, int level = 0 ) const
	{
		switch( type )
		{
		default: printf( "<error>" ); break;
		case mpdt_None: printf( "<none>" ); break;
		case mpdt_Struct:
		case mpdt_Pointer:
			mpdata->vdump( data.p, limit, level );
			break;
		case mpdt_Enum:
			mpdata->vdump_enumval( data.i, limit, level );
			break;
		case mpdt_ConstString:
			printf( "[%d]\"", (int) data.s.size );
			fwrite( data.s.str, data.s.size, 1, stdout );
			printf( "\"" );
			break;
		case mpdt_Bool: mpd_DumpData( (bool) data.u, limit, level ); break;
		case mpdt_Int8: mpd_DumpData( (int8_t) data.i, limit, level ); break;
		case mpdt_Int16: mpd_DumpData( (int16_t) data.i, limit, level ); break;
		case mpdt_Int32: mpd_DumpData( (int32_t) data.i, limit, level ); break;
		case mpdt_Int64: mpd_DumpData( (int64_t) data.i, limit, level ); break;
		case mpdt_UInt8: mpd_DumpData( (uint8_t) data.u, limit, level ); break;
		case mpdt_UInt16: mpd_DumpData( (uint16_t) data.u, limit, level ); break;
		case mpdt_UInt32: mpd_DumpData( (uint32_t) data.u, limit, level ); break;
		case mpdt_UInt64: mpd_DumpData( (uint64_t) data.u, limit, level ); break;
		case mpdt_Float32: mpd_DumpData( (float) data.f, limit, level ); break;
		case mpdt_Float64: mpd_DumpData( (double) data.f, limit, level ); break;
		}
	}
	
private:
	mpd_Type type;
	const virtual_MPD* mpdata;
	union mpdVariant_Data
	{
		mpd_StringView s;
		void* p;
		int64_t i;
		uint64_t u;
		double f;
	}
	data;
	mpd_BoxData* box;
};
template<> inline void mpd_DumpData<mpd_Variant>( MPD_DUMPDATA_ARGS(mpd_Variant) ){ data.dump( limit, level ); }

mpd_Variant inline virtual_MPD::vgetprop( const void*, int ) const
{
	return mpd_Variant();
}
mpd_Variant inline virtual_MPD::vgetindex( const void*, const mpd_Variant& ) const
{
	return mpd_Variant();
}

template< class T > inline T mpd_var_get( const mpd_Variant& v ){ return v.get_obj<T>(); }
template<> inline bool mpd_var_get<bool>( const mpd_Variant& v ){ return v.get_bool(); }
template<> inline int8_t mpd_var_get<int8_t>( const mpd_Variant& v ){ return v.get_int8(); }
template<> inline int16_t mpd_var_get<int16_t>( const mpd_Variant& v ){ return v.get_int16(); }
template<> inline int32_t mpd_var_get<int32_t>( const mpd_Variant& v ){ return v.get_int32(); }
template<> inline int64_t mpd_var_get<int64_t>( const mpd_Variant& v ){ return v.get_int64(); }
template<> inline uint8_t mpd_var_get<uint8_t>( const mpd_Variant& v ){ return v.get_uint8(); }
template<> inline uint16_t mpd_var_get<uint16_t>( const mpd_Variant& v ){ return v.get_uint16(); }
template<> inline uint32_t mpd_var_get<uint32_t>( const mpd_Variant& v ){ return v.get_uint32(); }
template<> inline uint64_t mpd_var_get<uint64_t>( const mpd_Variant& v ){ return v.get_uint64(); }
template<> inline float mpd_var_get<float>( const mpd_Variant& v ){ return v.get_float32(); }
template<> inline double mpd_var_get<double>( const mpd_Variant& v ){ return v.get_float64(); }

struct mpd_Location
{
	mpd_Location( const mpd_Variant& c = mpd_Variant(), int p = -1 ) : cont( c ), prop_id( p ){}
	mpd_Location( const mpd_Variant& c, const char* propname ) : cont( c ), prop_id( c.get_typeinfo()->vfindpropid( propname ) ){}
	
	const mpd_PropInfo* get_propinfo() const { return cont.get_typeinfo()->vprop( prop_id ); }
	const mpd_KeyValue* prop_metadata() const
	{
		const mpd_PropInfo* prop = get_propinfo();
		return prop ? prop->metadata : mpd_KeyValue::none();
	}
	mpd_Variant locvar() const
	{
		const mpd_PropInfo* prop = get_propinfo();
		if( prop && !( prop->flags & MPD_PROP_BOXED_GETTER ) )
			return cont.getpropbyid( prop_id );
		return cont;
	}
	bool has_prop() const { return prop_id >= 0 && (unsigned) prop_id < (unsigned) cont.get_typeinfo()->vpropcount(); }
	mpd_Location subprop( const char* name ) const { return mpd_Location( locvar(), name ); }
	const void* get_ptr() const { return locvar().get_voidptr(); }
	bool operator == ( const mpd_Location& o ) const
	{
		if( cont == o.cont && prop_id == o.prop_id )
			return true;
		const void* p1 = get_ptr();
		const void* p2 = o.get_ptr();
		return p1 && p2 && p1 == p2;
	}
	bool operator != ( const mpd_Location& o ) const { return !( *this == o ); }
	void contains_many( const mpd_Location* llist, int count, bool* outlist ) const
	{
		for( int i = 0; i < count; ++i )
		{
			if( !outlist[ i ] && *this == llist[ i ] )
				outlist[ i ] = true;
		}
		mpd_Variant ch = locvar();
		for( int i = 0, iend = ch.get_typeinfo()->vpropcount(); i < iend; ++i )
		{
			mpd_Location( ch, i ).contains_many( llist, count, outlist );
		}
	}
	bool contains( const mpd_Location& o ) const
	{
		if( *this == o )
			return true;
		mpd_Variant ch = locvar();
		for( int i = 0, iend = ch.get_typeinfo()->vpropcount(); i < iend; ++i )
		{
			if( mpd_Location( ch, i ).contains( o ) )
				return true;
		}
		return false;
	}
	
	// variant API wrapping
	mpd_Variant get_var() const { return has_prop() ? cont.getpropbyid( prop_id ) : cont; }
	bool set_var( const mpd_Variant& v ) const { return cont.setpropbyid( prop_id, v ); }
	
	mpd_Variant cont;
	int prop_id;
};

//
// --- METADATA ---
//
inline void mpd_DumpMetadata( const mpd_KeyValue* md )
{
	if( md->key )
	{
		printf( " [" );
		while( md->key )
		{
			printf( "%s = %s%s", md->key, md->value, md[1].key ? ", " : "" );
			++md;
		}
		printf( "]" );
	}
}
template< class T > inline void mpd_DumpInfo()
{
	typedef mpd_MetaType<T> MT;
	printf( "%s %s", MT::valuecount() ? "enum" : "struct", MT::name() );
	mpd_DumpMetadata( MT::metadata() );
	printf( "\n{\n" );
	if( MT::valuecount() )
	{
		const mpd_EnumValue* v = MT::values();
		while( v->name )
		{
			printf( "\t%s = %" PRId64, v->name, v->value );
			mpd_DumpMetadata( v->metadata );
			puts("");
			++v;
		}
	}
	else
	{
		const mpd_TypeInfo* indextypes = MT::indextypes();
		if( indextypes )
		{
			printf( "\tindex [%s => %s]\n", indextypes[0].name, indextypes[1].name );
		}
		const mpd_PropInfo* p = MT::props();
		while( p->name )
		{
			printf( "\t%s %s", p->type.name, p->name );
			mpd_DumpMetadata( p->metadata );
			puts("");
			++p;
		}
	}
	printf( "}\n" );
}

inline void mpd_DumpInfo( const virtual_MPD* type, int limit = 5, int level = 0, bool _int_subprop = false )
{
	if( !_int_subprop )
		__mpd_reprint( "\t", level );
	printf( "%s %s", type->vvaluecount() ? "enum" : "struct", type->vname() );
	mpd_DumpMetadata( type->vmetadata() );
	printf( "\n" );
	
	__mpd_reprint( "\t", level );
	printf( "{\n" );
	level++;
	
	if( type->vvaluecount() )
	{
		const mpd_EnumValue* v = type->vvalues();
		while( v->name )
		{
			__mpd_reprint( "\t", level );
			printf( "%s = %" PRId64, v->name, v->value );
			mpd_DumpMetadata( v->metadata );
			puts("");
			++v;
		}
	}
	else
	{
		const mpd_TypeInfo* indextypes = type->vindextypes();
		if( indextypes )
		{
			__mpd_reprint( "\t", level );
			printf( "index [%s => %s]\n", indextypes[0].name, indextypes[1].name );
		}
		const mpd_PropInfo* p = type->vprops();
		while( p->name )
		{
			__mpd_reprint( "\t", level );
			printf( "%s", p->name );
			mpd_DumpMetadata( p->metadata );
			printf( ": " );
			if( p->type.cls == mpdt_Struct && p->type.virt && level < limit )
			{
				mpd_DumpInfo( p->type.virt, limit, level, true );
			}
			else
				printf( "%s\n", p->type.name );
			++p;
		}
	}
	
	level--;
	__mpd_reprint( "\t", level );
	printf( "}\n" );
}

template< class T, class ST > struct struct_MPD : virtual_MPD
{
	typedef ST type;
	
	// variant/enum dumping hack
	static void dump_enumval( MPD_STATICDUMP_ARGS(int64_t) ){ MPD_DUMPDATA_USESTATICARGS; }
	
	// helper functions
	static const mpd_PropInfo* prop( int i )
	{
		if( unsigned(i) >= unsigned(T::propcount()) )
			return NULL;
		return &T::props()[ i ];
	}
	static const mpd_PropInfo* findprop_ext( const char* name, size_t namesz )
	{
		const mpd_PropInfo* p = T::props();
		while( p->name )
		{
			if( p->namesz == namesz && memcmp( p->name, name, namesz ) == 0 )
				return p;
			++p;
		}
		return NULL;
	}
	static const mpd_PropInfo* findprop( const char* name )
	{
		return findprop_ext( name, strlen( name ) );
	}
	static int findpropid_ext( const char* name, size_t namesz )
	{
		const mpd_PropInfo* p = findprop_ext( name, namesz );
		return p ? ( p - T::props() ) : -1;
	}
	static int findpropid( const char* name )
	{
		const mpd_PropInfo* p = findprop( name );
		return p ? ( p - T::props() ) : -1;
	}
	static int prop2id( const mpd_PropInfo* p )
	{
		int pid = ( p - T::props() );
		if( unsigned(pid) >= unsigned(T::propcount()) )
			return -1;
		return pid;
	}
	
	static const mpd_EnumValue* value( int i )
	{
		if( unsigned(i) >= unsigned(T::valuecount()) )
			return NULL;
		return &T::values()[ i ];
	}
	static const char* value2name( int64_t val, const char* def = "<unknown>" )
	{
		const mpd_EnumValue* v = T::values();
		while( v->name )
		{
			if( v->value == val )
				return v->name;
			++v;
		}
		return def;
	}
	static int64_t name2value( mpd_StringView name, int64_t def = 0 )
	{
		const mpd_EnumValue* v = T::values();
		while( v->name )
		{
			if( v->namesz == name.size && memcmp( name.str, v->name, name.size ) == 0 )
				return v->value;
			++v;
		}
		return def;
	}
	static int value2num( int64_t val, int def = -1 )
	{
		const mpd_EnumValue* v = T::values();
		while( v->name )
		{
			if( v->value == val )
				return v - T::values();
			++v;
		}
		return def;
	}
	
	static const mpd_MethodInfo* method( int i )
	{
		if( unsigned(i) >= unsigned(T::methodcount()) )
			return NULL;
		return &T::methods()[ i ];
	}
	static const mpd_MethodInfo* findmethod_ext( const char* name, size_t namesz )
	{
		const mpd_MethodInfo* m = T::methods();
		while( m->name )
		{
			if( m->namesz == namesz && memcmp( m->name, name, namesz ) == 0 )
				return m;
			++m;
		}
		return NULL;
	}
	static const mpd_MethodInfo* findmethod( const char* name )
	{
		return findmethod_ext( name, strlen( name ) );
	}
	static int findmethodid_ext( const char* name, size_t namesz )
	{
		const mpd_MethodInfo* m = findmethod_ext( name, namesz );
		return m ? ( m - T::methods() ) : -1;
	}
	static int findmethodid( const char* name )
	{
		const mpd_MethodInfo* m = findmethod( name );
		return m ? ( m - T::methods() ) : -1;
	}
	static int method2id( const mpd_MethodInfo* m )
	{
		int mid = ( m - T::methods() );
		if( unsigned(mid) >= unsigned(T::methodcount()) )
			return -1;
		return mid;
	}
	
	// virtual wrappers
	// - core interface
	virtual const char* vname() const { return T::name(); }
	virtual const mpd_KeyValue* vmetadata() const { return T::metadata(); }
	virtual int vpropcount() const { return T::propcount(); }
	virtual const mpd_PropInfo* vprops() const { return T::props(); }
	virtual const mpd_TypeInfo* vindextypes() const { return T::indextypes(); }
	virtual int vvaluecount() const { return T::valuecount(); }
	virtual const mpd_EnumValue* vvalues() const { return T::values(); }
	virtual int vmethodcount() const { return T::methodcount(); }
	virtual const mpd_MethodInfo* vmethods() const { return T::methods(); }
	virtual mpd_Variant vgetprop( const void* obj, int p ) const { return T::getprop( (type const*) obj, p ); }
	virtual bool vsetprop( void* obj, int p, const mpd_Variant& data ) const { return T::setprop( (type*) obj, p, data ); }
	virtual struct mpd_Variant vgetindex( const void* obj, const mpd_Variant& key ) const { return T::getindex( (type const*) obj, key ); }
	virtual bool vsetindex( void* obj, const mpd_Variant& key, const mpd_Variant& val ) const { return T::setindex( (type*) obj, key, val ); }
	virtual void vmethodcall( void* obj, int id, const mpd_Variant* args, int argcount ) const { return T::methodcall( (type*) obj, id, args, argcount ); }
	virtual void vdump( const void* p, int limit, int level ) const { T::dump( (type const*) p, limit, level ); }
	virtual void vdump_enumval( int64_t v, int limit, int level ) const { T::dump_enumval( &v, limit, level ); }
	// - virtual helper function wrappers
	virtual void vdestruct( void* obj ) const { ((type*) obj)->~type(); }
	virtual const mpd_PropInfo* vprop( int i ) const { return prop( i ); }
	virtual const mpd_PropInfo* vfindprop_ext( const char* name, size_t namesz ) const { return findprop_ext( name, namesz ); }
	virtual const mpd_PropInfo* vfindprop( const char* name ) const { return findprop( name ); }
	virtual int vfindpropid_ext( const char* name, size_t sz ) const { return findpropid_ext( name, sz ); }
	virtual int vfindpropid( const char* name ) const { return findpropid( name ); }
	virtual int vprop2id( const mpd_PropInfo* p ) const { return prop2id( p ); }
	virtual const mpd_EnumValue* vvalue( int i ) const { return value( i ); }
	virtual const char* vvalue2name( int64_t val, const char* def = "<unknown>" ) const { return value2name( val, def ); }
	virtual int64_t vname2value( mpd_StringView name, int64_t def = 0 ) const { return name2value( name, def ); }
	virtual int vvalue2num( int64_t val, int def = -1 ) const { return value2num( val, def ); }
	virtual const mpd_MethodInfo* vmethod( int i ) const { return method( i ); }
	virtual const mpd_MethodInfo* vfindmethod_ext( const char* name, size_t namesz ) const { return findmethod_ext( name, namesz ); }
	virtual const mpd_MethodInfo* vfindmethod( const char* name ) const { return findmethod( name ); }
	virtual int vfindmethodid_ext( const char* name, size_t namesz ) const { return findmethodid_ext( name, namesz ); }
	virtual int vfindmethodid( const char* name ) const { return findmethodid( name ); }
	virtual int vmethod2id( const mpd_MethodInfo* m ) const { return method2id( m ); }
};

template< class T > inline const char* mpd_Value2Name( T val, const char* def = "<unknown>" )
{
	return mpd_MetaType<T>::value2name( val, def );
}
template< class T > inline T mpd_Name2Value( mpd_StringView name, T def = (T) 0 )
{
	return (T) mpd_MetaType<T>::name2value( name, def );
}

