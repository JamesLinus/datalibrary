/* copyright (c) 2010 Fredrik Kihlander, see LICENSE for more info */

#include <gtest/gtest.h>

#include <dl/dl.h>
#include <dl/dl_txt.h>

#include "dl_test_common.h"

// TODO: add test for default values for uint*[] with true/false.

#define STRINGIFY( ... ) #__VA_ARGS__

class DLText : public DL {};

TEST_F(DLText, member_order)
{
	// test to pack a txt-instance that is not in order!
	const char* TextData =
	"{"
		"\"type\" : \"Pods\","
		"\"data\" : {"
			"\"i16\" : 2,"
			"\"u32\" : 7,"
			"\"u8\"  : 5,"
			"\"i32\" : 3,"
			"\"f64\" : 4.1234,"
			"\"i64\" : 4,"
			"\"u16\" : 6,"
			"\"u64\" : 8,"
			"\"i8\"  : 1,"
			"\"f32\" : 3.14"
		"}"
	"}";

	Pods P1;

 	unsigned char OutDataText[1024];

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, Pods::TYPE_ID, &P1, sizeof(Pods), OutDataText, 1024, 0x0 ));

	EXPECT_EQ(1,      P1.i8);
 	EXPECT_EQ(2,      P1.i16);
	EXPECT_EQ(3,      P1.i32);
	EXPECT_EQ(4,      P1.i64);
	EXPECT_EQ(5u,     P1.u8);
	EXPECT_EQ(6u,     P1.u16);
	EXPECT_EQ(7u,     P1.u32);
	EXPECT_EQ(8u,     P1.u64);
	EXPECT_EQ(3.14f,  P1.f32);
	EXPECT_EQ(4.1234, P1.f64);
}

TEST_F(DLText, set_member_twice)
{
	// test to pack a txt-instance that is not in order!
	const char* TextData = 
		"{"
			"\"type\" : \"Pods\","
			"\"data\" : {"
				"\"i16\" : 2,"
				"\"u32\" : 7,"
				"\"u8\"  : 5,"
				"\"i32\" : 3,"
				"\"i32\" : 7,"
				"\"f64\" : 4.1234,"
				"\"i64\" : 4,"
				"\"u16\" : 6,"
				"\"u64\" : 8,"
				"\"i8\"  : 1,"
				"\"f32\" : 3.14"
			"}"
		"}";

	unsigned char OutDataText[1024];

	EXPECT_DL_ERR_EQ(DL_ERROR_TXT_MEMBER_SET_TWICE, dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
}

TEST_F(DLText, member_not_exist)
{
	// test to pack a txt-instance that is not in order!
	const char* TextData = "{ \"type\" : \"Pods2\", \"data\" : { \"Int1\" : 1337, \"Int2\" : 1337, \"IDoNotExist\" : 1337 } }";

	unsigned char OutDataText[1024];

	EXPECT_DL_ERR_EQ(DL_ERROR_MEMBER_NOT_FOUND, dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
}

TEST_F(DLText, member_missing)
{
	// error should be cast if member is not set and has no default value!
	// in this case Int2 is not set!

	const char* TextData = "{ \"type\" : \"Pods2\", \"data\" : { \"Int1\" : 1337 } }";

	unsigned char OutDataText[1024];
	EXPECT_DL_ERR_EQ(DL_ERROR_TXT_MEMBER_MISSING, dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
}

TEST_F(DLText, default_value_pod)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"PodsDefaults\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	PodsDefaults P1;

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, PodsDefaults::TYPE_ID, &P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_EQ(2,     P1.i8);
	EXPECT_EQ(3,     P1.i16);
	EXPECT_EQ(4,     P1.i32);
	EXPECT_EQ(5,     P1.i64);
	EXPECT_EQ(6u,    P1.u8);
	EXPECT_EQ(7u,    P1.u16);
	EXPECT_EQ(8u,    P1.u32);
	EXPECT_EQ(9u,    P1.u64);
	EXPECT_EQ(10.0f, P1.f32);
	EXPECT_EQ(11.0,  P1.f64);
}

TEST_F(DLText, default_value_string)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultStr\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultStr P1[10]; // this is so ugly!

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultStr::TYPE_ID, P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_STREQ("cowbells ftw!", P1[0].Str);
}

TEST_F(DLText, default_value_with_data_in_struct)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultWithOtherDataBefore\", \"data\" : { \"t1\" : \"apa\" } }";

	unsigned char OutDataText[1024];
	DefaultWithOtherDataBefore P1[10]; // this is so ugly!

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultWithOtherDataBefore::TYPE_ID, P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_STREQ("apa", P1[0].t1);
	EXPECT_STREQ("who", P1[0].Str);
}

