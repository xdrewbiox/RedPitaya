/**
 * $Id: $
 *
 * @brief Red Pitaya Scpi server dpin SCPI commands implementation
 *
 * @Author Red Pitaya
 *
 * (c) Red Pitaya  http://www.redpitaya.com
 *
 * This part of code is written in C programming language.
 * Please visit http://en.wikipedia.org/wiki/C_(programming_language)
 * for more details on the language used herein.
 */

#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scpi/scpi.h"
#include "rp.h"

#include "utils.h"
#include "dpin.h"


/**
 * Returns Digital Pin state to SCPI context
 * @param context SCPI context
 * @return success or failure
 */
scpi_result_t RP_DigitalPinGetStateQ(scpi_t * context) {
    const char * param;
    size_t param_len;

	char port[15];

    // read first parameter PORT (LED1, LED2, ...)
    if (!SCPI_ParamString(context, &param, &param_len, true)) {
    	syslog(LOG_ERR, "*MEAS:DIG:DATA:BIT? is missing first parameter.");
    	return SCPI_RES_ERR;
    }
    strncpy(port, param, param_len);
    port[param_len] = '\0';

    // Convert port into pin id
    rp_dpin_t pin;
    if (getRpPin(port, &pin)) {
    	syslog(LOG_ERR, "*MEAS:DIG:DATA:BIT? parameter port is invalid.");
    	return SCPI_RES_ERR;
    }

    // Now get the pin state
    rp_pinState_t state;
    int result = rp_DpinGetState(pin, &state);

    if (RP_OK != result)
    {
    	syslog(LOG_ERR, "*MEAS:DIG:DATA:BIT? Failed to get pin state: %s", rp_GetError(result));
    	return SCPI_RES_ERR;
    }

    // Return back result
    SCPI_ResultInt(context, (state == RP_HIGH ? 1 : 0));

	syslog(LOG_INFO, "*MEAS:DIG:DATA:BIT? Successfully returned port %s value %d.", port, (state == RP_HIGH ? 1 : 0));

    return SCPI_RES_OK;
}

/**
 *  Sets Digital Pin to state High/Low
 * @param context SCPI context
 * @return success or failure
 */
scpi_result_t RP_DigitalPinSetState(scpi_t * context) {
    const char * param;
    size_t param_len;

    int32_t bit;
	char port[15];

    // read first parameter PORT (LED1, LED2, ...)
    if (!SCPI_ParamString(context, &param, &param_len, true)) {
    	syslog(LOG_ERR, "*SOUR:DIG:DATA:BIT is missing first parameter.");
    	return SCPI_RES_ERR;
    }
    strncpy(port, param, param_len);
    port[param_len] = '\0';

    // read second parameter BIT (1 -> HIGH; 0->LOW)
    if(!SCPI_ParamInt(context, &bit, true)) {
    	syslog(LOG_ERR, "*SOUR:DIG:DATA:BIT is missing second parameter.");
    	return SCPI_RES_ERR;
    }

    // Convert port into pin id
    rp_dpin_t pin;
    if (getRpPin(port, &pin)) {
    	syslog(LOG_ERR, "*SOUR:DIG:DATA:BIT parameter port is invalid.");
    	return SCPI_RES_ERR;
    }

    // Verify if bit value is valid
    if (bit !=0 && bit != 1) {
    	syslog(LOG_ERR, "*SOUR:DIG:DATA:BIT parameter bit is invalid.");
    }

    // Now set the pin state
    int result = rp_DpinSetState(pin, (bit == 0 ? RP_LOW : RP_HIGH));

    if (RP_OK != result)
	{
		syslog(LOG_ERR, "*SOUR:DIG:DATA:BIT Failed to set pin state: %s", rp_GetError(result));
		return SCPI_RES_ERR;
	}

	syslog(LOG_INFO, "*SOUR:DIG:DATA:BIT Successfully set port %s to value %d.", port, (bit == RP_HIGH ? 1 : 0));


	SET_OK(context);
}

