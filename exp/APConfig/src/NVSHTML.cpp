/*
 * NVSHTML.cpp
 *
 *  Created on: 21 Feb 2025
 *      Author: jondurrant
 */

#include "NVSHTML.h"
#include <inttypes.h>

NVSHTML * NVSHTML::pSingleton = NULL;

NVSHTML::NVSHTML(bool cleanNVS): NVSOnboard(cleanNVS) {
	//NOP
}

NVSHTML::~NVSHTML() {
	// TODO Auto-generated destructor stub
}

NVSHTML * NVSHTML:: getInstance(bool cleanNVS){
	if (NVSHTML::pSingleton == NULL) {
		NVSHTML::pSingleton = new NVSHTML(cleanNVS);
	}
	return NVSHTML::pSingleton;
}

void NVSHTML::toHTML(char *dest, size_t len){

	char *res = dest;
	size_t remLen = len;

	res = htmlStart(res, &remLen);
	res = formStart(res, &remLen);

	map<string, nvs_entry_t *>::iterator it = xClean.begin();
	while (it != xClean.end()){
		if (xDirty.count(it->first) == 0){
			//CLEAN
			res = toHTMLKey(
					res,
					&remLen ,
					it->first.c_str(),
					it->second->type
					);

		} else {
			if (xDirty[it->first]->type != NVS_TYPE_ERASE){
				//DIRTY
				res = toHTMLKey(
						res,
						&remLen ,
						it->first.c_str(),
						it->second->type
						);
			}
		}
		it++;
	}

	 it = xDirty.begin();
	while (it != xDirty.end()){
		if (xClean.count(it->first) == 0){
			if (it->second->type != NVS_TYPE_ERASE ){
				//NEW
				res = toHTMLKey(
						res,
						&remLen ,
						it->first.c_str(),
						it->second->type
						);
			}
		}
		it++;
	}

	res = formEnd(res, &remLen);
	res = htmlEnd(res, &remLen);
}


char *NVSHTML:: toHTMLKey(
		char *dest,
		size_t* remLen ,
		const char * key,
		nvs_type_t type){


	char * res = dest;

	res = html_label(res, key,remLen);

	switch(type){
	case NVS_TYPE_BOOL:{
			bool b;
			if (get_bool ( key, &b) == NVS_OK){
				res = html_input_bool(
						res,
						key,
						b,
						remLen
						);
			}
			break;
		}
	case NVS_TYPE_U8:{
		uint8_t v;
		if (get_u8 ( key, &v) == NVS_OK){
				res = html_input_num(
						res,
						key,
						(int64_t)v,
						remLen,
						0, 0xff
						);
		}
		break;
	}
	case NVS_TYPE_I8:{
		int8_t v;
		if (get_i8 ( key, &v) == NVS_OK){
			res = html_input_num(
				res,
				key,
				(int64_t)v,
				remLen,
				-128, 127
				);
		}
		break;
	}
	case  NVS_TYPE_U16:{
		uint16_t v;
		if (get_u16 ( key, &v) == NVS_OK){
			res = html_input_num(
					res,
					key,
					(int64_t)v,
					remLen,
					0, 0xFFFF
					);
		}
		break;
	}
	case NVS_TYPE_I16:{
		int16_t v;
		if (get_i16 ( key, &v) == NVS_OK){
			res = html_input_num(
					res,
					key,
					(int64_t)v,
					remLen,
					-32768, 32767
					);
		}
		break;
	}
	case NVS_TYPE_U32:{
		uint32_t v;
		if (get_u32 ( key, &v) == NVS_OK){
			res = html_input_num(
					res,
					key,
					(int64_t)v,
					remLen,
					0, 0xFFFFFFFF
					);
		}
		break;
	}
	case NVS_TYPE_I32:{
		int32_t v;
		if (get_i32 ( key, &v) == NVS_OK){
			res = html_input_num(
					res,
					key,
					(int64_t)v,
					remLen,
					-2147483648,
					2147483647
					);
		}
		break;
	}
	case NVS_TYPE_U64:{
		uint64_t v;
		if (get_u64 ( key, &v) == NVS_OK){
			//Note can'te cope with full u64
			res = html_input_num(
					res,
					key,
					(int64_t)v,
					remLen,
					0, 0xEFFFFFFFFFFFFFFF
					);
		}
		break;
	}
	case NVS_TYPE_I64:{
		int64_t v;
		if (get_i64 ( key, &v) == NVS_OK){
			res = html_input_num(
					res,
					key,
					(int64_t)v,
					remLen,
					0,
					0
					);
		}
		break;
	}
	case NVS_TYPE_DOUBLE:{
		double v;
		if (get_double( key, &v) == NVS_OK){
			res = html_input_double(
					res,
					key,
					v,
					remLen
					);
		}
		break;
	}
	case NVS_TYPE_STR:{
		char v[NVS_STR_LEN];
		size_t l = NVS_STR_LEN;
		if (get_str( key, v, &l) == NVS_OK){
			res = html_input_str(
					res,
					key,
					v,
					remLen
					);
			}
		break;
	}
	case NVS_TYPE_PWD:{
			char v[NVS_STR_LEN];
			size_t l = NVS_STR_LEN;
			if (get_pwd( key, v, &l) == NVS_OK){
					res = html_input_pwd(
							res,
							key,
							v,
							remLen
							);
				}
			break;
		}
	case NVS_TYPE_BLOB:
		break;
	case NVS_TYPE_ERASE:
		break;
	case NVS_TYPE_ANY:
		break;
	}

	return res;
}