TEST_F(DLText, default_value_ptr)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultPtr\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultPtr P1 = { 0 }; // this is so ugly!

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultPtr::TYPE_ID, &P1, sizeof(DefaultPtr), OutDataText, 1024, 0x0));

	EXPECT_EQ(0x0, P1.Ptr);
}

TEST_F(DLText, zero_as_ptr_fail)
{
	// when referring to the root item, item 0, the types need to match.

	const char* TextData = "{ \"type\" : \"DefaultPtr\", \"data\" : { \"Ptr\" : 0 } }";
	unsigned char OutDataText[1024];
	EXPECT_DL_ERR_EQ(DL_ERROR_TXT_INVALID_MEMBER_TYPE, dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
}

TEST_F(DLText, default_value_struct)
{
	const char* TextData = "{ \"type\" : \"DefaultStruct\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultStruct P1; // this is so ugly!

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultStruct::TYPE_ID, &P1, sizeof(DefaultStruct), OutDataText, 1024, 0x0));

	EXPECT_EQ(13u, P1.Struct.Int1);
	EXPECT_EQ(37u, P1.Struct.Int2);
}

TEST_F(DLText, default_value_enum)
{
	const char* TextData = "{ \"type\" : \"DefaultEnum\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultEnum P1;

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultEnum::TYPE_ID, &P1, sizeof(DefaultEnum), OutDataText, 1024, 0x0));

	EXPECT_EQ(TESTENUM1_VALUE3, P1.Enum);
}

TEST_F(DLText, default_value_inline_array_pod)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultInlArrayPod\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultInlArrayPod P1;

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultInlArrayPod::TYPE_ID, &P1, sizeof(DefaultInlArrayPod), OutDataText, 1024, 0x0));

	EXPECT_EQ(1u, P1.Arr[0]);
	EXPECT_EQ(3u, P1.Arr[1]);
	EXPECT_EQ(3u, P1.Arr[2]);
	EXPECT_EQ(7u, P1.Arr[3]);
}

TEST_F(DLText, default_value_inline_array_enum)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultInlArrayEnum\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultInlArrayEnum P1;

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultInlArrayEnum::TYPE_ID, &P1, sizeof(DefaultInlArrayEnum), OutDataText, 1024, 0x0));

	EXPECT_EQ(TESTENUM1_VALUE3, P1.Arr[0]);
	EXPECT_EQ(TESTENUM1_VALUE1, P1.Arr[1]);
	EXPECT_EQ(TESTENUM1_VALUE2, P1.Arr[2]);
	EXPECT_EQ(TESTENUM1_VALUE4, P1.Arr[3]);
}

TEST_F(DLText, default_value_inline_array_string)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultInlArrayStr\", \"data\" : {} }";

	unsigned char OutDataText[1024];

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));

	DefaultInlArrayStr P1[10];

	// load binary
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultInlArrayStr::TYPE_ID, P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_STREQ("cow",   P1[0].Arr[0]);
	EXPECT_STREQ("bells", P1[0].Arr[1]);
	EXPECT_STREQ("are",   P1[0].Arr[2]);
	EXPECT_STREQ("cool",  P1[0].Arr[3]);
}

TEST_F(DLText, default_value_inline_array_array)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultInlArrayArray\", \"data\" : {} }";

	unsigned char OutDataText[1024];

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));

	DefaultInlArrayArray P1[10];

	// load binary
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultInlArrayArray::TYPE_ID, P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_EQ( 2u, P1[0].Arr[0].u32_arr.count );
	EXPECT_EQ( 2u, P1[0].Arr[1].u32_arr.count );

	EXPECT_EQ( 1u, P1[0].Arr[0].u32_arr[0] );
	EXPECT_EQ( 3u, P1[0].Arr[0].u32_arr[1] );
	EXPECT_EQ( 3u, P1[0].Arr[1].u32_arr[0] );
	EXPECT_EQ( 7u, P1[0].Arr[1].u32_arr[1] );
}

TEST_F(DLText, default_value_array_pod)
{
	const char* TextData = "{ \"type\" : \"DefaultArrayPod\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultArrayPod P1[10];

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultArrayPod::TYPE_ID, P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_EQ(4u, P1[0].Arr.count);

	EXPECT_EQ(1u, P1[0].Arr[0]);
	EXPECT_EQ(3u, P1[0].Arr[1]);
	EXPECT_EQ(3u, P1[0].Arr[2]);
	EXPECT_EQ(7u, P1[0].Arr[3]);
}

