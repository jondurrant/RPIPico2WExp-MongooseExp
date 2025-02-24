/*
 * NVSHTML.h
 *
 * Extend the NVSOnboard storage to allow HTML input form and updates
 *
 *  Created on: 21 Feb 2025
 *      Author: jondurrant
 */

#ifndef EXP_APCONFIG_SRC_NVSHTML_H_
#define EXP_APCONFIG_SRC_NVSHTML_H_

#include "pico/stdlib.h"
#include "NVSOnboard.h"
#include "mongoose.h"

#define NVS_STR_LEN 256

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

	/***
	 * Generate an HTML form page for the current NVS Config
	 * Only Int, Doubles, Strings and Passwords are supported
	 * @param dest - Destination buffer to write to
	 * @param len - length of buffer
	 */
	virtual void toHTML(char *dest, size_t len);

	/***
	 * Use a HTTP GET Query string to update the NVS
	 * @param var = Query
	 */
	virtual void updateQuery(const struct mg_str *var);

	virtual ~NVSHTML();

protected:
	NVSHTML(bool cleanNVS=false);

	/***
	 * Create a form input line for a key in the NVS
	 * @param dest - Dest buffer
	 * @param remLen - Length at start and is update to remaining length after write
	 * @param key - Key in NVS
	 * @param type - type of item
	 * @return pointer to next free location in the buffer
	 */
	virtual char * toHTMLKey(char *dest,  size_t* remLen , const char * key,  nvs_type_t type);

	/***
	 * Crates the start of the HTML form
	 * @param dest - Dest buffer
	 * @param remLen - Length at start and is update to remaining length after write
	 * @return pointer to next free location in the buffer
	 */
	virtual char * formStart(char *dest,  size_t* remLen);

	/***
	 * End of the  HTML form
	 * @param dest - Dest buffer
	 * @param remLen - Length at start and is update to remaining length after write
	 * @return pointer to next free location in the buffer
	 */
	virtual char * formEnd(char *dest,  size_t* remLen);

	/***
	 * Add a line of txt to the destination if space available
	 * @param dest - Dest buffer
	 * @param remLen - Length at start and is update to remaining length after write
	 * @param txt - txt string to add to the buffer
	 * @return pointer to next free location in the buffer
	 */
	virtual char * addTxt(char *dest,  size_t* remLen, const char * txt);

	/***
	 * Add a string input to the html
	 * @param dest buffer to write to
	 * @param key of data item
	 * @param val to write
	 * @param remLen of the buffer, will be updated to new rem
	 * @return next free space in buffer
	 */
	virtual char * html_input_str(
			char *dest,
			const char * key,
			const char *val,
			size_t *remLen
			);

	/***
	 * Add a password input to the html
	 * @param dest buffer to write to
	 * @param key of data item
	 * @param val to write
	 * @param remLen of the buffer, will be updated to new rem
	 * @return next free space in buffer
	 */
	virtual char *html_input_pwd(
			char *dest,
			const char * key,
			const char *val,
			size_t *remLen
			);

	/***
	 * Add a number input to the html
	 * different int sizes are converted into min and max ranges
	 * If min and max are same then no range is put on the input
	 * @param dest buffer to write to
	 * @param key of data item
	 * @param val to write
	 * @param min minimum value
	 * @param max maxmimum value
	 * @param remLen of the buffer, will be updated to new rem
	 * @return next free space in buffer
	 */
	virtual char * html_input_num(
			char *dest,
			const char * key,
			const int64_t val,
			size_t *remLen,
			int64_t min = 0,
			int64_t max = 0
			);

	/***
	 * Add an HTML label
	 * @param dest buffer to write to
	 * @param key of the item, will be name of the Label
	 * @param remLen - remaining buffer space,  updated after write
	 * @return  next free space in buffer
	 */
	virtual char *html_label(
			char *dest,
			const char * key,
			size_t *remLen
		);

	/***
	 * Add a form input for a double to the buffer
	 * @param dest - buffer
	 * @param key - key of the item
	 * @param val - current value
	 * @param remLen - length of buffer remaining, will be updated
	 * @return next free location in buffer
	 */
	virtual char * html_input_double(
			char *dest,
			const char * key,
			const double val,
			size_t *remLen
			);

	/***
	 * Add a form input for a double to the buffer
	 * @param dest - buffer
	 * @param key - key of the item
	 * @param val - current value
	 * @param remLen - length of buffer remaining, will be updated
	 * @return next free location in buffer
	 */
	virtual char * html_input_bool(
			char *dest,
			const char * key,
			const bool val,
			size_t *remLen
			);


	/***
	 * Add the HTML page begining
	 * @param dest - buffer
	 * @param remLen of the buffer will be updated
	 * @return next fee location in buffer
	 */
	virtual char * htmlStart(char *dest,  size_t* remLen);

	/***
	 * Add teh HTML page end
	 * @param dest - buffer
	 * @param remLen of the buffer, will be updated
	 * @return next free location of the bugger
	 */
	virtual char * htmlEnd(char *dest,  size_t* remLen);

	/***
	 * Update an NVS key based on value in a HTML GET query
	 * @param key to update
	 * @param type of the nvs value
	 * @param var query
	 */
	virtual void updateKey(const char * key,  nvs_type_t type, const struct mg_str *var);

	/***
	 * Update an NVS key based on value in a  string
	 * @param key to update
	 * @param type of the nvs value
	 * @param txt int as string
	 */
	virtual void  updateKeyNum(
			const char * key,
			nvs_type_t type,
			const char * txt
			);

private:
	static NVSHTML * pSingleton;

};

#endif /* EXP_APCONFIG_SRC_NVSHTML_H_ */