char * NVSHTML::formStart(char *dest,  size_t* remLen){
	const char txt[] = "<form action=\"/setConfig\">\n";
	return addTxt(dest, remLen, txt);
}

char * NVSHTML::addTxt(char *dest,  size_t* remLen, const char * txt){
	size_t l = strlen(txt);

	//printf("addTxt %d: %s\n", *remLen, txt);

	if (*remLen > l){
		strcpy(dest, txt);
		*remLen = *remLen - l;
		return &dest[l];
	} else {
		printf("ERROR: addTxt ran out of buffer space at: %s\n", txt);
	}
	return dest;
}

char * NVSHTML::formEnd(char *dest,  size_t* remLen){
	const char txt[] =
			"<input type=\"submit\" value=\"Submit\">\n"
			"</form>\n";
	return addTxt(dest, remLen, txt);
}

char * NVSHTML::html_input_str(
				char *dest,
				const char * key,
				const char *val,
				size_t *remLen
				){
	const char txt[] = "<input type=\"text\" id=\"%s\" name=\"%s\" value=\"%s\"><br>\n";
	char buf[NVS_STR_LEN];
	char * res = dest;

	sprintf(buf, txt, key, key, val);
	res = addTxt(res, remLen, buf);
	return res;
}

char * NVSHTML::html_input_pwd(
				char *dest,
				const char * key,
				const char *val,
				size_t *remLen
				){
	const char txt[] = "<input type=\"password\" id=\"%s\" name=\"%s\"  value=\"########\"><br>\n";
	char buf[NVS_STR_LEN];
	char * res = dest;

	sprintf(buf, txt, key, key);
	res = addTxt(res, remLen, buf);
	return res;
}

char * NVSHTML::html_input_num(
				char *dest,
				const char * key,
				const int64_t val,
				size_t *remLen,
				int64_t min,
				int64_t max
				){
	const char txtRange[] = "<input type=\"number\" id=\"%s\" name=\"%s\" value=\"%"
			PRId64 "\" min=\"%" PRId64 "\" max=\"%" PRId64 "\"><br>\n";
	const char txt[] = "<input type=\"number\" id=\"%s\" name=\"%s\" value=\"%"
			PRId64 "\"><br>\n";
	char buf[NVS_STR_LEN];
	char * res = dest;

	if (min == max){
		sprintf(buf, txt, key, key, val);
	} else {
		sprintf(buf, txtRange, key, key, val, min, max);
	}
	res = addTxt(res, remLen, buf);
	return res;
}

char * NVSHTML::html_input_double(
				char *dest,
				const char * key,
				const double val,
				size_t *remLen
				){
	const char txt[] = "<input type=\"number\" id=\"%s\" name=\"%s\" "
			"value=\"%f\" step=\"0.001\" \"><br>\n";
	char buf[NVS_STR_LEN];
	char * res = dest;

	sprintf(buf, txt, key, key, val);
	res = addTxt(res, remLen, buf);
	return res;
}