TEST_F(DLText, default_value_array_enum)
{
	const char* TextData = "{ \"type\" : \"DefaultArrayEnum\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultArrayEnum P1[10];

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultArrayEnum::TYPE_ID, P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_EQ(4u, P1[0].Arr.count);

	EXPECT_EQ(TESTENUM1_VALUE3, P1[0].Arr[0]);
	EXPECT_EQ(TESTENUM1_VALUE1, P1[0].Arr[1]);
	EXPECT_EQ(TESTENUM1_VALUE2, P1[0].Arr[2]);
	EXPECT_EQ(TESTENUM1_VALUE4, P1[0].Arr[3]);
}

TEST_F(DLText, default_value_array_string)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultArrayStr\", \"data\" : {} }";

	unsigned char OutDataText[1024];
	DefaultArrayStr P1[10];

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultArrayStr::TYPE_ID, P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_EQ(4u, P1[0].Arr.count);

	EXPECT_STREQ("cow",   P1[0].Arr[0]);
	EXPECT_STREQ("bells", P1[0].Arr[1]);
	EXPECT_STREQ("are",   P1[0].Arr[2]);
	EXPECT_STREQ("cool",  P1[0].Arr[3]);
}

TEST_F(DLText, default_value_array_array)
{
	// default-values should be set correctly!

	const char* TextData = "{ \"type\" : \"DefaultArrayArray\", \"data\" : {} }";

	unsigned char OutDataText[1024];

	EXPECT_DL_ERR_OK(dl_txt_pack(Ctx, TextData, OutDataText, 1024, 0x0));

	DefaultArrayArray P1[10];

	// load binary
	EXPECT_DL_ERR_OK(dl_instance_load(Ctx, DefaultArrayArray::TYPE_ID, P1, sizeof(P1), OutDataText, 1024, 0x0));

	EXPECT_EQ( 2u, P1[0].Arr.count );
	EXPECT_EQ( 2u, P1[0].Arr[0].u32_arr.count );
	EXPECT_EQ( 2u, P1[0].Arr[1].u32_arr.count );

	EXPECT_EQ( 1u, P1[0].Arr[0].u32_arr[0] );
	EXPECT_EQ( 3u, P1[0].Arr[0].u32_arr[1] );
	EXPECT_EQ( 3u, P1[0].Arr[1].u32_arr[0] );
	EXPECT_EQ( 7u, P1[0].Arr[1].u32_arr[1] );
}

