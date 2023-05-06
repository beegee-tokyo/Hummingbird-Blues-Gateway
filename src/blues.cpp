/**
 * @file blues.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Blues.IO NoteCard handler
 * @version 0.1
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"
#include "product_uid.h"

// #define usbSerial Serial

#define myProductID PRODUCT_UID

Notecard notecard;

J *req;

bool init_blues(void)
{
#ifdef usbSerial
	delay(2500);
	usbSerial.begin(115200);
	notecard.setDebugOutputStream(usbSerial);
#endif

	Wire.begin();
	notecard.begin();

	MYLOG("BLUES", "Set Product ID");
	req = notecard.newRequest("hub.set");

	JAddStringToObject(req, "product", myProductID);
	JAddStringToObject(req, "mode", "continuous");
	// if (!notecard.sendRequest(req))
	if (!blues_send_req())
	{
		MYLOG("BLUES", "hub.set request failed");
		return false;
	}

	/*************************************************/
	/* If the Notecard is properly setup, there is   */
	/* need to setup the APN and card mode on every  */
	/* restart! It will reuse the APN and mode that  */
	/* was originally setup.                         */
	/*************************************************/
	/* If using the built-in eSIM card from Blues.IO */
	/* these code lines should be complete removed!  */
	/*************************************************/
	MYLOG("BLUES", "Set APN");
	// {“req”:”card.wireless”}
	req = notecard.newRequest("card.wireless");
	// For SMART
	// JAddStringToObject(req, "apn", "internet");
	// JAddStringToObject(req, "mode", "a");
	// For Monogoto
	JAddStringToObject(req, "apn", "data.mono");
	JAddStringToObject(req, "mode", "a");
	// if (!notecard.sendRequest(req))
	if (!blues_send_req())
	{
		MYLOG("BLUES", "card.wireless request failed");
		return false;
	}
	/*************************************************/
	/* End of code block to be removed               */
	/*************************************************/

	MYLOG("BLUES", "Create template");
	req = notecard.newRequest("note.add");
	if (req != NULL)
	{

		// Create the body for a template that will be used to send notes below
		J *body = JCreateObject();
		if (body != NULL)
		{
			JAddStringToObject(body, "node_id", "AAAAAAAAAAAAAAAA");

			// Define the JSON template
			JAddNumberToObject(body, "voltage_1", 1.11);	  // floating point (double)
			JAddNumberToObject(body, "humidity_2", 1.11);	  // floating point (double)
			JAddNumberToObject(body, "temperature_3", 1.11);  // floating point (double)
			JAddNumberToObject(body, "humidity_6", 1.11);	  // floating point (double)
			JAddNumberToObject(body, "temperature_7", 1.11);  // floating point (double)
			JAddNumberToObject(body, "barometer_8", 1.11);	  // floating point (double)
			JAddNumberToObject(body, "illuminance_15", 1.11); // floating point (double)
			JAddNumberToObject(body, "voc_16", 1);			  // integer
			JAddNumberToObject(body, "concentration_35", 1);  // integer
			JAddNumberToObject(body, "voc_40", 1);			  // integer
			JAddNumberToObject(body, "voc_41", 1);			  // integer
			JAddNumberToObject(body, "voc_42", 1);			  // integer
			JAddNumberToObject(body, "presence_48", 1);		  // integer

			// Add the body to the request
			JAddItemToObject(req, "body", body);
		}

		JAddStringToObject(req, "node_id", "AAAAAAAAAAAAAAAA");

		// Register the template in the output queue notefile
		JAddStringToObject(req, "file", "sensors.qo");
		JAddBoolToObject(req, "sync", true);
		JAddBoolToObject(req, "template", true);
		if (!blues_send_req())
		{
			MYLOG("BLUES", "Set template failed");
		}
	}
	return true;
}

bool blues_start_req(String request_name)
{
	req = notecard.newRequest(request_name.c_str());
	if (req != NULL)
	{
		return true;
	}
	return false;
}

bool blues_send_req(void)
{
	char *json = JPrintUnformatted(req);
	MYLOG("BLUES", "Card request = %s", json);

	J *rsp;
	rsp = notecard.requestAndResponse(req);
	if (rsp == NULL)
	{
		return false;
	}
	json = JPrintUnformatted(rsp);
	MYLOG("BLUES", "Card response = %s", json);
	notecard.deleteResponse(rsp);

	return true;
}
