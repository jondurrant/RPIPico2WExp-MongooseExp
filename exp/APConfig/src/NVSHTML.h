/*
 * NVSHTML.h
 *
 *  Created on: 21 Feb 2025
 *      Author: jondurrant
 */

#ifndef EXP_APCONFIG_SRC_NVSHTML_H_
#define EXP_APCONFIG_SRC_NVSHTML_H_

#include "pico/stdlib.h"
#include "NVSOnboard.h"
#include "mongoose.h"

#define NVS_STR_LEN 80

class NVSHTML  : public NVSOnboard {
public:

	/***
	 * Singleton to return the current instance
	 * @param cleanNVS - Only usable on first call.
	 * If true returns a clean NVS without reading the Flash
	 * Used mainly for rest purposes.
	 * Default is false
	 * @return the NVSOnboard instance
	 */
	static NVSHTML * getInstance(bool cleanNVS=false);

	virtual void toHTML(char *dest, size_t len);

	virtual void updateQuery(const struct mg_str *var);

	virtual ~NVSHTML();

protected:
	NVSHTML(bool cleanNVS=false);
	virtual char * toHTMLKey(char *dest,  size_t* remLen , const char * key,  nvs_type_t type);

	virtual char * formStart(char *dest,  size_t* remLen);
	virtual char * formEnd(char *dest,  size_t* remLen);
	virtual char * addTxt(char *dest,  size_t* remLen, const char * txt);

	virtual char * html_input_str(
			char *dest,
			const char * key,
			const char *val,
			size_t *remLen
			);
	virtual char *html_input_pwd(
			char *dest,
			const char * key,
			const char *val,
			size_t *remLen
			);
	virtual char *html_label(
			char *dest,
			const char * key,
			size_t *remLen
		);

	virtual char * htmlStart(char *dest,  size_t* remLen);
	virtual char * htmlEnd(char *dest,  size_t* remLen);


	virtual void updateKey(const char * key,  nvs_type_t type, const struct mg_str *var);


private:
	static NVSHTML * pSingleton;

};

#endif /* EXP_APCONFIG_SRC_NVSHTML_H_ */
