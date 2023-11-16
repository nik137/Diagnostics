package com.ap_impulse.websocket;

/*Щоб змоделювати повідомлення, яке несе ім’я, ви можете створити звичайний старий об’єкт Java із властивістю name 
 * та відповідним методом getName() у вигляді наступного списку */
/**
* ValuesMessage.java Об'єкт що приймає запит - json - об'єкт від клієнта. 
* <p>
* Об'єкт що приймає запит - json - об'єкт від клієнта. (ім'я змінної, та якійсь текст)  
* <pre>
* {
     "name": "text",
     "text": "text"
  }
* </pre>
* @param  name, text
* @return name, text
*/ 
public class ValuesMessage {

	private String name;
	private String text;

	public ValuesMessage() {
	}

	public ValuesMessage(String name) {
		this.name = name;
	}
	
	public ValuesMessage(String name, String text) {
		this.name = name;
		this.text = text;
	}

	public String getName() {
		return name ;
	}
	
	public String getText() {
	    return text;
	}

	public void setName(String name) {
		this.name = name;
		
	}
}