TEST_F( DLText, test_alias )
{
	const char* text_data = "{"
								"\"type\" : \"with_alias_enum\","
								"\"data\" : {"
									"\"e1\" : \"MULTI_ALIAS1\","
									"\"e2\" : \"alias1\","
									"\"e3\" : \"alias2\","
									"\"e4\" : \"alias4\""
							    "}"
						    "}";
	with_alias_enum loaded;
	unsigned char out_data_text[1024];

	EXPECT_DL_ERR_OK( dl_txt_pack( Ctx, text_data, out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
	EXPECT_DL_ERR_OK( dl_instance_load( Ctx, with_alias_enum::TYPE_ID, &loaded, sizeof(loaded), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );

	EXPECT_EQ( MULTI_ALIAS1, loaded.e1 );
	EXPECT_EQ( MULTI_ALIAS1, loaded.e2 );
	EXPECT_EQ( MULTI_ALIAS1, loaded.e3 );
	EXPECT_EQ( MULTI_ALIAS2, loaded.e4 );
}

TEST_F( DLText, invalid_enum_value )
{
	const char* text_data = "{"
								"\"type\" : \"with_alias_enum\","
								"\"data\" : {"
									"\"e1\" : \"MULTI_ALIAS1\","
									"\"e2\" : \"I do not exist\","
									"\"e3\" : \"alias2\","
									"\"e4\" : \"alias4\""
							    "}"
						    "}";
	unsigned char out_data_text[1024];
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_INVALID_ENUM_VALUE, dl_txt_pack( Ctx, text_data, out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
}

TEST_F( DLText, invalid_data_format )
{
	unsigned char out_data_text[1024];
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( [] ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( { "type" : "Pods", "data" : [] }   ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( { "type" : "Pods", "data" : 1 }    ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( { "type" : "Pods", "data" : true } ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
}

TEST_F( DLText, invalid_type_format )
{
	unsigned char out_data_text[1024];
	// EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( [] ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( { "type" : {},   "data" : {} }   ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( { "type" : [],   "data" : {} }   ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( { "type" : 1,    "data" : {} }   ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_PARSE_ERROR, dl_txt_pack( Ctx, STRINGIFY( { "type" : true, "data" : {} }   ), out_data_text, DL_ARRAY_LENGTH(out_data_text), 0x0 ) );
}


TEST_F( DLText, basic_bool_all_true )
{
	const char* all_true_text =
	"{"
		"\"type\" : \"Pods\","
		"\"data\" : {"
			"\"u8\"  : true,"
			"\"u16\" : true,"
			"\"u32\" : true,"
			"\"u64\" : true,"
			"\"i8\"  : true,"
			"\"i16\" : true,"
			"\"i32\" : true,"
			"\"i64\" : true,"
			"\"f32\" : 0.0,"
			"\"f64\" : 0.0"
		"}"
	"}";

	Pods p1;

	unsigned char out_text_data[1024];

	EXPECT_DL_ERR_OK( dl_txt_pack( Ctx, all_true_text, out_text_data, DL_ARRAY_LENGTH(out_text_data), 0x0 ) );
	EXPECT_DL_ERR_OK( dl_instance_load( Ctx, Pods::TYPE_ID, &p1, sizeof(Pods), out_text_data, DL_ARRAY_LENGTH(out_text_data), 0x0 ) );

	EXPECT_EQ(1,    p1.i8);
	EXPECT_EQ(1,    p1.i16);
	EXPECT_EQ(1,    p1.i32);
	EXPECT_EQ(1,    p1.i64);
	EXPECT_EQ(1u,   p1.u8);
	EXPECT_EQ(1u,   p1.u16);
	EXPECT_EQ(1u,   p1.u32);
	EXPECT_EQ(1u,   p1.u64);
	EXPECT_EQ(0.0f, p1.f32);
	EXPECT_EQ(0.0,  p1.f64);
}

TEST_F( DLText, basic_bool_all_false )
{
	const char* all_false_text =
	"{"
		"\"type\" : \"Pods\","
		"\"data\" : {"
			"\"u8\"  : false,"
			"\"u16\" : false,"
			"\"u32\" : false,"
			"\"u64\" : false,"
			"\"i8\"  : false,"
			"\"i16\" : false,"
			"\"i32\" : false,"
			"\"i64\" : false,"
			"\"f32\" : 0.0,"
			"\"f64\" : 0.0"
		"}"
	"}";

	Pods p1;

	unsigned char out_text_data[1024];

	EXPECT_DL_ERR_OK( dl_txt_pack( Ctx, all_false_text, out_text_data, DL_ARRAY_LENGTH(out_text_data), 0x0 ) );
	EXPECT_DL_ERR_OK( dl_instance_load( Ctx, Pods::TYPE_ID, &p1, sizeof(Pods), out_text_data, DL_ARRAY_LENGTH(out_text_data), 0x0 ) );

	EXPECT_EQ(0,    p1.i8);
	EXPECT_EQ(0,    p1.i16);
	EXPECT_EQ(0,    p1.i32);
	EXPECT_EQ(0,    p1.i64);
	EXPECT_EQ(0u,   p1.u8);
	EXPECT_EQ(0u,   p1.u16);
	EXPECT_EQ(0u,   p1.u32);
	EXPECT_EQ(0u,   p1.u64);
	EXPECT_EQ(0.0f, p1.f32);
	EXPECT_EQ(0.0,  p1.f64);
}

TEST_F( DLText, basic_bool_in_bitfield )
{
	const char* test_bits_text =
	"{"
		"\"type\" : \"TestBits\","
		"\"data\" : {"
			"\"Bit1\" : false,"
			"\"Bit2\" : true,"
			"\"Bit3\" : false,"
			"\"Bit4\" : true,"
			"\"Bit5\" : false,"
			"\"Bit6\" : true,"
			"\"make_it_uneven\" : true"
		"}"
	"}";

	TestBits p1;
	memset( &p1, 0x0, sizeof(TestBits) );

	unsigned char out_text_data[1024];

	EXPECT_DL_ERR_OK( dl_txt_pack( Ctx, test_bits_text, out_text_data, DL_ARRAY_LENGTH(out_text_data), 0x0 ) );
	EXPECT_DL_ERR_OK( dl_instance_load( Ctx, TestBits::TYPE_ID, &p1, sizeof(TestBits), out_text_data, DL_ARRAY_LENGTH(out_text_data), 0x0 ) );

	EXPECT_EQ(0, p1.Bit1);
	EXPECT_EQ(1, p1.Bit2);
	EXPECT_EQ(0, p1.Bit3);
	EXPECT_EQ(1, p1.Bit4);
	EXPECT_EQ(0, p1.Bit5);
	EXPECT_EQ(1, p1.Bit6);
	EXPECT_EQ(1, p1.make_it_uneven);
}

TEST_F( DLText, missing_field_data )
{
	const char* test_text = "{\"type\" : \"Pods\"}";

	Pods p1;
	memset( &p1, 0x0, sizeof(Pods) );

	unsigned char out_text_data[1024];
	EXPECT_DL_ERR_EQ( DL_ERROR_TXT_DATA_SECTION_MISSING, dl_txt_pack( Ctx, test_text, out_text_data, DL_ARRAY_LENGTH(out_text_data), 0x0 ) );
}