char * NVSHTML::html_input_bool(
				char *dest,
				const char * key,
				const bool val,
				size_t *remLen
				){
	const char txt[] = "<input type=\"checkbox\" id=\"%s\" name=\"%s\" "
			"value=\"true\" %s \"><br>\n";
	char buf[NVS_STR_LEN];
	char * res = dest;

	if (val){
		sprintf(buf, txt, key, key, "checked");
	} else {
		sprintf(buf, txt, key, key, "");
	}
	res = addTxt(res, remLen, buf);
	return res;
}

char * NVSHTML::html_label(
			char *dest,
			const char * key,
			size_t *remLen
		){
	const char label[] =
				"<label for=\"%s\">%s: </label>\n";
	char buf[NVS_STR_LEN];
	char * res = dest;

	sprintf(buf, label, key, key);
	res = addTxt(res, remLen, buf);
	return res;
}


char * NVSHTML::htmlStart(char *dest,  size_t* remLen){
	const char txt[] =
						"<html>\n"
						"<body>\n"
						"<h1>Configuration</h1>";
			char * res = dest;
			res = addTxt(res, remLen, txt);
			return res;
}

char * NVSHTML::htmlEnd(char *dest,  size_t* remLen){
	const char txt[] =
					"</body>\n"
					"</html>";
		char * res = dest;
		res = addTxt(res, remLen, txt);
		return res;
}


void NVSHTML::updateQuery(const struct mg_str *var){
	map<string, nvs_entry_t *>::iterator it = xClean.begin();
	while (it != xClean.end()){
		if (xDirty.count(it->first) == 0){
			//CLEAN
			updateKey(
					it->first.c_str(),
					it->second->type,
					var
					);

		} else {
			if (xDirty[it->first]->type != NVS_TYPE_ERASE){
				//DIRTY
				updateKey(
					it->first.c_str(),
					it->second->type,
					var
					);
			}
		}
		it++;
	}

	 it = xDirty.begin();
	while (it != xDirty.end()){
		if (xClean.count(it->first) == 0){
			if (it->second->type != NVS_TYPE_ERASE ){
				//NEW
				updateKey(
					it->first.c_str(),
					it->second->type,
					var
					);
			}
		}
		it++;
	}
	commit();
}

void NVSHTML::updateKey(
		const char * key,
		nvs_type_t type,
		const struct mg_str *var){
	char buf[NVS_STR_LEN];

	int l = mg_http_get_var(var,  key,  buf , NVS_STR_LEN);
	if (l >= 0){
		if (type == NVS_TYPE_STR){
			set_str (key,  buf);
		}
		if (type == NVS_TYPE_PWD){
			set_pwd (key,  buf);
		}
		if (type <= NVS_TYPE_I64){
			updateKeyNum(key, type, buf);
		}
		if (type == NVS_TYPE_DOUBLE){
			double d = atof(buf);
			set_double(key, d);
		}
		if (type == NVS_TYPE_BOOL){
			set_bool(key, true);
		}
	} else {
		if (type == NVS_TYPE_BOOL){
			set_bool(key, false);
		}
	}
}

void NVSHTML::updateKeyNum(
		const char * key,
		nvs_type_t type,
		const char * txt
		){

	char* endptr = NULL;
	int64_t v = strtoimax(txt, &endptr, 10);
	if (endptr != txt){
		switch(type){
		case NVS_TYPE_U8:
			set_u8(key, (uint8_t)v);
			break;
		case NVS_TYPE_I8:
			set_i8(key, (int8_t)v);
			break;
		case NVS_TYPE_U16:
			set_u16(key, (uint16_t)v);
			break;
		case NVS_TYPE_I16:
			set_i16(key, (int16_t)v);
			break;
		case NVS_TYPE_U32:
			set_u32(key, (uint32_t)v);
			break;
		case NVS_TYPE_I32:
			set_i32(key, (int32_t)v);
			break;
		case NVS_TYPE_U64:
			set_u64(key, (uint64_t)v);
			break;
		case NVS_TYPE_I64:
			set_i64(key, (int64_t)v);
			break;
		}
	}
}

